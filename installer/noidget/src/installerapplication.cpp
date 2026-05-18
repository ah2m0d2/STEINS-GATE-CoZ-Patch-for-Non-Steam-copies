#include "installerapplication.h"
#include "installerwindow.h"
#include <api/apihost.h>
#include "fs.h"
#include <tx/transaction.h>
#include "receiptwriter.h"
#include "win32_registry.h"
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>
#include <QResource>
#include <QMessageBox>
#include <QScriptEngineAgent>

#ifdef SCRIPT_DEBUG
#include <QScriptEngineDebugger>
#include <QAction>
#endif

class ErrorAgent : public QScriptEngineAgent {
   public:
    ErrorAgent(QScriptEngine* engine) : QScriptEngineAgent(engine) {}

    void exceptionThrow(qint64 scriptId, const QScriptValue& exception,
                        bool hasHandler) override {
        if (hasHandler) return;

        QMessageBox mb(ngApp->window());
        mb.setText(
            QString("Script error (please send this to patch developers):\n%1")
                .arg(exception.toString()));
        mb.setDetailedText(engine()->currentContext()->backtrace().join('\n'));
        mb.setWindowTitle("Script error");
        mb.exec();
    }
};

InstallerApplication::InstallerApplication(int& argc, char** argv)
    : QApplication(argc, argv) {
    // Despite Q_ENUM this is apparently required for use in signals
    qRegisterMetaType<InstallerApplication::State>(
        "InstallerApplication::State");

    _currentState = State::Preparation;

    if (!QResource::registerResource("userdata.rcc")) {
        QMessageBox::critical(0, "Error",
                              "Could not load userdata.rcc (are you running "
                              "the installer out of its directory?)");
        exit(1);
        return;
    }

    w = new InstallerWindow(0);

    // we do not set these globally so that we can have unthemed dialogs
    w->setStyle(QStyleFactory::create("windows"));
    QFile qssFile(":/kofuna/style.qss");
    qssFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&qssFile);
    w->setStyleSheet(ts.readAll());

    h = new api::ApiHost(0);

    _fs = new Fs(this);
#ifdef Q_OS_WIN32
    _registry = new Registry(this);
#endif

    _receipt = new ReceiptWriter(this);

    _tx = new Transaction(this);

    QFile scriptFile(":/userdata/script.js");
    scriptFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts2(&scriptFile);
#ifdef SCRIPT_DEBUG
    QScriptEngineDebugger* debugger = new QScriptEngineDebugger(this);
    debugger->attachTo(h->engine());
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
#else
    ErrorAgent* agent = new ErrorAgent(h->engine());
    h->engine()->setAgent(agent);
#endif

    h->engine()->evaluate(ts2.readAll(), "script.js");
}

InstallerApplication::~InstallerApplication() {
    if (w) delete w;
    if (h) delete h;
}

void InstallerApplication::showWindow() { w->show(); }