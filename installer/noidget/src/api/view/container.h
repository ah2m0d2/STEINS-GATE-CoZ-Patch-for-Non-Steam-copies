#pragma once

#include <QWidget>
#include <QScriptable>
#include <QBoxLayout>
#include "icontainer.h"

namespace api {
namespace view {

class Label;
class Button;
class TextField;
class CheckBox;
class RadioGroup;
class DirectoryPicker;

class Column;
class Row;

/*^jsdoc
 * Abstract base class for widgets containing child widgets
 * @class Container
 * @memberof ng.view
 * @static
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class Container : public QWidget, public IContainer, protected QScriptable {
    Q_OBJECT

   public:
    explicit Container(QWidget* parent = 0);
    virtual ~Container() = 0;

    Q_INVOKABLE void addSpace(int space) override;

    Q_INVOKABLE api::view::Label* addLabel(const QScriptValue& obj) override;
    Q_INVOKABLE api::view::Button* addButton(const QScriptValue& obj) override;
    Q_INVOKABLE api::view::TextField* addTextField(
        const QScriptValue& obj) override;
    Q_INVOKABLE api::view::CheckBox* addCheckBox(
        const QScriptValue& obj) override;
    Q_INVOKABLE api::view::RadioGroup* addRadioGroup(
        const QScriptValue& obj) override;
    Q_INVOKABLE api::view::DirectoryPicker* addDirectoryPicker(
        const QScriptValue& obj) override;

    Q_INVOKABLE api::view::Column* addColumn() override;
    Q_INVOKABLE api::view::Row* addRow() override;

   protected:
    QBoxLayout* _layout;
};

}  // namespace view
}  // namespace api