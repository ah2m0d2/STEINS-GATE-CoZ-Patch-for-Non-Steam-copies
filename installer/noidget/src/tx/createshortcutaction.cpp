#include "createshortcutaction.h"
#include "util/exception.h"

#if !defined(Q_OS_WIN32) && !defined(Q_OS_LINUX)

void CreateShortcutAction::run() {
    throw NgException("Tried to create shortcut on unsupported platform");
}

#endif