#include "dialog.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include "column.h"
#include <installerapplication.h>
#include <api/exception.h>
#include "scriptabledialog_p.h"

namespace api {
namespace view {

Dialog::Dialog(Dialog::DlgType type, QWidget* parent)
    : QObject(parent), _type(type) {
    _dlg = new ScriptableDialog(parent);
    _dlg->_btnBox = new QDialogButtonBox(Qt::Horizontal, _dlg);
    _dlg->_col = new Column(_dlg);
    QVBoxLayout* layout = new QVBoxLayout(_dlg);
    _dlg->setLayout(layout);
    layout->setMargin(16);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(_dlg->_col, 1);
    layout->addWidget(_dlg->_btnBox, 0, Qt::AlignBottom);
    switch (_type) {
        case DlgType::OK:
            _dlg->_btnBox->setCenterButtons(true);
            _dlg->_btnBox->addButton(QDialogButtonBox::Ok);
            connect(_dlg->_btnBox, &QDialogButtonBox::accepted, _dlg,
                    &QDialog::accept);
            break;
        case DlgType::YesNo:
            _dlg->_btnBox->setCenterButtons(true);
            _dlg->_btnBox->addButton(QDialogButtonBox::Yes);
            _dlg->_btnBox->addButton(QDialogButtonBox::No);
            connect(_dlg->_btnBox, &QDialogButtonBox::accepted, _dlg,
                    &QDialog::accept);
            connect(_dlg->_btnBox, &QDialogButtonBox::rejected, _dlg,
                    &QDialog::reject);
            break;
    }
}

Dialog::~Dialog() {}

QString Dialog::title() const { return _dlg->windowTitle(); }
void Dialog::setTitle(const QString& v) { _dlg->setWindowTitle(v); }

int Dialog::width() const { return _dlg->width(); }
void Dialog::setWidth(int v) { _dlg->setFixedWidth(v); }
int Dialog::height() const { return _dlg->height(); }
void Dialog::setHeight(int v) { _dlg->setFixedHeight(v); }

void Dialog::addSpace(int space) { _dlg->_col->addSpace(space); }

Label* Dialog::addLabel(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addLabel(obj);
    SCRIPT_EX_GUARD_END(nullptr)
}
Button* Dialog::addButton(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addButton(obj);
    SCRIPT_EX_GUARD_END(nullptr)
}
TextField* Dialog::addTextField(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addTextField(obj);
    SCRIPT_EX_GUARD_END(nullptr)
}
CheckBox* Dialog::addCheckBox(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addCheckBox(obj);
    SCRIPT_EX_GUARD_END(nullptr)
}
RadioGroup* Dialog::addRadioGroup(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addRadioGroup(obj);
    SCRIPT_EX_GUARD_END(nullptr)
}
DirectoryPicker* Dialog::addDirectoryPicker(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addDirectoryPicker(obj);
    SCRIPT_EX_GUARD_END(nullptr)
}

Column* Dialog::addColumn() {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addColumn();
    SCRIPT_EX_GUARD_END(nullptr)
}
Row* Dialog::addRow() {
    SCRIPT_EX_GUARD_START
    return _dlg->_col->addRow();
    SCRIPT_EX_GUARD_END(nullptr)
}

bool Dialog::present() {
    int ret = _dlg->exec();
    switch (_type) {
        case DlgType::OK:
            // can't reject a message box
            return true;
        case DlgType::YesNo:
            return ret == QDialog::Accepted;
    }
    return false;
}

}  // namespace view
}  // namespace api
