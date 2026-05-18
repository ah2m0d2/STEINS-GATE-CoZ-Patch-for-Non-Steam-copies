#include "rollbackreceiptaction.h"
#include "installerapplication.h"
#include "fs.h"
#include "transaction.h"
#include <api/exception.h>
#include "receipt.h"
#include <QDir>
#include "win32_registry.h"

RollbackReceiptAction::RollbackReceiptAction(QObject* parent)
    : TxAction(parent) {
    if (!ngApp->tx()->uninstallMode()) {
        SCRIPT_THROW("Tried to create RollbackReceiptAction in installer mode")
    }
}

void RollbackReceiptAction::prepare() {
    _receipt = new Receipt(ngApp->tx()->receiptPath() + "/ngreceipt.bin", this);
}

qint64 RollbackReceiptAction::calcSubactionCount() {
    return _receipt->entries().count();
}

void RollbackReceiptAction::run() {
    // run in reverse so e.g. all files in a directory are cleared out before we
    // try to delete the directory
    for (auto entry = _receipt->entries().crbegin();
         entry != _receipt->entries().crend(); entry++) {
        switch (entry->type) {
            case Receipt::TokenType::File: {
                QString path = *(QString*)entry->data;
                if (ngApp->globalFs()->pathIsDirectory(path)) {
                    if (QDir(path).isEmpty()) {
                        QDir(path).removeRecursively();
                        emit log(QString("Deleted %1").arg(path));
                    } else {
                        emit log(QString("Not deleting %1: Directory not empty")
                                     .arg(path));
                    }
                } else if (ngApp->globalFs()->pathIsFile(path)) {
                    QFile::remove(path);
                    emit log(QString("Deleted %1").arg(path));
                } else {
                    emit log(
                        QString("Not deleting %1: no such file or directory")
                            .arg(path));
                }
                break;
            }
            case Receipt::TokenType::RegKey: {
#ifdef Q_OS_WIN32
                RegKeyRecord record = *(RegKeyRecord*)entry->data;
                if (!ngApp->registry()->keyExists(record.root, record.key,
                                                  record.use64bit)) {
                    emit log(
                        QString("Not deleting registry key %1\\%2: no such key")
                            .arg(EnumToString(record.root), record.key));
                } else if (ngApp->registry()->keyHasContent(
                               record.root, record.key, record.use64bit)) {
                    emit log(
                        QString("Not deleting registry key %1\\%2: not empty")
                            .arg(EnumToString(record.root), record.key));
                } else {
                    ngApp->registry()->deleteKeyNonRecursive(
                        record.root, record.key, record.use64bit);
                    emit log(QString("Deleted registry key %1\\%2")
                                 .arg(EnumToString(record.root), record.key));
                }
#endif
                break;
            }
            case Receipt::TokenType::RegValue: {
#ifdef Q_OS_WIN32
                RegValRecord record = *(RegValRecord*)entry->data;
                if (!ngApp->registry()->valueExists(record.root, record.key,
                                                    record.use64bit,
                                                    record.valName)) {
                    emit log(QString("Not deleting registry value %1\\%2\\%3: "
                                     "no such value")
                                 .arg(EnumToString(record.root), record.key,
                                      record.valName));
                } else {
                    ngApp->registry()->deleteValue(record.root, record.key,
                                                   record.use64bit,
                                                   record.valName);
                    emit log(QString("Deleted registry value %1\\%2\\%3")
                                 .arg(EnumToString(record.root), record.key,
                                      record.valName));
                }
#endif
                break;
            }
        }
        _subactionProgress++;
        emit subactionProgress(_subactionProgress);
    }
}