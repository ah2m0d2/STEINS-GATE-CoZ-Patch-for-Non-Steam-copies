#include "win32_registry.h"
#ifdef Q_OS_WIN32
#include "installerapplication.h"
#include "receiptwriter.h"
#include <QDebug>

/*^jsdoc
 * Does `root\\key` exist?
 * 
 * This will also return `false` if the key is inaccessible for read
 * (though noidget is always run as Administrator).
 * 
 * @method keyExists
 * @memberof ng.win32.Registry
 * @instance
 * @param {ng.win32.RootKey} root
 * @param {string} key
 * @param {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @returns {boolean}
 ^jsdoc*/
bool Registry::keyExists(RootKey root, const QString& key, bool use64bit) {
    // wchar_t is 16 bit on windows, so we can use QString::utf16() directly
    HKEY hKey;
    bool success =
        RegOpenKeyExW(rootKeyToHkey(root), (LPCWSTR)key.utf16(), NULL,
                      KEY_READ | (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY),
                      &hKey) == ERROR_SUCCESS;
    if (success) {
        RegCloseKey(hKey);
    }
    return success;
}

/*^jsdoc
 * Does `valName` in `root\\key` exist?
 * 
 * This will also return `false` if the value is inaccessible for read
 * (though noidget is always run as Administrator) or `keyExists()` returns `false`
 * for this `root\\key`.
 * 
 * @method valueExists
 * @memberof ng.win32.Registry
 * @instance
 * @param {ng.win32.RootKey} root
 * @param {string} key
 * @param {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @param {string} valName
 * @returns {boolean}
 ^jsdoc*/
bool Registry::valueExists(RootKey root, const QString& key, bool use64bit,
                           const QString& valName) {
    HKEY hKey;
    bool success =
        RegOpenKeyExW(rootKeyToHkey(root), (LPCWSTR)key.utf16(), NULL,
                      KEY_READ | (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY),
                      &hKey) == ERROR_SUCCESS;
    if (success) {
        success = RegGetValueW(hKey, NULL, (LPCWSTR)valName.utf16(),
                               RRF_RT_REG_DWORD | RRF_RT_REG_SZ, NULL, NULL,
                               NULL) == ERROR_SUCCESS;
        RegCloseKey(hKey);
    }
    return success;
}

/*^jsdoc
 * Create `root\\key`
 * 
 * Parent keys do not need to exist, they will be created recursively. Returns
 * `false` on error - some of the parent keys may have been created by then.
 * 
 * @method createKey
 * @memberof ng.win32.Registry
 * @instance
 * @param {ng.win32.RootKey} root
 * @param {string} key
 * @param {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @returns {boolean} - `false` on error.
 ^jsdoc*/
bool Registry::createKey(RootKey root, const QString& key, bool use64bit) {
    // While RegCreateKey(Ex) does work recursively, we need to log
    // each subkey creation, so we still need to iterate manually
    QString currentSubkey = "";
    auto subkeys = key.split('\\');
    HKEY hKey;
    DWORD dwDisposition;
    for (auto subkey : subkeys) {
        currentSubkey += subkey;
        if (keyExists(root, currentSubkey, use64bit)) {
            // creation attempts for direct children of HKLM\ etc. will fail
            // so we need to skip those
            currentSubkey += "\\";
            continue;
        }
        bool success =
            RegCreateKeyExW(rootKeyToHkey(root), (LPCWSTR)currentSubkey.utf16(),
                            0, NULL, 0,
                            (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY), 0,
                            &hKey, &dwDisposition) == ERROR_SUCCESS;
        if (success) {
            RegCloseKey(hKey);
            if (dwDisposition == REG_CREATED_NEW_KEY) {
                ngApp->receipt()->logRegKeyCreate(root, key, use64bit);
            }
        } else {
            return false;
        }
        currentSubkey += "\\";
    }
    return true;
}

/*^jsdoc
 * Gets `valName` in `root\\key`
 * 
 * Only `REG_SZ`, `REG_EXPAND_SZ` and `REG_DWORD` supported. `REG_EXPAND_SZ` strings
 * will be expanded.
 * 
 * @method value
 * @memberof ng.win32.Registry
 * @instance
 * @param {ng.win32.RootKey} root
 * @param {string} key
 * @param {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @param {string} valName
 * @returns {{string|Number}} - `undefined` on error, or value
 ^jsdoc*/
QVariant Registry::value(RootKey root, const QString& key, bool use64bit,
                         const QString& valName) {
    HKEY hKey;
    bool success =
        RegOpenKeyExW(rootKeyToHkey(root), (LPCWSTR)key.utf16(), NULL,
                      KEY_READ | (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY),
                      &hKey) == ERROR_SUCCESS;
    QVariant result = QVariant();
    if (success) {
        DWORD type, size;
        success = RegGetValueW(hKey, NULL, (LPCWSTR)valName.utf16(),
                               RRF_RT_REG_DWORD | RRF_RT_REG_SZ, &type, NULL,
                               &size) == ERROR_SUCCESS;
        if (success) {
            if (type == REG_DWORD) {
                DWORD dwResult;
                success = RegGetValueW(hKey, NULL, (LPCWSTR)valName.utf16(),
                                       RRF_RT_REG_DWORD | RRF_RT_REG_SZ, NULL,
                                       &dwResult, &size) == ERROR_SUCCESS;
                if (success) {
                    result = QVariant((int)dwResult);
                }
            } else if (type == REG_SZ) {
                wchar_t* wstr = new wchar_t[size / sizeof(wchar_t)];
                success = RegGetValueW(hKey, NULL, (LPCWSTR)valName.utf16(),
                                       RRF_RT_REG_DWORD | RRF_RT_REG_SZ, NULL,
                                       wstr, &size) == ERROR_SUCCESS;
                if (success) {
                    QString qstr = QString::fromWCharArray(wstr);
                    result = QVariant(qstr);
                }
                delete[] wstr;
            }
        }
        RegCloseKey(hKey);
    }
    return result;
}

/*^jsdoc
 * Sets `valName` in `root\\key`
 * 
 * Parent keys do not need to exist, they will be created recursively. Returns
 * `false` on error - some of the parent keys may have been created by then.
 * 
 * TODO: test overwriting REG_EXPAND_SZ
 * 
 * @method setValue
 * @memberof ng.win32.Registry
 * @instance
 * @param {ng.win32.RootKey} root
 * @param {string} key
 * @param {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @param {string} valName
 * @param {{string|Number}} value - string or number (converted to unsigned 32-bit integer REG_DWORD)
 * @returns {boolean} - `false` on error
 ^jsdoc*/
bool Registry::setValue(RootKey root, const QString& key, bool use64bit,
                        const QString& valName, const QVariant& value) {
    if (value.type() != QMetaType::QString && !value.canConvert<uint>()) {
        return false;
    }
    if (!createKey(root, key, use64bit)) return false;
    HKEY hKey;
    bool success =
        RegOpenKeyExW(
            rootKeyToHkey(root), (LPCWSTR)key.utf16(), NULL,
            KEY_SET_VALUE | (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY),
            &hKey) == ERROR_SUCCESS;
    if (success) {
        bool valueExisted = valueExists(root, key, use64bit, valName);
        if (value.type() == QMetaType::QString) {
            QString str = value.toString();
            success = RegSetValueExW(hKey, (LPCWSTR)valName.utf16(), 0, REG_SZ,
                                     (const BYTE*)str.utf16(),
                                     (wcslen((const wchar_t*)str.utf16()) + 1) *
                                         sizeof(wchar_t)) == ERROR_SUCCESS;
        } else {
            uint val = value.toUInt();
            success = RegSetValueExW(hKey, (LPCWSTR)valName.utf16(), 0,
                                     REG_DWORD, (const BYTE*)&val,
                                     sizeof(uint)) == ERROR_SUCCESS;
        }
        if (success && !valueExisted) {
            ngApp->receipt()->logRegValueCreate(root, key, use64bit, valName);
        }
        RegCloseKey(hKey);
    }
    // TODO REG_EXPAND_SZ?
    // TODO null?
    return success;
}

bool Registry::keyHasContent(RootKey root, const QString& key, bool use64bit) {
    if (!keyExists(root, key, use64bit)) return false;
    HKEY hKey;
    bool success =
        RegOpenKeyExW(
            rootKeyToHkey(root), (LPCWSTR)key.utf16(), NULL,
            KEY_QUERY_VALUE | (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY),
            &hKey) == ERROR_SUCCESS;
    if (!success) return false;
    DWORD cSubKeys, cValues;
    RegQueryInfoKeyW(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, &cValues,
                     NULL, NULL, NULL, NULL);
    bool result = cSubKeys > 0 || cValues > 0;
    RegCloseKey(hKey);
}

bool Registry::deleteKeyNonRecursive(RootKey root, const QString& key,
                                     bool use64bit) {
    if (!keyExists(root, key, use64bit) || keyHasContent(root, key, use64bit))
        return false;
    return RegDeleteKeyExW(rootKeyToHkey(root), (LPCWSTR)key.utf16(),
                           use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY,
                           0) == ERROR_SUCCESS;
}

bool Registry::deleteValue(RootKey root, const QString& key, bool use64bit,
                           const QString& valName) {
    if (!valueExists(root, key, use64bit, valName)) return false;
    HKEY hKey;
    bool success =
        RegOpenKeyExW(
            rootKeyToHkey(root), (LPCWSTR)key.utf16(), NULL,
            KEY_SET_VALUE | (use64bit ? KEY_WOW64_64KEY : KEY_WOW64_32KEY),
            &hKey) == ERROR_SUCCESS;
    if (success) {
        success =
            RegDeleteValueW(hKey, (LPCWSTR)valName.utf16()) == ERROR_SUCCESS;
        RegCloseKey(hKey);
    }
    return success;
}

HKEY Registry::rootKeyToHkey(RootKey root) {
    switch (root) {
        case RootKey::HKLM:
            return HKEY_LOCAL_MACHINE;
        case RootKey::HKCC:
            return HKEY_CURRENT_CONFIG;
        case RootKey::HKCU:
            return HKEY_CURRENT_USER;
        case RootKey::HKCR:
            return HKEY_CLASSES_ROOT;
        case RootKey::HKU:
            return HKEY_USERS;
    }
}

#endif