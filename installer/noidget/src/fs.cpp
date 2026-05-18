#include "fs.h"
#include "installerapplication.h"
#include "receiptwriter.h"
#include <util/systeminfo.h>
#include <api/exception.h>
#include <QFileInfo>
#include <QRegularExpression>
#include <QProcessEnvironment>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>
#include <QTextStream>

// http://doc.qt.io/qt-5/qfileinfo.html#ntfs-permissions
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

Fs::Fs(QObject* parent) : QObject(parent), macros() {}
Fs::~Fs() {}

/*^jsdoc
 * Adds or changes macro `key` to mean `value`. Throws if `key` contains illegal
 characters.
 *
 * Valid characters are word characters and parentheses.
 * @method setMacro
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} key
 * @param {string} value
 ^jsdoc*/
void Fs::setMacro(const QString& key, const QString& value) {
    const static QRegularExpression validMacro(
        "^[\\w\\(\\)]+$", QRegularExpression::OptimizeOnFirstUsageOption);
    if (!validMacro.match(key).hasMatch()) {
        SCRIPT_THROW("Macro name contains invalid characters")
        return;
    }
    macros[key] = value;
}
/*^jsdoc
 * Removes macro `key`. Environment variables cannot be removed.
 * @method removeMacro
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} key
 ^jsdoc*/
void Fs::removeMacro(const QString& key) { macros.remove(key); }
/*^jsdoc
 * Removes all macros
 * @method clearMacros
 * @memberof ng.fs.Fs
 * @instance
 ^jsdoc*/
void Fs::clearMacros() { macros.clear(); }

/*^jsdoc
 * Expands macros in `inPath`
 * @method expandedPath
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} inPath
 * @returns {string}
 ^jsdoc*/
QString Fs::expandedPath(const QString& inPath) const {
    const static QRegularExpression matchMacro(
        "%([\\w\\(\\)]+)%", QRegularExpression::OptimizeOnFirstUsageOption);
    QString result = inPath;
    bool found;
    do {
        found = false;
        auto match = matchMacro.match(result);
        if (match.hasMatch()) {
            auto key = match.captured(1);
            QString replacement;
            if (macros.contains(key)) {
                found = true;
                replacement = macros[key];
            } else if (QProcessEnvironment::systemEnvironment().contains(key)) {
                found = true;
                replacement =
                    QProcessEnvironment::systemEnvironment().value(key);
            }

            if (found) {
                result.replace(match.capturedStart(1) - 1,
                               match.capturedLength(1) + 2, replacement);
            }
        }
    } while (found);
    return result;
}
/*^jsdoc
 * Expands macros in `inPath` and turns it absolute (according to the process
 working directory) if it isn't already
 * @method absPath
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} inPath
 * @returns {string}
 ^jsdoc*/
QString Fs::absPath(const QString& inPath) const {
    return QFileInfo(expandedPath(inPath)).absoluteFilePath();
}
/*^jsdoc
 * Does `path` point to an existing file or directory?
 * @method pathExists
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} path
 * @returns {boolean}
 ^jsdoc*/
bool Fs::pathExists(const QString& path) const {
    return QFileInfo::exists(expandedPath(path));
}
/*^jsdoc
 * Does `path` point to an existing directory?
 * @method pathIsDirectory
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} path
 * @returns {boolean}
 ^jsdoc*/
bool Fs::pathIsDirectory(const QString& path) const {
    return QFileInfo(expandedPath(path)).isDir();
}
/*^jsdoc
 * Does `path` point to an existing file?
 * @method pathIsFile
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} path
 * @returns {boolean}
 ^jsdoc*/
bool Fs::pathIsFile(const QString& path) const {
    return QFileInfo(expandedPath(path)).isFile();
}
/*^jsdoc
 * Does `path` point to an existing file or directory that we can read?
 * @method pathIsReadable
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} path
 * @returns {boolean}
 ^jsdoc*/
bool Fs::pathIsReadable(const QString& path) const {
    qt_ntfs_permission_lookup++;
    bool result = QFileInfo(expandedPath(path)).isReadable();
    qt_ntfs_permission_lookup--;
    return result;
}
/*^jsdoc
 * Does `path` point to an existing file or directory that we can write?
 * @method pathIsWritable
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} path
 * @returns {boolean}
 ^jsdoc*/
bool Fs::pathIsWritable(const QString& path) const {
    qt_ntfs_permission_lookup++;
    bool result = QFileInfo(expandedPath(path)).isWritable();
    qt_ntfs_permission_lookup--;
    return result;
}

/*^jsdoc
 * Reads UTF-8 text file at `filePath` into string. Throws if file does not exist /
 * is not readable.
 * @method readTextFile
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} filePath
 * @returns {string}
 ^jsdoc*/
QString Fs::readTextFile(const QString& filePath) const {
    QFile f(expandedPath(filePath));
    if (!f.open(QIODevice::ReadOnly)) {
        SCRIPT_THROW("Can't open file")
        return "";
    }
    QTextStream in(&f);
    in.setCodec("UTF-8");
    return in.readAll();
}

/*^jsdoc
 * Computes the MD5 hash of data in `filePath`. Throws if file does not exist /
 is not readable.
 * @method md5sum
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} filePath
 * @returns {string} Hash as lowercase hexadecimal string
 ^jsdoc*/
QString Fs::md5sum(const QString& filePath) const {
    QFile f(expandedPath(filePath));
    if (!f.open(QIODevice::ReadOnly)) {
        SCRIPT_THROW("Can't open file")
        return "";
    }
    QCryptographicHash hash(QCryptographicHash::Md5);
    if (!hash.addData(&f)) {
        SCRIPT_THROW("Can't read data")
        return "";
    }

    return QString(hash.result().toHex());  // toHex() is lowercase
}

/*^jsdoc
 * Tries to rename file/directory `from` to `to`. Throws if rename operation
 fails for some reason.
 *
 * Renames **are not logged** to the uninstall receipt.
 * @method rename
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} from
 * @param {string} to
 ^jsdoc*/
void Fs::rename(const QString& from, const QString& to) const {
    bool ok = QFile::rename(expandedPath(from), expandedPath(to));
    if (!ok) SCRIPT_THROW("Renaming failed");
}

void Fs::tryCreateSubPath(const QString& workPath,
                          const QString& fullPath) const {
    if (pathIsDirectory(workPath)) return;
    if (pathIsFile(workPath))
        throw NgException(
            QString("Path %1 contains existing file").arg(fullPath));
    if (!QDir(workPath).mkdir("."))
        throw NgException(
            QString("Couldn't create directory %1").arg(workPath));
    ngApp->receipt()->logFileCreate(workPath);
}

/*^jsdoc
 * Tries to create directory `path`. Throws if create operation
 fails for some reason - does *not* throw if directory already exists.
 * @method createDirectory
 * @memberof ng.fs.Fs
 * @instance
 * @param {string} path
 ^jsdoc*/
void Fs::createDirectory(const QString& path) const {
    // we can't use mkpath here because we need to log each directory creation

    QString fullPath =
        QDir::toNativeSeparators(QDir::cleanPath(expandedPath(path)));

    if (pathIsDirectory(fullPath)) return;
    if (pathIsFile(fullPath)) {
        SCRIPT_THROW(QString("Path %1 is an existing file").arg(fullPath));
        return;
    }

    int pos = 0;
    if (util::SystemInfo::platform() == util::SystemInfo::OsFamily::Windows) {
        if (fullPath.startsWith("\\\\")) {
            // UNC path
            pos = fullPath.indexOf(QDir::separator(), 2);
        } else if (fullPath.length() > 1 && fullPath[1] == ':') {
            // drive
            pos = 2;
        }
    }
    // try to create upper directories
    SCRIPT_EX_GUARD_START
    while ((pos = fullPath.indexOf(QDir::separator(), pos + 1)) > -1) {
        QString workPath = fullPath.left(pos);
        tryCreateSubPath(workPath, fullPath);
    }
    tryCreateSubPath(fullPath, fullPath);
    SCRIPT_EX_GUARD_END()
}