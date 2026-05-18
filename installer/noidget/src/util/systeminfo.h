#pragma once

#include <QObject>
#include <QString>

namespace util {

class SystemInfo {
    Q_GADGET

    SystemInfo() = delete;

   public:
    enum class OsFamily { Unknown, Windows, Mac, Linux };
    Q_ENUM(OsFamily)

    static OsFamily platform();
    static bool isWine();
    static bool isProcessRunning(const QString& processName);
};
}  // namespace util