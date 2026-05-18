#pragma once

#include <QObject>
#include <QScriptable>
#include <QVector>
#include <QTemporaryFile>
#include "installerapplication.h"
#include <utility>

class TxSection;

/*^jsdoc
 * Recursive sequence of steps to perform after configuration. **Not actually
 * transactional** (yet).
 *
 * The global {@link ng.fs.Fs} instance is used for all path handling.
 *
 * At the top level, transactions are divided into {@link ng.tx.TxSection}s
 * which are executed in order. The name of the current section is displayed on
 * the progress page.
 * 
 * Also contains text templates for the finish page. Rich text is not supported.
 * Use `%LOGPATH%` as a placeholder for the text log file path.
 *
 * @class Transaction
 * @memberof ng.tx
 * @toplevel
 * @hideconstructor
 * @property {string} finishText
 * @property {string} cancelText
 * @property {string} errorText
 * @property {string} receiptPath - **Folder** the receipt resides in
 ^jsdoc*/
class Transaction : public QObject, protected QScriptable {
    Q_OBJECT
    Q_PROPERTY(QString finishText READ finishText WRITE setFinishText)
    Q_PROPERTY(QString cancelText READ cancelText WRITE setCancelText)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText)
    Q_PROPERTY(QString receiptPath READ receiptPath WRITE setReceiptPath)
    /*^jsdoc
     * Use uninstall mode?
     * 
     * In uninstall mode, no receipt will be written and progress will be
     * displayed in sub-actions (e.g. one file deletion) instead of bytes.
     * It is otherwise equivalent to install mode.
     * @member {boolean} uninstallMode
     * @default false
     * @instance
     * @memberof ng.tx.Transaction
     ^jsdoc*/
    Q_PROPERTY(bool uninstallMode READ uninstallMode WRITE setUninstallMode)

   public:
    explicit Transaction(QObject* parent);
    ~Transaction();

    Q_INVOKABLE TxSection* addSection(const QString& title);
    Q_INVOKABLE void addExecuteAfterFinish(const QString& cmd);

    std::pair<qint64, qint64> prepare();  // returns (subaction count, size)
    void run();
    void runPost();

    bool isStarted() { return _isStarted; }
    bool isCancelled() { return _isCancelled; }

    int sectionCount() { return _sections.count(); }

    QString logFileName();

    QString finishText() { return _finishText; }
    void setFinishText(const QString& finishText) { _finishText = finishText; }
    QString errorText() { return _errorText; }
    void setErrorText(const QString& errorText) { _errorText = errorText; }
    QString cancelText() { return _cancelText; }
    void setCancelText(const QString& cancelText) { _cancelText = cancelText; }
    QString receiptPath() { return _receiptPath; }
    void setReceiptPath(const QString& receiptPath) {
        _receiptPath = receiptPath;
    }
    bool uninstallMode() { return _uninstallMode; }
    void setUninstallMode(bool uninstallMode) {
        _uninstallMode = uninstallMode;
    }

   signals:
    void sectionChanged(int i, const QString& title);
    void log(const QString& text);
    void progress(qint64 progress);
    void subactionProgress(qint64 progress);
    void cancelled();

   public slots:
    void handleAppStateChange(InstallerApplication::State newState);

   private slots:
    void sectionLog(const QString& text, bool fileOnly = false);

   private:
    QVector<TxSection*> _sections;
    QVector<qint64> _sectionSizes;
    QVector<qint64> _sectionSubactionCounts;
    QVector<QString> _postFinishCmds;
    QTemporaryFile* _logFile = nullptr;
    qint64 _roughProgress = 0;
    qint64 _roughSubactionProgress = 0;
    bool _isPrepared = false;
    bool _isStarted = false;
    bool _isCancelled = false;
    bool _uninstallMode = false;

    QString _finishText, _cancelText, _errorText;
    QString _receiptPath;

    void logToFile(const QString& text);
};