#pragma once

#include <QObject>
#include <QString>
#include <QHash>
#include <QScriptable>

/*^jsdoc
 * Filesystem interface.
 *
 * **Environment variables** and **custom macros** can be used in paths. E.g.
 *
 * ```javascript
 * fs.expandedPath("%LOCALAPPDATA%/foo"); // => C:/Users/<user>/AppData/Local/foo
 * ```
 *
 * Unless otherwise specified, all functions accept macro'd paths.
 *
 * Macros are case-sensitive and made up of word characters and parentheses.
 Environment variables can be overridden (within the `fs` context - this does
 not change the native environment). Macros may expand to further macros, e.g.
 *
 * ```javascript
 * fs.setMacro("INSTALL_ROOT", "%PROGRAMFILES(X86)%/Noid");
 * ```
 *
 * @class Fs
 * @memberof ng.fs
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class Fs : public QObject, protected QScriptable {
    Q_OBJECT

   public:
    explicit Fs(QObject* parent = 0);
    ~Fs();

    Q_INVOKABLE void setMacro(const QString& key, const QString& value);
    Q_INVOKABLE void removeMacro(const QString& key);
    Q_INVOKABLE void clearMacros();

    Q_INVOKABLE QString expandedPath(const QString& inPath) const;
    Q_INVOKABLE QString absPath(const QString& inPath) const;
    Q_INVOKABLE bool pathExists(const QString& path) const;
    Q_INVOKABLE bool pathIsDirectory(const QString& path) const;
    Q_INVOKABLE bool pathIsFile(const QString& path) const;
    Q_INVOKABLE bool pathIsReadable(const QString& path) const;
    Q_INVOKABLE bool pathIsWritable(const QString& path) const;

    Q_INVOKABLE QString readTextFile(const QString& filePath) const;

    Q_INVOKABLE QString md5sum(const QString& filePath) const;

    Q_INVOKABLE void rename(const QString& from, const QString& to) const;
    Q_INVOKABLE void createDirectory(const QString& path) const;

   private:
    QHash<QString, QString> macros;

    void tryCreateSubPath(const QString& workPath,
                          const QString& fullPath) const;
};