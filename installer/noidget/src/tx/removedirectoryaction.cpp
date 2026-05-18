#include "removedirectoryaction.h"
#include "installerapplication.h"
#include "fs.h"
#include <QDir>

void RemoveDirectoryAction::run() {
    QString expandedPath = ngApp->globalFs()->expandedPath(_path);
    if (!ngApp->globalFs()->pathIsDirectory(expandedPath)) {
        emit log(QString("Not deleting %1: no such file or directory")
                     .arg(expandedPath));
    } else if (!QDir(expandedPath).isEmpty() && _onlyIfEmpty) {
        emit log(
            QString("Not deleting %1: Directory not empty").arg(expandedPath));
    } else {
        QDir(expandedPath).removeRecursively();
        emit log(QString("Deleted %1").arg(expandedPath));
    }
}