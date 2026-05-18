#include "binarysearchreplaceaction.h"
#include "installerapplication.h"
#include "fs.h"
#include <util/exception.h>
#include <QFile>
#include <QByteArray>

const qint64 blockSize = 1024 * 1024;

void BinarySearchReplaceAction::run() {
    QString expandedPath = ngApp->globalFs()->expandedPath(_path);
    emit log(QString("Patching %1").arg(expandedPath));
    QFile in(expandedPath);
    if (!in.open(QIODevice::ReadOnly)) {
        throw NgException(QString("Could not read file: %1").arg(expandedPath));
    }
    emit log(QString("Input size: %1").arg(in.size()), true);
    QByteArray data =
        in.readAll().replace(QByteArray::fromHex(_needle.toLatin1()),
                             QByteArray::fromHex(_replace.toLatin1()));

    const char *rawData = data.constData();
    qint64 bytesToWrite = data.size();  // TODO null terminated?
    qint64 totalWritten = 0;

    in.close();
    QFile out(expandedPath);
    if (!out.open(QIODevice::WriteOnly)) {
        throw NgException(
            QString("Could not write file: %1").arg(expandedPath));
    }
    while (bytesToWrite > 0) {
        if (_isCancelled) {
            break;
        }
        qint64 curBytes = qMin(blockSize, bytesToWrite);
        out.write(rawData, curBytes);
        rawData += curBytes;
        totalWritten += curBytes;
        bytesToWrite -= curBytes;
        emit progress(totalWritten);
    }
    if (_isCancelled) {
        emit log(QString("Patch write to %1 aborted mid-write after user "
                         "cancelled transaction")
                     .arg(expandedPath));
    }
}