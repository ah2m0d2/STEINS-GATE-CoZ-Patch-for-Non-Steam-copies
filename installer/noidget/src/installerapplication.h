#pragma once

#include <QApplication>
#include <QCoreApplication>
#include <QStyle>
#include <QMetaEnum>
#include <QString>

class InstallerWindow;
class Fs;
class Transaction;
#ifdef Q_OS_WIN32
class Registry;
#endif
class ReceiptWriter;
namespace api {
class ApiHost;
}

#define ngApp static_cast<::InstallerApplication*>(QCoreApplication::instance())

template <typename Enum>
QString EnumToString(Enum e) {
    return QString(QMetaEnum::fromType<Enum>().valueToKey((int)e));
}

class InstallerApplication : public QApplication {
    Q_OBJECT

   public:
    enum class State { Preparation, Installing, Cancelled, Finished, Error };
    Q_ENUM(State)
    Q_PROPERTY(State currentState READ currentState WRITE setCurrentState NOTIFY
                   currentStateChanged)

    explicit InstallerApplication(int& argc, char** argv);
    ~InstallerApplication();

    void showWindow();

    State currentState() const { return _currentState; }
    void setCurrentState(State currentState) {
        _currentState = currentState;
        emit currentStateChanged(currentState);
    }

    InstallerWindow* window() { return w; }
    api::ApiHost* apiHost() { return h; }
    Fs* globalFs() { return _fs; }
    Transaction* tx() { return _tx; }
#ifdef Q_OS_WIN32
    Registry* registry() { return _registry; }
#endif
    ReceiptWriter* receipt() { return _receipt; }

   signals:
    void currentStateChanged(InstallerApplication::State currentState);

   private:
    InstallerWindow* w;
    api::ApiHost* h;
    Fs* _fs;
    Transaction* _tx;
#ifdef Q_OS_WIN32
    Registry* _registry;
#endif
    ReceiptWriter* _receipt = nullptr;

    State _currentState;
};