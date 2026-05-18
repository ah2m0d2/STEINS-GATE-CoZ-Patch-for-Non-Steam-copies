#include "label.h"
#include <QHBoxLayout>

namespace api {
namespace view {

Label::Label(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    _lbl = new QLabel(this);
    _lbl->setWordWrap(true);
    _lbl->setTextFormat(Qt::PlainText);
    layout->addWidget(_lbl);
}
Label::~Label() {}

}  // namespace view
}  // namespace api
