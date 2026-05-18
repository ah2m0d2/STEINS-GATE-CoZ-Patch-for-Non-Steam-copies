#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

class Receipt;

/*^jsdoc
 * Action to rollback the transaction's receipt
 * 
 * Can only be used in uninstall mode.
 * 
 * @class RollbackReceiptAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class RollbackReceiptAction : public TxAction {
    Q_OBJECT

   public:
    explicit RollbackReceiptAction(QObject* parent = 0);
    void prepare() override;
    void run() override;

   private:
    qint64 calcSubactionCount() override;
    qint64 _subactionProgress = 0;

    Receipt* _receipt;
};