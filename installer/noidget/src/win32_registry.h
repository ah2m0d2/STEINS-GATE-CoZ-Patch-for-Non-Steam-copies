#pragma once
#include <QObject>
#include <QString>
#include <QVariant>
#include <QScriptable>
#ifdef Q_OS_WIN32
#include <windows.h>
#endif

/*^jsdoc
 * Registry interface (Win32 only)
 *
 * Key/value creations are logged to the uninstall receipt.
 * 
 * Only `REG_DWORD` and `REG_SZ` values can be queried with this for now.
 * `REG_EXPAND_SZ` values are expanded upon retrieval.
 * 
 * Deleting keys/values or setting them to NULL is currently not supported.
 * 
 * The desired registry view - *64-bit* or *32-bit* - must be explicitly specified.
 * See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724072(v=vs.85).aspx
 * for further information. `use64bit` has no effect on 32-bit Windows.
 * 
 * @class Registry
 * @memberof ng.win32
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class Registry : public QObject, protected QScriptable {
    Q_OBJECT

   public:
    /*^jsdoc
     * @namespace RootKey
     * @memberof ng.win32
     * @static
     * @toplevel
     * @property {number} HKLM - HKEY_LOCAL_MACHINE
     * @property {number} HKCC - HKEY_CURRENT_CONFIG
     * @property {number} HKCU - HKEY_CURRENT_USER
     * @property {number} HKCR - HKEY_CLASSES_ROOT
     * @property {number} HKU - HKEY_USERS
     ^jsdoc*/
    enum class RootKey { HKLM, HKCC, HKCU, HKCR, HKU };
    Q_ENUM(RootKey)
#ifdef Q_OS_WIN32
    explicit Registry(QObject* parent = 0) : QObject(parent) {}
    ~Registry() {}

    Q_INVOKABLE bool keyExists(RootKey root, const QString& key, bool use64bit);
    Q_INVOKABLE bool valueExists(RootKey root, const QString& key,
                                 bool use64bit, const QString& valName);
    Q_INVOKABLE bool createKey(RootKey root, const QString& key, bool use64bit);
    Q_INVOKABLE QVariant value(RootKey root, const QString& key, bool use64bit,
                               const QString& valName);
    Q_INVOKABLE bool setValue(RootKey root, const QString& key, bool use64bit,
                              const QString& valName, const QVariant& value);

    // currently not part of the scripting API
    bool keyHasContent(RootKey root, const QString& key, bool use64bit);
    bool deleteKeyNonRecursive(RootKey root, const QString& key, bool use64bit);
    bool deleteValue(RootKey root, const QString& key, bool use64bit,
                     const QString& valName);

   private:
    HKEY rootKeyToHkey(RootKey root);
#endif
};