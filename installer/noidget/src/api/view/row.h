#pragma once

#include <QScriptable>
#include <QHBoxLayout>
#include "container.h"

namespace api {
namespace view {

/*^jsdoc
 * {@link ng.view.Container} that horizontally arranges child widgets
 * @class Row
 * @extends ng.view.Container
 * @static
 * @memberof ng.view
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class Row : public Container {
    Q_OBJECT

   public:
    explicit Row(QWidget *parent = 0) : Container(parent) {
        _layout = new QHBoxLayout(this);
        _layout->setMargin(0);
        _layout->setSpacing(8);  // TODO parameterize
        _layout->setAlignment(Qt::AlignLeft);
        setLayout(_layout);
    }
    ~Row() {}
};
}  // namespace view
}  // namespace api