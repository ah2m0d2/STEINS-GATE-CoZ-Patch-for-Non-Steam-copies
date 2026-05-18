#include "progresspage.h"
#include <kofuna/invertprogressbar.h>
#include "installerapplication.h"
#include <tx/transaction.h>
#include <QFuture>
#include <QtConcurrent>
#include <QScrollBar>

ProgressPage::ProgressPage(QWidget* parent) : view::Page(parent) {
    _layout = new QVBoxLayout(this);
    _layout->setMargin(0);
    _layout->setSpacing(8);
    _layout->setAlignment(Qt::AlignTop);
    setLayout(_layout);

    _titleLbl = new QLabel(this);
    _titleLbl->setTextFormat(Qt::PlainText);
    _titleLbl->setWordWrap(false);
    _titleLbl->setText("Preparing");
    _titleLbl->setObjectName("pageHeading");
    _layout->addWidget(_titleLbl);

    _progressBar = new InvertProgressBar(this);
    _layout->addWidget(_progressBar);

    _logBox = new QTextEdit(this);
    _logBox->setReadOnly(true);
    _logBox->setAcceptRichText(false);
    _logBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _logBox->setObjectName("logBox");
    _layout->addWidget(_logBox);

    ngApp->setCurrentState(InstallerApplication::State::Installing);

    Transaction* tx = ngApp->tx();
    connect(tx, &Transaction::sectionChanged, this,
            &ProgressPage::txSectionChange);
    connect(tx, &Transaction::log, this, &ProgressPage::getLog);
    if (tx->uninstallMode()) {
        connect(tx, &Transaction::subactionProgress, this,
                &ProgressPage::updateProgressBar);
    } else {
        connect(tx, &Transaction::progress, this,
                &ProgressPage::updateProgressBar);
    }

    QFuture<std::pair<qint64, qint64>> txSize =
        QtConcurrent::run(tx, &Transaction::prepare);
    QFutureWatcher<void>* txSizeWatcher = new QFutureWatcher<void>(this);
    connect(txSizeWatcher, &QFutureWatcher<void>::finished, [=]() {
        qint64 rawSize;
        if (tx->uninstallMode()) {
            rawSize = txSize.result().first;
        } else {
            rawSize = txSize.result().second;
        }
        if (rawSize > INT_MAX) {
            _needProgressDownscale = true;
            _progressDownscaleFactor = (double)INT_MAX / (double)rawSize;
            _progressBar->setMaximum(INT_MAX);
        } else {
            _progressBar->setMaximum((int)rawSize);
        }

        startTx();
    });
    txSizeWatcher->setFuture(txSize);
}

void ProgressPage::startTx() {
    _sectionCount = ngApp->tx()->sectionCount();

    QFuture<void> f = QtConcurrent::run(ngApp->tx(), &Transaction::run);
}

void ProgressPage::txSectionChange(int i, const QString& sectionTitle) {
    _titleLbl->setText(
        QString("(%1/%2) %3").arg(i + 1).arg(_sectionCount).arg(sectionTitle));
}

void ProgressPage::getLog(const QString& text) {
    QScrollBar* textScroll = _logBox->verticalScrollBar();
    int oldScroll = textScroll->value();
    bool autoscrollEnabled = textScroll->value() == textScroll->maximum();
    QTextCursor oldCursor = _logBox->textCursor();
    QTextCursor newCursor(oldCursor);
    newCursor.movePosition(QTextCursor::End);
    _logBox->setTextCursor(newCursor);
    // insertPlainText inserts at cursor, so we need to go to the end (in case user has selected something)
    _logBox->insertPlainText("\n" + text);
    // give selection back
    _logBox->setTextCursor(oldCursor);
    if (autoscrollEnabled) {
        textScroll->setValue(textScroll->maximum());
    } else {
        textScroll->setValue(oldScroll);
    }
}

void ProgressPage::updateProgressBar(qint64 progress) {
    if (_needProgressDownscale) {
        _progressBar->setValue(
            (int)((double)progress * _progressDownscaleFactor));
    } else {
        _progressBar->setValue((int)progress);
    }
}
