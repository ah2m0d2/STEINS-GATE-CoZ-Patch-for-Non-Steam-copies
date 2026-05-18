#pragma once

#include <QScriptable>
#include <QScriptValue>
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
 * Modal dialog. Uses {@link ng.view.Column} to contain child widgets.
 * @class Dialog
 * @memberof ng.view
 * @static
 * @toplevel
 * @hideconstructor
 * @extends ng.view.Container
 * @property {string} title - Window title
 * @property {number} width - Window width
 * @property {number} height - Window height
 ^jsdoc*/
class Dialog : public QObject, public IContainer, protected QScriptable {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int height READ height WRITE setHeight)

   public:
    /*^jsdoc
     * Available kinds of {@link ng.view.Dialog}
     * @namespace DlgType
     * @memberof ng.view
     * @static
     * @toplevel
     * @property {number} OK - *message box* style dialog with just an "OK"
     button - cannot be rejected
     * @property {number} YesNo - like OK, but has "Yes" and "No" buttons
     ^jsdoc*/
    enum class DlgType { OK, YesNo };
    Q_ENUM(DlgType)

    explicit Dialog(DlgType type, QWidget* parent = 0);
    ~Dialog();

    QString title() const;
    void setTitle(const QString& v);

    int width() const;
    void setWidth(int v);
    int height() const;
    void setHeight(int v);

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

    bool present();

   private:
    DlgType _type;
    class ScriptableDialog;
    ScriptableDialog* _dlg;
};

}  // namespace view
}  // namespace api
