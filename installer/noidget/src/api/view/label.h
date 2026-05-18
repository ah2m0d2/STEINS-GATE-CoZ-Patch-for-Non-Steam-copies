#pragma once

#include <QWidget>
#include <QScriptable>
#include <QLabel>

namespace api {
namespace view {
/*^jsdoc
 * Multiline, rich-text capable label
 * @class Label
 * @memberof ng.view
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} text - Label text, supports wordwrapping
 ^jsdoc*/
class Label : public QWidget, protected QScriptable {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    /*^jsdoc
     * Render basic HTML in label?
     * @member {boolean} richText
     * @default false
     * @instance
     * @memberof ng.view.Label
     ^jsdoc*/
    Q_PROPERTY(bool richText READ richText WRITE setRichText)
   public:
    explicit Label(QWidget *parent = 0);
    ~Label();

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

   private:
    QLabel *_lbl;
};
}  // namespace view
}  // namespace api