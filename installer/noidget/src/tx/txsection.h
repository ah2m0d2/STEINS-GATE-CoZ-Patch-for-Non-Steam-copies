#pragma once

#include <QObject>
#include <QVector>
#include <QScriptable>

class TxAction;

/*^jsdoc
 * Subsequence of steps in a transaction (purely for organisational purposes)
 *
 * @class TxSection
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} title - Section heading displayed in log and on progress page
 ^jsdoc*/
class TxSection : public QObject, protected QScriptable {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)

   public:
    explicit TxSection(QObject* parent);
    ~TxSection();

    QString title() { return _title; }
    void setTitle(const QString& title) { _title = title; }

    void addAction(TxAction* action);
    qint64 size();
    qint64 subactionCount();
    void prepare();
    void run();

   signals:
    void log(const QString& text, bool fileOnly = false);
    void progress(qint64 progress);
    void subactionProgress(qint64 progress);
    void cancelled();

   public slots:
    void cancel() {
        _isCancelled = true;
        emit cancelled();
    }

   private slots:
    void actionLog(const QString& text, bool fileOnly = false);

   private:
    QVector<TxAction*> _actions;
    QVector<qint64> _actionSizes;
    QVector<qint64> _actionSubactionCounts;
    QString _title;
    qint64 _roughProgress = 0;
    qint64 _roughSubactionProgress = 0;
    bool _isCancelled = false;
};