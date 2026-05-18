#pragma once

#include <QScriptContext>
#include <util/exception.h>

// NOTE: context() is only set for calls *directly* from script
// Q_INVOKABLE void native() { foo(); } - context() == nullptr in foo()
// invocation

#define SCRIPT_EX_GUARD_START try {
#define SCRIPT_EX_GUARD_END(defaultReturn)    \
    }                                         \
    catch (const NgException& ex) {           \
        if (context() == nullptr) {           \
            throw;                            \
        } else {                              \
            context()->throwError(ex.what()); \
        }                                     \
        return defaultReturn;                 \
    }

// Functions created with QScriptEngine::newFunction are not inside a
// QScriptable, but instead receive a "QScriptContext *context" parameter
#define SCRIPT_EX_GUARD_START_FUN try {
#define SCRIPT_EX_GUARD_END_FUN(defaultReturn) \
    }                                          \
    catch (const NgException& ex) {            \
        if (context == nullptr) {              \
            throw;                             \
        } else {                               \
            context->throwError(ex.what());    \
        }                                      \
        return defaultReturn;                  \
    }

#define SCRIPT_THROW(what)           \
    if (context() == nullptr) {      \
        throw NgException(what);     \
    } else {                         \
        context()->throwError(what); \
    }

#define SCRIPT_THROW_FUN(what)     \
    if (context == nullptr) {      \
        throw NgException(what);   \
    } else {                       \
        context->throwError(what); \
    }

#define ENFORCE_SCRIPT                                               \
    if (context() == nullptr) {                                      \
        throw NgException(                                           \
            "Tried to call script-only functions from native code"); \
    }