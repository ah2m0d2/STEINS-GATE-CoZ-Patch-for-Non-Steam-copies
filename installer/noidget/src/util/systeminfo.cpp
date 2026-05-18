#include "systeminfo.h"
#include "exception.h"
#include <QCoreApplication>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <psapi.h>
#endif

namespace util {

SystemInfo::OsFamily SystemInfo::platform() {
#if defined(Q_OS_WIN32)
    return OsFamily::Windows;
#elif defined(Q_OS_MACOS)
    return OsFamily::Mac;
#elif defined(Q_OS_LINUX)
    return OsFamily::Linux;
#else
    return OsFamily::Unknown;
#endif
}

bool SystemInfo::isWine() {
#ifndef Q_OS_WIN32
    return false;
#else
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll) return false;
    return GetProcAddress(ntdll, "wine_get_version") != NULL;
#endif
}

bool SystemInfo::isProcessRunning(const QString& processName) {
#ifdef Q_OS_WIN32
    bool result = false;
    DWORD allocation = 4096;
    DWORD used;
    DWORD* pids = nullptr;
    BOOL epmSucceded = TRUE;
    do {
        if (pids != nullptr) {
            free(pids);
            allocation *= 2;
        }
        if (!epmSucceded) {
            return false;
        }
        pids = (DWORD*)malloc(allocation);
        epmSucceded = EnumProcesses(pids, allocation, &used);
    } while (allocation == used);
    int processCount = used / sizeof(DWORD);
    wchar_t szProcessName[MAX_PATH + 1];
    for (int i = 0; i < processCount; i++) {
        if (pids[i] == (DWORD)QCoreApplication::applicationPid()) continue;
        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pids[i]);
        if (hProcess != NULL) {
            GetModuleBaseNameW(hProcess, NULL, szProcessName,
                               sizeof(szProcessName) / sizeof(wchar_t));
            QString qstr = QString::fromWCharArray(szProcessName);
            if (qstr == processName) result = true;
            CloseHandle(hProcess);
        }
        if (result) break;
    }
    free(pids);
    return result;
#else
    throw NgException("Not implemented");
#endif
}

}  // namespace util
