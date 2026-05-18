#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QDataStream>
#include "win32_registry.h"

struct RegKeyRecord {
    Registry::RootKey root;
    QString key;
    bool use64bit;
    bool operator==(const RegKeyRecord& other) const {
        return root == other.root && key == other.key &&
               use64bit == other.use64bit;
    }
};
struct RegValRecord {
    Registry::RootKey root;
    QString key;
    bool use64bit;
    QString valName;
    bool operator==(const RegValRecord& other) const {
        return root == other.root && key == other.key &&
               use64bit == other.use64bit && valName == other.valName;
    }
};

struct ReceiptEntry;

// I don't know what I'm doing help
// https://stackoverflow.com/a/22402626
inline uint qHash(const RegKeyRecord& record, uint seed = 0) {
    return qHash((int)record.root) ^ qHash(record.key) + qHash(record.use64bit);
}
inline uint qHash(const RegValRecord& record, uint seed = 0) {
    return qHash((int)record.root) ^
           qHash(record.key) + qHash(record.use64bit) + qHash(record.valName);
}
inline uint qHash(const QSet<RegKeyRecord>& set, uint seed = 0) {
    return std::accumulate(set.begin(), set.end(), seed,
                           [](uint seed, const RegKeyRecord& value) {
                               return seed + qHash(value);  // or ^
                           });
}
inline uint qHash(const QSet<RegValRecord>& set, uint seed = 0) {
    return std::accumulate(set.begin(), set.end(), seed,
                           [](uint seed, const RegValRecord& value) {
                               return seed + qHash(value);  // or ^
                           });
}

class Receipt : public QObject {
    Q_OBJECT

   public:
    const static qint32 FileFormatVersion = 1;
    // Warning: we cannot read the latter from a file before opening it,
    // so we should not change this in the future
    const static QDataStream::Version DataStreamVersion = QDataStream::Qt_5_9;
    enum class TokenType { File = 0, RegKey = 1, RegValue = 2 };

    explicit Receipt(const QString& filePath, QObject* parent = 0);
    explicit Receipt(const QByteArray& data, QObject* parent = 0);
    ~Receipt();

    const QVector<ReceiptEntry>& entries() { return _entries; }
    qint32 version() { return _version; }

   private:
    QVector<ReceiptEntry> _entries;
    qint32 _version;

    void loadFromData(const QByteArray& data);
};

struct ReceiptEntry {
    Receipt::TokenType type;
    // we should be using QObjects here but no time
    void* data;
};