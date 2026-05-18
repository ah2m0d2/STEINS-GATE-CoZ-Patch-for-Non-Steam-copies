#pragma once

#include <QObject>
#include <QScriptable>
#include <QScriptValue>

class Fs;

namespace api {

class ApiHost;

/*^jsdoc
 * @namespace fs
 * @memberof ng
 * @static
 * @toplevel
 ^jsdoc*/
class FsHost : public QObject, protected QScriptable {
    Q_OBJECT

   public:
    explicit FsHost(ApiHost* parent);
    void setupScriptObject(QScriptValue& o);
    ~FsHost();

    Q_INVOKABLE QScriptValue createFs();
    Q_INVOKABLE Fs* global();
};
}  // namespace api