#pragma once

#include <QScriptable>
#include <QVBoxLayout>
#include "container.h"

namespace api {
namespace view {

/*^jsdoc
 * {@link ng.view.Container} that vertically arranges child widgets
 * @class Column
 * @extends ng.view.Container
 * @memberof ng.view
 * @static
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class Column : public Container {
    Q_OBJECT

   public:
    explicit Column(QWidget *parent = 0) : Container(parent) {
        _layout = new QVBoxLayout(this);
        _layout->setMargin(0);
        _layout->setSpacing(8);  // TODO parameterize
        _layout->setAlignment(Qt::AlignTop);
        setLayout(_layout);
    }
    ~Column() {}
};
}  // namespace view
}  // namespace api