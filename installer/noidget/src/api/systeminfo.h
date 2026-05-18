#pragma once

#include <QObject>
#include <QString>
#include <QScriptable>
#include <util/systeminfo.h>

namespace api {

class ApiHost;

/*^jsdoc
 * @namespace systemInfo
 * @memberof ng
 * @static
 * @toplevel
 ^jsdoc*/
class SystemInfo : public QObject, protected QScriptable {
    Q_OBJECT

   public:
    /*^jsdoc
     * Operating system family
     * @namespace OsFamily
     * @memberof ng.systemInfo
     * @static
     * @toplevel
     * @property {number} Unknown
     * @property {number} Windows
     * @property {number} Mac
     * @property {number} Linux
     ^jsdoc*/

    explicit SystemInfo(ApiHost* parent);
    void setupScriptObject(QScriptValue& o);
    ~SystemInfo();

    Q_INVOKABLE util::SystemInfo::OsFamily platform() const;
    Q_INVOKABLE bool isWine() const;
    Q_INVOKABLE bool isProcessRunning(const QString& processName) const;
    Q_INVOKABLE QString getEnv(const QString& name) const;
};
}  // namespace api