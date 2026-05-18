#include "buildmpkaction.h"
#include "installerapplication.h"
#include "fs.h"
#include "receiptwriter.h"
#include "txstream.h"
#include "txfilestream.h"
#include <util/exception.h>
#include <QtEndian>
#include <QFile>

static const qint64 bufferSize = 1024 * 1024;
static const qint64 mpkHeaderSize = 0x40;
static const qint64 mpkEntrySize = 0x100;
static const qint64 padding = 2 * 1024;

static const quint32 mpkMagic =
    qToLittleEndian<quint32>(0x004B504D);  // 'MPK\0'
static const quint16 mpkVersionMinor = qToLittleEndian<quint16>(0);
static const quint16 mpkVersionMajor = qToLittleEndian<quint16>(2);

static qint64 mpkAlign(qint64 n) {
    if (n % padding == 0) return n;
    return n + (padding - (n % padding));
}

qint64 BuildMpkAction::calcSize() {
    qint64 result = 0;
    for (const MpkMetaEntry& entry : _entries) {
        result += entry.displaySize;
    }
    return result;
}

void BuildMpkAction::addEntry(int id, const QString& name,
                              const QString& srcPath, qint64 displaySize) {
    TxFileStream* stream = new TxFileStream();
    stream->setInPath(srcPath);
    addEntry(id, name, stream, displaySize);
}
void BuildMpkAction::addEntry(int id, const QString& name, TxStream* source,
                              qint64 displaySize) {
    MpkMetaEntry entry;
    entry.id = id;
    entry.source = source;
    entry.displaySize = displaySize;
    QByteArray latin1Name = name.toLatin1();
    const char* _name = latin1Name.constData();
    strncpy(entry.name, _name, sizeof(entry.name));
    entry.name[sizeof(entry.name) - 1] = '\0';
    _entries.append(entry);
}

void BuildMpkAction::run() {
    QString archivePath = ngApp->globalFs()->expandedPath(_path);
    emit log(QString("Building archive: %1...").arg(archivePath));
    QFile outFile(archivePath);
    bool archiveIsNew = !outFile.exists();
    if (!outFile.open(QIODevice::WriteOnly)) {
        throw NgException(QString("Could not write file: %1").arg(archivePath));
    }
    emit log(QString("(File is new: %1)").arg(archiveIsNew), true);
    if (archiveIsNew) {
        ngApp->receipt()->logFileCreate(archivePath);
    }

    // write header
    outFile.write((const char*)&mpkMagic, sizeof(mpkMagic));
    outFile.write((const char*)&mpkVersionMinor, sizeof(mpkVersionMinor));
    outFile.write((const char*)&mpkVersionMajor, sizeof(mpkVersionMajor));
    qint64 outEntryCount = qToLittleEndian((qint64)_entries.count());
    outFile.write((const char*)&outEntryCount, sizeof(outEntryCount));
    // rest is padding

    // write data
    outFile.seek(mpkAlign(mpkHeaderSize + mpkEntrySize * _entries.count()));
    void* buffer = malloc(bufferSize);
    for (MpkMetaEntry& entry : _entries) {
        if (_isCancelled) {
            emit log(QString("MPK build of %1 aborted mid-write after user "
                             "cancelled transaction")
                         .arg(archivePath));
            free(buffer);
            return;
        }
        emit log(QString::fromLatin1(entry.name));
        bool streamWasOpen = entry.source->isOpen();
        emit log(QString("(Stream was open: %1)").arg(streamWasOpen), true);
        if (!streamWasOpen) {
            entry.source->open();
        }
        qint64 bytesRead = 0;
        entry.dataSize = 0;
        entry.dataOffset = outFile.pos();
        while ((bytesRead = entry.source->read(buffer, bufferSize)) > 0) {
            if (_isCancelled) {
                emit log(QString("MPK build of %1 aborted mid-write after user "
                                 "cancelled transaction")
                             .arg(archivePath));
                free(buffer);
                return;
            }
            outFile.write((char*)buffer, bytesRead);
            entry.dataSize += bytesRead;
            emit progress(qMin(_progress + entry.dataSize,
                               _progress + entry.displaySize));
        }
        _progress += entry.displaySize;
        if (!streamWasOpen) {
            entry.source->close();
        }
        outFile.seek(mpkAlign(outFile.pos()));
    }
    free(buffer);

    // write TOC
    emit log("Writing TOC", true);
    for (int i = 0; i < _entries.count(); i++) {
        MpkMetaEntry& entry = _entries[i];
        outFile.seek(mpkHeaderSize + mpkEntrySize * i);
        quint32 outCompressionMethod =
            qToLittleEndian<quint32>(0);  // no compression
        outFile.write((const char*)&outCompressionMethod, sizeof(quint32));
        int outId = qToLittleEndian(entry.id);
        outFile.write((const char*)&outId, sizeof(outId));
        qint64 outOffset = qToLittleEndian(entry.dataOffset);
        outFile.write((const char*)&outOffset, sizeof(outOffset));
        qint64 outSize = qToLittleEndian(entry.dataSize);
        // compressed and uncompressed
        outFile.write((const char*)&outSize, sizeof(outSize));
        outFile.write((const char*)&outSize, sizeof(outSize));
        outFile.write(entry.name);
    }
}