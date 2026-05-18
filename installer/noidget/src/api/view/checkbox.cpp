#include "checkbox.h"
#include <QHBoxLayout>
#include "installerapplication.h"
#include <QScriptValueList>

namespace api {
namespace view {

CheckBox::CheckBox(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);
    _cb = new QCheckBox(this);
    connect(_cb, &QCheckBox::stateChanged, this, &CheckBox::qcb_stateChanged);
    layout->addWidget(_cb, 0, Qt::AlignBaseline);
    _lbl = new QLabel(this);
    _lbl->setWordWrap(true);
    _lbl->setTextFormat(Qt::PlainText);
    layout->addWidget(_lbl, 1);
}
CheckBox::~CheckBox() {}

void CheckBox::mousePressEvent(QMouseEvent *event) {
    QMouseEvent *ne =
        new QMouseEvent(event->type(), QPointF(0.0, 0.0), event->button(),
                        event->buttons(), event->modifiers());
    ngApp->postEvent(_cb, ne);
}
void CheckBox::mouseReleaseEvent(QMouseEvent *event) {
    QMouseEvent *ne =
        new QMouseEvent(event->type(), QPointF(0.0, 0.0), event->button(),
                        event->buttons(), event->modifiers());
    ngApp->postEvent(_cb, ne);
}

void CheckBox::qcb_stateChanged(int state) {
    bool checked = state == Qt::Checked;
    if (_onChange.isFunction()) {
        QScriptValueList args;
        args << checked;
        _onChange.call(QScriptValue(), args);
    }
}

}  // namespace view
}  // namespace api
