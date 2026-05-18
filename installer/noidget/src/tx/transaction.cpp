#include "transaction.h"
#include "txsection.h"
#include "fs.h"
#include "receiptwriter.h"
#include <util/exception.h>
#include <QProcess>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

Transaction::Transaction(QObject* parent = 0) : QObject(parent) {
    connect(ngApp, &InstallerApplication::currentStateChanged, this,
            &Transaction::handleAppStateChange);
}
Transaction::~Transaction() {
    if (_logFile != nullptr) {
        delete _logFile;
        _logFile = nullptr;
    }
}

QString Transaction::logFileName() {
    if (_logFile == nullptr) return QString();
    return _logFile->fileName();
}

void Transaction::handleAppStateChange(InstallerApplication::State newState) {
    if (newState == InstallerApplication::State::Cancelled) {
        _isCancelled = true;
        logToFile("User cancelled transaction.");
        emit cancelled();
    } else if (newState == InstallerApplication::State::Finished) {
        logToFile("Transaction finished.");
    }
}

/*^jsdoc
 * Append a section
 * 
 * @method addSection
 * @param {string} title
 * @memberof ng.tx.Transaction
 * @returns {ng.tx.TxSection}
 * @instance
 ^jsdoc*/
TxSection* Transaction::addSection(const QString& title) {
    TxSection* section = new TxSection(this);
    section->setTitle(title);
    connect(section, &TxSection::log, this, &Transaction::sectionLog,
            Qt::DirectConnection);
    connect(this, &Transaction::cancelled, section, &TxSection::cancel);
    _sections.append(section);
    int i = _sections.count() - 1;
    connect(section, &TxSection::progress, [i, this](qint64 sectionProgress) {
        sectionProgress = qMin(_sectionSizes[i], sectionProgress);
        emit progress(_roughProgress + sectionProgress);
    });
    connect(
        section, &TxSection::subactionProgress,
        [i, this](qint64 sectionProgress) {
            sectionProgress = qMin(_sectionSubactionCounts[i], sectionProgress);
            emit subactionProgress(_roughSubactionProgress + sectionProgress);
        });
    return section;
}

/*^jsdoc
 * Add a command to run when the installer closes
 * 
 * Commands are started in the order they are added, but without waiting for each
 * other to exit.
 * 
 * @method addExecuteAfterFinish
 * @param {string} cmd
 * @memberof ng.tx.Transaction
 * @instance
 ^jsdoc*/
void Transaction::addExecuteAfterFinish(const QString& cmd) {
    _postFinishCmds.append(cmd);
}

// call this before run()
std::pair<qint64, qint64> Transaction::prepare() {
    if (_isPrepared) {
        throw NgException("Tried to prepare transaction twice");
    }

    _logFile = new QTemporaryFile(QDir::tempPath() + "/noidget.XXXXXX.txt");
    _logFile->setAutoRemove(false);
    _logFile->open();
    _logFile->setTextModeEnabled(true);
    sectionLog(QString("Logging to %1").arg(_logFile->fileName()));

    if (!_uninstallMode) {
        ngApp->receipt()->open(_receiptPath);
    }

    try {
        qint64 size = 0;
        qint64 subactionCount = 0;
        for (TxSection* section : _sections) {
            section->prepare();
            qint64 sectionSize = section->size();
            qint64 sectionSubactionCount = section->subactionCount();
            _sectionSizes.append(sectionSize);
            _sectionSubactionCounts.append(sectionSubactionCount);
            size += sectionSize;
            subactionCount += sectionSubactionCount;
        }
        _isPrepared = true;
        return std::make_pair(subactionCount, size);
    } catch (const NgException& ex) {
        logToFile(QString("Error during preparation: %1").arg(ex.qWhat()));
        ngApp->setCurrentState(InstallerApplication::State::Error);
        throw;
    } catch (...) {
        logToFile("Unhandled exception during preparation");
        ngApp->setCurrentState(InstallerApplication::State::Error);
        throw;
    }
}

void Transaction::run() {
    if (!_isPrepared) {
        throw NgException("Tried to run transaction before preparing it");
    }
    if (_isStarted) {
        throw NgException("Tried to start transaction twice");
    }

    try {
        _isStarted = true;
        for (int i = 0; i < _sections.count(); i++) {
            if (_isCancelled) {
                break;
            }
            TxSection* section = _sections[i];
            emit sectionChanged(i, section->title());
            section->run();
            _roughProgress += _sectionSizes[i];
            _roughSubactionProgress += _sectionSubactionCounts[i];
            emit progress(_roughProgress);
            emit subactionProgress(_roughSubactionProgress);
        }
    } catch (const NgException& ex) {
        logToFile(QString("Error during transaction: %1").arg(ex.qWhat()));
        ngApp->setCurrentState(InstallerApplication::State::Error);
        throw;
    } catch (...) {
        logToFile("Unhandled exception during transaction");
        ngApp->setCurrentState(InstallerApplication::State::Error);
        throw;
    }

    if (_isCancelled) {
        ngApp->setCurrentState(InstallerApplication::State::Cancelled);
    } else {
        ngApp->setCurrentState(InstallerApplication::State::Finished);
    }
}

// TODO wait for process to finish, set working directory
void Transaction::runPost() {
    for (const QString& cmd : _postFinishCmds) {
        QString expandedCmd = ngApp->globalFs()->expandedPath(cmd);
        QProcess::startDetached(cmd);
    }
}

void Transaction::sectionLog(const QString& text, bool fileOnly) {
    if (!fileOnly) {
        emit log(text);
    }
    logToFile(text);
}

void Transaction::logToFile(const QString& text) {
    if (_logFile == nullptr || !_logFile->isOpen()) return;
    QTextStream stream(_logFile);
    QString timestamp =
        QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    stream << QString("[%1 UTC] %2").arg(timestamp, text) << endl;
}