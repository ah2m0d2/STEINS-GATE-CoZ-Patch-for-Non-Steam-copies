#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to create a (desktop/start menu) shortcut (Win32, Linux only)
 * 
 * Parent directories will be created when this is executed.
 * 
 * Global FS macros are allowed in paths.
 * 
 * @class CreateShortcutAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} shortcutPath **(Required)** File path of the shortcut itself
 * @property {string} version Linux only. Display version
 * @property {string} displayName Linux only. Display name - on Windows, that's just the shortcut's filename itself minus .lnk.
 * @property {string} targetPath **(Required)** File path to the shortcut's target
 * @property {string} targetArgs Command-line arguments (if any). Note these are *not* parsed for FS macros.
 * @property {string} workingDir Working directory to execute the target in (or default)
 * @property {string} iconPath (Persistent) path to the icon to use (or default). On Windows, always uses the first icon present in that file.
 * @property {string} tooltip Display description
 * @property {string} xdgCategories Linux only. Semicolon-separated list of [XDG menu entry categories]{@link https://standards.freedesktop.org/menu-spec/latest/apa.html}
 ^jsdoc*/
class CreateShortcutAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString shortcutPath READ shortcutPath WRITE setShortcutPath)
    Q_PROPERTY(QString version READ version WRITE setVersion)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName)
    Q_PROPERTY(QString targetPath READ targetPath WRITE setTargetPath)
    Q_PROPERTY(QString targetArgs READ targetArgs WRITE setTargetArgs)
    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir)
    Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath)
    Q_PROPERTY(QString tooltip READ tooltip WRITE setTooltip)
    Q_PROPERTY(QString xdgCategories READ xdgCategories WRITE setXdgCategories)

   public:
    explicit CreateShortcutAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    QString shortcutPath() { return _shortcutPath; }
    void setShortcutPath(const QString& shortcutPath) {
        _shortcutPath = shortcutPath;
    }
    QString version() { return _version; }
    void setVersion(const QString& version) { _version = version; }
    QString displayName() { return _displayName; }
    void setDisplayName(const QString& displayName) {
        _displayName = displayName;
    }
    QString targetPath() { return _targetPath; }
    void setTargetPath(const QString& targetPath) { _targetPath = targetPath; }
    QString targetArgs() { return _targetArgs; }
    void setTargetArgs(const QString& targetArgs) { _targetArgs = targetArgs; }
    QString workingDir() { return _workingDir; }
    void setWorkingDir(const QString& workingDir) { _workingDir = workingDir; }
    QString iconPath() { return _iconPath; }
    void setIconPath(const QString& iconPath) { _iconPath = iconPath; }
    QString tooltip() { return _tooltip; }
    void setTooltip(const QString& tooltip) { _tooltip = tooltip; }
    QString xdgCategories() { return _xdgCategories; }
    void setXdgCategories(const QString& xdgCategories) {
        _xdgCategories = xdgCategories;
    }

   private:
    QString _shortcutPath, _version, _displayName, _targetPath, _targetArgs,
        _workingDir, _iconPath, _tooltip, _xdgCategories;
};