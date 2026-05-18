#include "txmpkinputstream.h"
#include <util/exception.h>
#include "installerapplication.h"
#include "fs.h"
#include <QtEndian>

static const quint32 mpkMagic =
    qToLittleEndian<quint32>(0x004B504D);  // 'MPK\0'
static const quint16 mpkVersionMinor = qToLittleEndian<quint16>(0);
static const quint16 mpkVersionMajor = qToLittleEndian<quint16>(2);

static const qint64 mpkHeaderSize = 0x40;
static const qint64 mpkEntrySize = 0x100;

TxMpkInputStream::~TxMpkInputStream() { doClose(); }

void TxMpkInputStream::open() {
    QString expandedPath = ngApp->globalFs()->expandedPath(_inPath);
    if (_inFile != nullptr) {
        throw NgException(
            QString("Tried to open MPK entry twice: %1").arg(expandedPath));
    }
    _inFile = new QFile(expandedPath);
    if (!_inFile->open(QIODevice::ReadOnly)) {
        throw NgException(
            QString("Couldn't open MPK entry: %1").arg(expandedPath));
    }

    _inStream = new QDataStream(_inFile);
    _inStream->setByteOrder(QDataStream::LittleEndian);

    quint32 magic;
    (*_inStream) >> magic;

    if (magic != mpkMagic) {
        doClose();
        throw NgException(QString("File is not an MPK: %1").arg(expandedPath));
    }

    quint16 versionMinor;
    (*_inStream) >> versionMinor;
    quint16 versionMajor;
    (*_inStream) >> versionMajor;

    if (versionMinor != mpkVersionMinor || versionMajor != mpkVersionMajor) {
        doClose();
        throw NgException(
            QString("MPK has unsupported version: %1").arg(expandedPath));
    }

    qint64 entryCount;
    (*_inStream) >> entryCount;

    for (qint64 i = 0; i < entryCount; i++) {
        _inFile->seek(mpkHeaderSize + i * mpkEntrySize);
        quint32 compressionMethod;
        (*_inStream) >> compressionMethod;
        quint32 id;
        (*_inStream) >> id;
        if (id == _entry) {
            if (compressionMethod != 0) {
                doClose();
                throw NgException(QString("MPK entry is compressed: %1 in %2")
                                      .arg(_entry)
                                      .arg(expandedPath));
            }
            (*_inStream) >> _offset;
            (*_inStream) >> _size;
            _isOpen = true;
        }
    }

    if (!_isOpen) {
        doClose();
        throw NgException(QString("MPK entry not found: %1 in %2")
                              .arg(_entry)
                              .arg(expandedPath));
    }

    _inFile->seek(_offset);
}

void TxMpkInputStream::close() {
    QString expandedPath = ngApp->globalFs()->expandedPath(_inPath);
    if (_inFile == nullptr || !_inFile->isOpen()) {
        throw NgException(
            QString("Tried to close MPK entry that wasn't open: %1")
                .arg(expandedPath));
    }
    doClose();
}

void TxMpkInputStream::doClose() {
    if (_inStream != nullptr) {
        delete _inStream;
        _inStream = nullptr;
    }
    if (_inFile != nullptr) {
        _inFile->close();
        delete _inFile;
        _inFile = nullptr;
    }
    _isOpen = false;
}

void TxMpkInputStream::seek(qint64 count) {
    QString expandedPath = ngApp->globalFs()->expandedPath(_inPath);
    if (_inFile == nullptr || !_inFile->isOpen()) {
        throw NgException(
            QString("Tried to seek MPK entry that wasn't open: %1")
                .arg(expandedPath));
    }
    qint64 seekStartPos = _inFile->pos() - _offset;
    seekAbs(seekStartPos + count);
}

void TxMpkInputStream::seekAbs(qint64 pos) {
    QString expandedPath = ngApp->globalFs()->expandedPath(_inPath);
    if (_inFile == nullptr || !_inFile->isOpen()) {
        throw NgException(
            QString("Tried to seek MPK entry that wasn't open: %1")
                .arg(expandedPath));
    }
    if (pos >= _size || pos < 0) {
        throw NgException(
            QString("Tried to seek out of bounds of MPK entry: %1 in %2")
                .arg(_entry)
                .arg(expandedPath));
    }
    if (!_inFile->seek(_offset + pos)) {
        throw NgException(QString("Couldn't seek in MPK entry: %1 in %2")
                              .arg(_entry)
                              .arg(expandedPath));
    }
}

qint64 TxMpkInputStream::read(void* buffer, qint64 max) {
    QString expandedPath = ngApp->globalFs()->expandedPath(_inPath);
    if (_inFile == nullptr || !_inFile->isOpen()) {
        throw NgException(
            QString("Tried to read MPK entry that wasn't open: %1 in %2")
                .arg(_entry)
                .arg(expandedPath));
    }

    qint64 end = _offset + _size;
    max = qMin(end - _inFile->pos(), max);
    max = qMax(max, (qint64)0);
    return _inFile->read((char*)buffer, max);
}