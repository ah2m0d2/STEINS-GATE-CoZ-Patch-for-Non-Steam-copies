#include "receiptwriter.h"
#include "installerapplication.h"
#include "fs.h"
#include "util/exception.h"
#include <QFileInfo>
#include "receipt.h"

// NOTE: QIODevice::Unbuffered is not supported on Windows, hence we still need to flush() manually

void ReceiptWriter::open(const QString& dir) {
    if (_outFile != nullptr) {
        throw NgException("Tried to open receipt twice");
    }

    _isLogging = true;
    ngApp->globalFs()->createDirectory(dir);
    QString filePath = ngApp->globalFs()->expandedPath(dir + "/ngreceipt.bin");

    QSet<QString> filesToWrite = _store.filesCreated;
    QSet<RegKeyRecord> regKeysToWrite = _store.regKeysCreated;
    QSet<RegValRecord> regValsToWrite = _store.regValsCreated;

    if (ngApp->globalFs()->pathExists(filePath)) {
        Store oldStore;
        try {
            oldStore = loadOldLog(filePath);
        } catch (const NgException& ex) {
            _isLogging = false;
            throw;
        }

        _outFile = new QFile(filePath, this);
        if (!_outFile->open(QIODevice::ReadWrite | QIODevice::Append |
                            QIODevice::Unbuffered)) {
            _isLogging = false;
            delete _outFile;
            _outFile = nullptr;
            throw NgException(
                QString("Couldn't write to receipt at %1").arg(filePath));
        }
        // TODO handle version upgrades?
        _out.setDevice(_outFile);
        _out.setVersion(Receipt::DataStreamVersion);

        filesToWrite.subtract(oldStore.filesCreated);
        regKeysToWrite.subtract(oldStore.regKeysCreated);
        regValsToWrite.subtract(oldStore.regValsCreated);
    } else {
        _outFile = new QFile(filePath, this);
        if (!_outFile->open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
            _isLogging = false;
            delete _outFile;
            _outFile = nullptr;
            throw NgException(
                QString("Couldn't create receipt at %1").arg(filePath));
        }
        _out.setDevice(_outFile);
        _out.setVersion(Receipt::DataStreamVersion);
        _out << Receipt::FileFormatVersion;
        logFileCreate(filePath);
    }

    for (const QString& f : filesToWrite) {
        writeLogFileCreate(f);
    }
    for (const RegKeyRecord& r : regKeysToWrite) {
        writeLogRegKeyCreate(r);
    }
    for (const RegValRecord& r : regValsToWrite) {
        writeLogRegValueCreate(r);
    }
}

void ReceiptWriter::close() {
    if (_outFile == nullptr | !_outFile->isOpen()) {
        throw NgException("Tried to close receipt that wasn't open");
    }
    _outFile->close();
    delete _outFile;
    _outFile = nullptr;
    _isLogging = false;
}

void ReceiptWriter::logFileCreate(const QString& path) {
    if (!_isLogging) return;
    auto fi = QFileInfo(ngApp->globalFs()->expandedPath(path));
    QString expandedPath;
    if (fi.isSymLink()) {
        // canonicalFilePath() follows symlinks / Windows .lnk shortcuts,
        // so we'll have to make do with this I guess
        expandedPath = fi.absoluteFilePath();
    } else {
        expandedPath = fi.canonicalFilePath();
    }
    if (!_store.filesCreated.contains(expandedPath)) {
        _store.filesCreated << expandedPath;
        writeLogFileCreate(path);
    }
}

void ReceiptWriter::writeLogFileCreate(const QString& path) {
    if (_outFile != nullptr && _outFile->isOpen()) {
        _out << (int)Receipt::TokenType::File;
        _out << path;
        _outFile->flush();
    }
}

void ReceiptWriter::logRegKeyCreate(Registry::RootKey root, const QString& key,
                                    bool use64bit) {
    if (!_isLogging) return;
    RegKeyRecord record;
    record.root = root;
    record.key = key.toLower();
    record.use64bit = use64bit;
    if (!_store.regKeysCreated.contains(record)) {
        _store.regKeysCreated << record;
        writeLogRegKeyCreate(record);
    }
}
void ReceiptWriter::writeLogRegKeyCreate(const RegKeyRecord& record) {
    if (_outFile != nullptr && _outFile->isOpen()) {
        _out << (int)Receipt::TokenType::RegKey;
        _out << (int)record.root;
        _out << record.key;
        _out << record.use64bit;
        _outFile->flush();
    }
}
void ReceiptWriter::logRegValueCreate(Registry::RootKey root,
                                      const QString& key, bool use64bit,
                                      const QString& valName) {
    if (!_isLogging) return;
    RegValRecord record;
    record.root = root;
    record.key = key.toLower();
    record.use64bit = use64bit;
    record.valName = valName.toLower();
    if (!_store.regValsCreated.contains(record)) {
        _store.regValsCreated << record;
        writeLogRegValueCreate(record);
    }
}
void ReceiptWriter::writeLogRegValueCreate(const RegValRecord& record) {
    if (_outFile != nullptr && _outFile->isOpen()) {
        _out << (int)Receipt::TokenType::RegValue;
        _out << (int)record.root;
        _out << record.key;
        _out << record.use64bit;
        _out << record.valName;
        _outFile->flush();
    }
}

ReceiptWriter::Store ReceiptWriter::loadOldLog(const QString& path) {
    Receipt oldReceipt(path);
    Store result;
    result.version = oldReceipt.version();
    for (const auto& entry : oldReceipt.entries()) {
        switch (entry.type) {
            case Receipt::TokenType::File:
                result.filesCreated << *(QString*)entry.data;
                break;
            case Receipt::TokenType::RegKey:
                result.regKeysCreated << *(RegKeyRecord*)entry.data;
                break;
            case Receipt::TokenType::RegValue:
                result.regValsCreated << *(RegValRecord*)entry.data;
                break;
        }
    }
    return result;
}