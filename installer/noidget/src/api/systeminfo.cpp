#include "systeminfo.h"
#include "apihost.h"
#include <api/exception.h>
#include <QScriptValue>
#include <QScriptValueList>
#include <QProcessEnvironment>

namespace api {

SystemInfo::SystemInfo(ApiHost* parent) : QObject(parent) {}
SystemInfo::~SystemInfo() {}

void SystemInfo::setupScriptObject(QScriptValue& o) {
    ApiHost::registerEnum<util::SystemInfo::OsFamily>(o);
}

/*^jsdoc
 * Platform the program is currently running on.
 * @method platform
 * @memberof ng.systemInfo
 * @static
 * @returns {ng.systemInfo.OsFamily}
 ^jsdoc*/
util::SystemInfo::OsFamily SystemInfo::platform() const {
    return util::SystemInfo::platform();
}

/*^jsdoc
 * Is the installer a Windows build running under Wine?
 * @method isWine
 * @memberof ng.systemInfo
 * @static
 * @returns {boolean}
 ^jsdoc*/
bool SystemInfo::isWine() const { return util::SystemInfo::isWine(); }

/*^jsdoc
 * Is a process with the given name running (besides the installer application itself)?
 * 
 * `processName` is **case sensitive**.
 * 
 * @method isProcessRunning
 * @memberof ng.systemInfo
 * @static
 * @param {string} processName
 * @returns {boolean}
 ^jsdoc*/
bool SystemInfo::isProcessRunning(const QString& processName) const {
    return util::SystemInfo::isProcessRunning(processName);
}

/*^jsdoc
 * Get an environment variable
 * 
 * Note we also have such functionality in {@link ng.fs.Fs#expandedPath}
 * 
 * @method getEnv
 * @memberof ng.systemInfo
 * @static
 * @param {string} name
 * @returns {string} Value (empty if not set)
 ^jsdoc*/
QString SystemInfo::getEnv(const QString& name) const {
    return QProcessEnvironment::systemEnvironment().value(name);
}

}  // namespace api
