#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>
#include "win32_registry.h"

#ifdef Q_OS_WIN32

/*^jsdoc
 * Action to set a value in the registry (Win32 only)
 * 
 * Like {@link ng.win32.Registry#setValue}.
 * 
 * Does not log anything by default.
 * 
 * @class SetRegistryValueAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {ng.win32.RootKey} root
 * @property {string} key
 * @property {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @property {string} valName
 * @property {{string|Number}} value - string or number (converted to unsigned 32-bit integer REG_DWORD)
 ^jsdoc*/
class SetRegistryValueAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(Registry::RootKey root READ root WRITE setRoot)
    Q_PROPERTY(QString key READ key WRITE setKey)
    Q_PROPERTY(bool use64bit READ use64bit WRITE setUse64bit)
    Q_PROPERTY(QString valName READ valName WRITE setValName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)

   public:
    explicit SetRegistryValueAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    Registry::RootKey root() { return _root; }
    void setRoot(Registry::RootKey root) { _root = root; }
    QString key() { return _key; }
    void setKey(const QString& key) { _key = key; }
    bool use64bit() { return _use64bit; }
    void setUse64bit(bool use64bit) { _use64bit = use64bit; }
    QString valName() { return _valName; }
    void setValName(const QString& valName) { _valName = valName; }
    QVariant value() { return _value; }
    void setValue(const QVariant& value) { _value = value; }

   private:
    Registry::RootKey _root;
    QString _key;
    bool _use64bit;
    QString _valName;
    QVariant _value;
};

#endif