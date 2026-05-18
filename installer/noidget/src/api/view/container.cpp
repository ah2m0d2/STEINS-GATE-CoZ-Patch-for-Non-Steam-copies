#include "container.h"
#include "label.h"
#include "button.h"
#include "textfield.h"
#include "checkbox.h"
#include "radiogroup.h"
#include "column.h"
#include "row.h"
#include "directorypicker.h"
#include <QScriptValue>
#include <QScriptValueIterator>
#include <api/exception.h>

namespace api {
namespace view {

Container::Container(QWidget* parent) : QWidget(parent) {}
Container::~Container() {}

// TODO do this without QBoxLayout::addSpacing, to support FlowLayouts
/*^jsdoc
 * Appends `space` px of empty space after the last widget
 * @method addSpace
 * @memberof ng.view.Container
 * @instance
 * @param {number} space
 ^jsdoc*/
void Container::addSpace(int space) { _layout->addSpacing(space); }

/*^jsdoc
 * Creates a new {@link ng.view.Label} and appends it to the container
 * @method addLabel
 * @memberof ng.view.Container
 * @instance
 * @param {string} text
 * @returns {ng.view.Label}
 ^jsdoc*/
/*^jsdoc
 * Creates a new {@link ng.view.Label} and appends it to the container
 * @method addLabel
 * @memberof ng.view.Container
 * @instance
 * @param {Object} params
 * @param {string} params.text
 * @param {boolean} [params.richText=false]
 * @returns {ng.view.Label}
 ^jsdoc*/
Label* Container::addLabel(const QScriptValue& obj) {
    QString text;
    bool richText = false;

    if (obj.isString()) {
        text = obj.toString();
    } else if (obj.isObject()) {
        auto text_ = obj.property("text");
        if (text_.isString()) text = text_.toString();
        auto richText_ = obj.property("richText");
        if (richText_.isBool()) richText = richText_.toBool();
    } else {
        SCRIPT_THROW("Wrong type")
        return nullptr;
    }

    Label* lbl = new Label(this);
    lbl->setText(text);
    lbl->setRichText(richText);
    _layout->addWidget(lbl);
    return lbl;
}
/*^jsdoc
 * Creates a new {@link ng.view.Button} and appends it to the container
 * @method addButton
 * @memberof ng.view.Container
 * @instance
 * @param {Object} params
 * @param {string} params.text
 * @param {boolean} [params.enabled=true]
 * @param {Function} [params.onClick]
 * @param {number} [params.width]
 * @param {number} [params.height]
 * @returns {ng.view.Button}
 ^jsdoc*/
Button* Container::addButton(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    if (!obj.isObject()) {
        // TODO what about no parameters?
        SCRIPT_THROW("Wrong type")
        return nullptr;
    }

    Button* btn = new Button(this);
    auto text = obj.property("text");
    if (text.isString()) btn->setText(text.toString());
    auto enabled = obj.property("enabled");
    if (enabled.isBool()) btn->setEnabled(enabled.toBool());
    btn->setOnClick(obj.property("onClick"));
    auto width = obj.property("width");
    if (width.isNumber()) btn->setWidth(width.toInt32());
    auto height = obj.property("height");
    if (height.isNumber()) btn->setHeight(height.toInt32());
    _layout->addWidget(btn);
    return btn;
    SCRIPT_EX_GUARD_END(nullptr)
}
/*^jsdoc
 * Creates a new {@link ng.view.TextField} and appends it to the container
 * @method addTextField
 * @memberof ng.view.Container
 * @instance
 * @param {string} text
 * @returns {ng.view.TextField}
 ^jsdoc*/
/*^jsdoc
 * Creates a new {@link ng.view.TextField} and appends it to the container
 * @method addTextField
 * @memberof ng.view.Container
 * @instance
 * @param {Object} params
 * @param {string} params.text
 * @param {boolean} [params.richText=false]
 * @returns {ng.view.TextField}
 ^jsdoc*/
TextField* Container::addTextField(const QScriptValue& obj) {
    QString text;
    bool richText = false;

    if (obj.isString()) {
        text = obj.toString();
    } else if (obj.isObject()) {
        auto text_ = obj.property("text");
        if (text_.isString()) text = text_.toString();
        auto richText_ = obj.property("richText");
        if (richText_.isBool()) richText = richText_.toBool();
    } else {
        SCRIPT_THROW("Wrong type")
        return nullptr;
    }

    TextField* tf = new TextField(this);
    tf->setText(text);
    tf->setRichText(richText);
    _layout->addWidget(tf);
    return tf;
}
/*^jsdoc
 * Creates a new {@link ng.view.CheckBox} and appends it to the container
 * @method addCheckBox
 * @memberof ng.view.Container
 * @instance
 * @param {string} text
 * @returns {ng.view.CheckBox}
 ^jsdoc*/
/*^jsdoc
 * Creates a new {@link ng.view.CheckBox} and appends it to the container
 * @method addCheckBox
 * @memberof ng.view.Container
 * @instance
 * @param {Object} params
 * @param {string} params.text
 * @param {boolean} [params.richText=false]
 * @param {boolean} [params.enabled=true]
 * @param {boolean} [params.preset=false] - default value (`onChange` will not
 be called when this is initially set)
 * @param {Function} [params.onChange]
 * @returns {ng.view.CheckBox}
 ^jsdoc*/
CheckBox* Container::addCheckBox(const QScriptValue& obj) {
    QString text;
    bool richText = false;
    bool preset = false;
    bool enabled = true;
    QScriptValue onChange(QScriptValue::UndefinedValue);

    if (obj.isString()) {
        text = obj.toString();
    } else if (obj.isObject()) {
        auto text_ = obj.property("text");
        if (text_.isString()) text = text_.toString();
        auto richText_ = obj.property("richText");
        if (richText_.isBool()) richText = richText_.toBool();
        // important: set this *before* onChange, so the handler isn't called
        // for default setup
        auto preset_ = obj.property("preset");
        if (preset_.isBool()) preset = preset_.toBool();
        onChange = obj.property("onChange");
        auto enabled_ = obj.property("enabled");
        if (enabled_.isBool()) enabled = enabled_.toBool();
    } else {
        SCRIPT_THROW("Wrong type")
        return nullptr;
    }

    CheckBox* cb = new CheckBox(this);
    cb->setText(text);
    cb->setRichText(richText);
    cb->setChecked(preset);
    cb->setOnChange(onChange);
    cb->setEnabled(enabled);
    _layout->addWidget(cb);
    return cb;
}
/*^jsdoc
 * Creates a new {@link ng.view.RadioGroup} and appends it to the container
 * @method addRadioGroup
 * @memberof ng.view.Container
 * @instance
 * @param {Object} params
 * @param {string} params.text
 * @param {Array<Object>} [params.options] - see {@link
 ng.view.RadioGroup#addOption}
 * @param {string} params.options.name
 * @param {string} params.options.text
 * @param {string} [params.preset] - default value (`onChange` will not be
 called when this is initially set)
 * @param {Function} [params.onChange]
 * @param {boolean} [params.vertical=false]
 * @returns {ng.view.RadioGroup}
 ^jsdoc*/
RadioGroup* Container::addRadioGroup(const QScriptValue& obj) {
    // TODO what about no parameters?
    if (!obj.isObject()) {
        SCRIPT_THROW("Wrong type")
        return nullptr;
    }

    bool vertical = false;
    auto vertical_ = obj.property("vertical");
    if (vertical_.isBool()) vertical = vertical_.toBool();

    RadioGroup* grp = new RadioGroup(this, vertical);

    auto text_ = obj.property("text");
    if (text_.isString()) grp->setText(text_.toString());

    auto options_ = obj.property("options");
    if (options_.isArray()) {
        QScriptValueIterator it(options_);
        while (it.hasNext()) {
            it.next();
            auto option = it.value();
            if (option.isObject()) {
                QString oName;
                QString oText;
                auto oName_ = option.property("name");
                if (oName_.isString()) oName = oName_.toString();
                auto oText_ = option.property("text");
                if (oText_.isString()) oText = oText_.toString();

                try {
                    grp->addOption(oName, oText);
                } catch (const NgException& ex) {
                    delete grp;
                    SCRIPT_THROW("Invalid option for RadioGroup")
                    return nullptr;
                }
            }
        }
    }

    grp->setOnChange(obj.property("onChange"));

    auto preset_ = obj.property("preset");
    if (preset_.isString()) {
        try {
            grp->setSelected(preset_.toString());
        } catch (const NgException& ex) {
            delete grp;
            SCRIPT_THROW("Invalid option for RadioGroup")
            return nullptr;
        }
    }

    _layout->addWidget(grp);
    return grp;
}
/*^jsdoc
 * Creates a new {@link ng.view.DirectoryPicker} and appends it to the container
 * @method addDirectoryPicker
 * @memberof ng.view.Container
 * @instance
 * @param {Object} params
 * @param {string} params.text
 * @param {boolean} [params.richText=false]
 * @param {string} [params.title]
 * @param {string} [params.preset] - default value
 * @param {Function} [params.onPick]
 * @param {Function} [params.adjustDirectory]
 * @returns {ng.view.DirectoryPicker}
 ^jsdoc*/
DirectoryPicker* Container::addDirectoryPicker(const QScriptValue& obj) {
    SCRIPT_EX_GUARD_START
    if (!obj.isObject()) {
        // TODO what about no parameters?
        SCRIPT_THROW("Wrong type")
        return nullptr;
    }

    DirectoryPicker* pick = new DirectoryPicker(this);

    auto text_ = obj.property("text");
    if (text_.isString()) pick->setText(text_.toString());
    auto richText_ = obj.property("richText");
    if (richText_.isBool()) pick->setRichText(richText_.toBool());
    auto title_ = obj.property("title");
    if (title_.isString()) pick->setTitle(title_.toString());
    auto preset_ = obj.property("preset");
    if (preset_.isString()) pick->setValue(preset_.toString());
    pick->setOnPick(obj.property("onPick"));
    pick->setAdjustDirectory(obj.property("adjustDirectory"));

    _layout->addWidget(pick);
    return pick;
    SCRIPT_EX_GUARD_END(nullptr)
}
/*^jsdoc
 * Creates a new {@link ng.view.Column} and appends it to the container
 * @method addColumn
 * @memberof ng.view.Container
 * @instance
 * @returns {ng.view.Column}
 ^jsdoc*/
Column* Container::addColumn() {
    SCRIPT_EX_GUARD_START
    Column* col = new Column(this);
    _layout->addWidget(col);
    return col;
    SCRIPT_EX_GUARD_END(nullptr)
}
/*^jsdoc
 * Creates a new {@link ng.view.Row} and appends it to the container
 * @method addRow
 * @memberof ng.view.Container
 * @instance
 * @returns {ng.view.Row}
 ^jsdoc*/
Row* Container::addRow() {
    SCRIPT_EX_GUARD_START
    Row* row = new Row(this);
    _layout->addWidget(row);
    return row;
    SCRIPT_EX_GUARD_END(nullptr)
}

}  // namespace view
}  // namespace api