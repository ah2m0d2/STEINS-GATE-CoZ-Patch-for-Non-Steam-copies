#include "createdirectoryaction.h"
#include "installerapplication.h"
#include "fs.h"

void CreateDirectoryAction::run() {
    emit log(QString("Creating directory %1")
                 .arg(ngApp->globalFs()->expandedPath(_path)));
    ngApp->globalFs()->createDirectory(_path);
}