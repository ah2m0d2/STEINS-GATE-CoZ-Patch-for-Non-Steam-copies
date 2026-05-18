#include "createshortcutaction.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"

#include <QFileInfo>
#include "fs.h"
#include "installerapplication.h"
#include "util/exception.h"
#include "receiptwriter.h"

void CreateShortcutAction::run() {
    if (_targetPath.isEmpty() || _shortcutPath.isEmpty()) {
        throw NgException(
            "Missing required parameter trying to create shortcut");
    }

    QString _targetExpanded = ngApp->globalFs()->expandedPath(_targetPath);
    QString _shortcutExpanded = ngApp->globalFs()->expandedPath(_shortcutPath);

    emit log(QString("Creating shortcut to %1 at %2")
                 .arg(_targetExpanded, _shortcutExpanded),
             true);

    HRESULT hres;
    IShellLink* psl;

    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres)) {
        IPersistFile* ppf;

        psl->SetPath((LPCWSTR)_targetExpanded.utf16());
        if (!_targetArgs.isEmpty()) {
            psl->SetArguments((LPCWSTR)_targetArgs.utf16());
        }
        if (!_workingDir.isEmpty()) {
            psl->SetWorkingDirectory(
                (LPCWSTR)ngApp->globalFs()->expandedPath(_workingDir).utf16());
        }
        if (!_iconPath.isEmpty()) {
            psl->SetIconLocation(
                (LPCWSTR)ngApp->globalFs()->expandedPath(_iconPath).utf16(), 0);
        }
        if (!_tooltip.isEmpty()) {
            psl->SetDescription((LPCWSTR)_tooltip.utf16());
        }

        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres)) {
            ngApp->globalFs()->createDirectory(
                QFileInfo(_shortcutExpanded).path());
            bool fileIsNew = !ngApp->globalFs()->pathExists(_shortcutExpanded);
            hres = ppf->Save((LPCOLESTR)_shortcutExpanded.utf16(), TRUE);
            if (!SUCCEEDED(hres)) {
                ppf->Release();
                psl->Release();
                throw NgException(QString("Failed to save shortcut at %1")
                                      .arg(_shortcutExpanded));
            } else if (fileIsNew) {
                ngApp->receipt()->logFileCreate(_shortcutExpanded);
            }
            ppf->Release();
        } else {
            psl->Release();
            throw NgException("Failed to create shortcut");
        }
        psl->Release();
    } else {
        throw NgException("Failed to create shortcut");
    }
}

#endif