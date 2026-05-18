#pragma once

#include <QWidget>
#include <QScriptable>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QMouseEvent>
#include <QScriptValue>

namespace api {
namespace view {
/*^jsdoc
 * Check box with associated multiline label
 * @class CheckBox
 * @memberof ng.view
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} text - Label text, supports wordwrapping
 * @property {boolean} checked - Currently checked?
 *
 * Setting this from script **will trigger** {@link ng.view.CheckBox#onChange}.
 Setting a default (`preset`) when creating the CheckBox with {@link
 ng.view.Container#addCheckBox} will not.
 ^jsdoc*/
class CheckBox : public QWidget, protected QScriptable {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    /*^jsdoc
     * Render basic HTML in label?
     * @member {boolean} richText
     * @default false
     * @instance
     * @memberof ng.view.CheckBox
     ^jsdoc*/
    Q_PROPERTY(bool richText READ richText WRITE setRichText)
    Q_PROPERTY(bool checked READ checked WRITE setChecked)
    /*^jsdoc
     * Event handler that triggers when `checked` changes.
     *
     * `function(checked: boolean)`
     * @member {Function} onChange
     * @instance
     * @memberof ng.view.CheckBox
     ^jsdoc*/
    Q_PROPERTY(QScriptValue onChange READ onChange WRITE setOnChange)
    /*^jsdoc
     * Input allowed? (When this is `false`, `checked` can still be changed
     programmatically)
     * @member {boolean} enabled
     * @default true
     * @instance
     * @memberof ng.view.CheckBox
     ^jsdoc*/
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)

   public:
    explicit CheckBox(QWidget *parent = 0);
    ~CheckBox();

    QString text() const { return _lbl->text(); }
    void setText(const QString &v) { _lbl->setText(v); }

    bool richText() const { return _lbl->textFormat() == Qt::RichText; }
    void setRichText(bool v) {
        if (v) {
            _lbl->setTextFormat(Qt::RichText);
        } else {
            _lbl->setTextFormat(Qt::PlainText);
        }
    }

    bool checked() const { return _cb->isChecked(); }
    void setChecked(bool v) { _cb->setChecked(v); }

    QScriptValue onChange() const { return _onChange; }
    void setOnChange(const QScriptValue &v) { _onChange = v; }

    bool enabled() const { return _cb->isEnabled(); }
    void setEnabled(bool v) { _cb->setEnabled(v); }

   protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

   private slots:
    void qcb_stateChanged(int state);

   private:
    // We need our own label to support rich text and avoid shortcut processing
    QLabel *_lbl;
    QCheckBox *_cb;
    QScriptValue _onChange;
};
}  // namespace view
}  // namespace api