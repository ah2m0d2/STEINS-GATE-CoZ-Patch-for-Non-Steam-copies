#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QFile>
#include <QDataStream>
#include "receipt.h"

class ReceiptWriter : public QObject {
    Q_OBJECT

   public:
    explicit ReceiptWriter(QObject* parent = 0) : QObject(parent) {
        _store.version = Receipt::FileFormatVersion;
    };
    ~ReceiptWriter(){};

    void open(const QString& dir);
    void close();

    // Path is resolved with globalFs to an absolute path
    void logFileCreate(const QString& path);
    void logRegKeyCreate(Registry::RootKey root, const QString& key,
                         bool use64bit);
    void logRegValueCreate(Registry::RootKey root, const QString& key,
                           bool use64bit, const QString& valName);

   private:
    struct Store {
        qint32 version;
        QSet<QString> filesCreated;
        QSet<RegKeyRecord> regKeysCreated;
        QSet<RegValRecord> regValsCreated;
    };

    Store _store;
    bool _isLogging = false;

    QDataStream _out;
    QFile* _outFile = nullptr;

    Store loadOldLog(const QString& path);

    void writeLogFileCreate(const QString& path);
    void writeLogRegKeyCreate(const RegKeyRecord& record);
    void writeLogRegValueCreate(const RegValRecord& record);
};