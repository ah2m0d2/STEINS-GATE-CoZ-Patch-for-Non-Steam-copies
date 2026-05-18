#include "txsection.h"
#include "txaction.h"

TxSection::TxSection(QObject* parent = 0) : QObject(parent) {}
TxSection::~TxSection() {}

void TxSection::addAction(TxAction* action) {
    action->setParent(this);
    connect(action, &TxAction::log, this, &TxSection::actionLog,
            Qt::DirectConnection);
    connect(this, &TxSection::cancelled, action, &TxAction::cancel);
    _actions.append(action);
    int i = _actions.count() - 1;
    connect(action, &TxAction::progress, [i, this](qint64 actionProgress) {
        actionProgress = qMin(_actionSizes[i], actionProgress);
        emit progress(_roughProgress + actionProgress);
    });
    connect(
        action, &TxAction::subactionProgress, [i, this](qint64 actionProgress) {
            actionProgress = qMin(_actionSubactionCounts[i], actionProgress);
            emit subactionProgress(_roughSubactionProgress + actionProgress);
        });
}

qint64 TxSection::size() {
    qint64 result = 0;
    for (TxAction* action : _actions) {
        qint64 actionSize = action->size();
        _actionSizes.append(actionSize);
        result += actionSize;
    }
    return result;
}

qint64 TxSection::subactionCount() {
    qint64 result = 0;
    for (TxAction* action : _actions) {
        qint64 subactionCount = action->subactionCount();
        _actionSubactionCounts.append(subactionCount);
        result += subactionCount;
    }
    return result;
}

void TxSection::prepare() {
    for (TxAction* action : _actions) {
        action->prepare();
    }
}

void TxSection::run() {
    for (int i = 0; i < _actions.count(); i++) {
        if (_isCancelled) {
            return;
        }
        TxAction* action = _actions[i];
        action->run();
        _roughProgress += _actionSizes[i];
        _roughSubactionProgress += _actionSubactionCounts[i];
        emit progress(_roughProgress);
        emit subactionProgress(_roughSubactionProgress);
    }
}

void TxSection::actionLog(const QString& text, bool fileOnly) {
    emit log(text, fileOnly);
}