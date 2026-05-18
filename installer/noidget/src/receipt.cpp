#include "receipt.h"
#include "installerapplication.h"
#include "fs.h"
#include <QFile>
#include <util/exception.h>

Receipt::Receipt(const QString& filePath, QObject* parent) : QObject(parent) {
    QString expandedFilePath = ngApp->globalFs()->expandedPath(filePath);
    QFile inFile(expandedFilePath);
    if (!inFile.open(QIODevice::ReadOnly)) {
        throw NgException(
            QString("Couldn't open receipt at %1").arg(expandedFilePath));
    }
    QByteArray data = inFile.readAll();
    loadFromData(data);
}

Receipt::Receipt(const QByteArray& data, QObject* parent) : QObject(parent) {
    loadFromData(data);
}

Receipt::~Receipt() {
    for (auto& entry : _entries) {
        if (entry.data != nullptr) {
            switch (entry.type) {
                case TokenType::File: {
                    QString* _data = (QString*)entry.data;
                    delete _data;
                    break;
                }
                case TokenType::RegKey: {
                    RegKeyRecord* _data = (RegKeyRecord*)entry.data;
                    delete _data;
                    break;
                }
                case TokenType::RegValue: {
                    RegValRecord* _data = (RegValRecord*)entry.data;
                    delete _data;
                    break;
                }
            }
        }
    }
}

void Receipt::loadFromData(const QByteArray& data) {
    QDataStream in(data);
    in.setVersion(DataStreamVersion);
    in >> _version;
    while (!in.atEnd()) {
        ReceiptEntry entry;
        in >> (qint32&)entry.type;
        switch (entry.type) {
            case TokenType::File: {
                QString* createdFile = new QString;
                in >> *createdFile;
                entry.data = createdFile;
                break;
            }
            case TokenType::RegKey: {
                RegKeyRecord* record = new RegKeyRecord;
                in >> (qint32&)record->root;
                in >> record->key;
                in >> record->use64bit;
                entry.data = record;
                break;
            }
            case TokenType::RegValue: {
                RegValRecord* record = new RegValRecord;
                in >> (qint32&)record->root;
                in >> record->key;
                in >> record->use64bit;
                in >> record->valName;
                entry.data = record;
                break;
            }
        }
        _entries << entry;
    }
}