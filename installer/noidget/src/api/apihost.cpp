#include <QScriptValue>
#include <QFile>
#include <QTextStream>
#include "apihost.h"
#include "window.h"
#include "viewhost.h"
#include "systeminfo.h"
#include "fshost.h"
#include "txhost.h"
#include "win32.h"

namespace api {

ApiHost::ApiHost(QObject *parent) : QObject(parent) {
    _engine = new QScriptEngine(this);
    QScriptValue root_ = _engine->newObject();

    root_.setProperty("window", _engine->newQObject(new Window(this)));

    auto viewHost = new ViewHost(this);
    auto ng_view = _engine->newQObject(viewHost);
    viewHost->setupScriptObject(ng_view);
    root_.setProperty("view", ng_view);

    auto systemInfo = new SystemInfo(this);
    auto ng_systemInfo = _engine->newQObject(systemInfo);
    systemInfo->setupScriptObject(ng_systemInfo);
    root_.setProperty("systemInfo", ng_systemInfo);

    auto fsHost = new FsHost(this);
    auto ng_fs = _engine->newQObject(fsHost);
    fsHost->setupScriptObject(ng_fs);
    root_.setProperty("fs", ng_fs);

    auto txHost = new TxHost(this);
    auto ng_tx = _engine->newQObject(txHost);
    txHost->setupScriptObject(ng_tx);
    root_.setProperty("tx", ng_tx);

#ifdef Q_OS_WIN32
    auto win32 = new Win32(this);
    auto ng_win32 = _engine->newQObject(win32);
    win32->setupScriptObject(ng_win32);
    root_.setProperty("win32", ng_win32);
#endif

    _engine->globalObject().setProperty("ng", root_);

    QFile jslibFile(":/jslib.js");
    jslibFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&jslibFile);
    _engine->evaluate(ts.readAll(), "jslib.js");
}

QScriptValue ApiHost::root() { return _engine->globalObject().property("ng"); }

ApiHost::~ApiHost() {}

}  // namespace api