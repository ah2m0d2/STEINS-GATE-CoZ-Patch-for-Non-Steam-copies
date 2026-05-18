#include "setregistryvalueaction.h"
#include "installerapplication.h"
#include "util/exception.h"

#ifdef Q_OS_WIN32

void SetRegistryValueAction::run() {
    emit log(QString("Registry: %1\\%2 val %3 := %4 (use64bit: %5)")
                 .arg(EnumToString(_root), _key, _valName, _value.toString(),
                      QString("%1").arg(_use64bit)),
             true);
    if (!ngApp->registry()->setValue(_root, _key, _use64bit, _valName,
                                     _value)) {
        throw NgException(
            QString(
                "Failed to set registry value '%1' in '%2\\%3' (64-bit: %4)")
                .arg(_valName, EnumToString(_root), _key,
                     QString("%1").arg(_use64bit)));
    }
}

#endif