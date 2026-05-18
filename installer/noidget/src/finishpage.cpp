#include "finishpage.h"
#include "installerapplication.h"
#include <tx/transaction.h>

FinishPage::FinishPage(QWidget* parent) : view::Page(parent) {
    _layout = new QVBoxLayout(this);
    _layout->setMargin(0);
    _layout->setSpacing(8);
    _layout->setAlignment(Qt::AlignTop);
    setLayout(_layout);

    _titleLbl = new QLabel(this);
    _titleLbl->setTextFormat(Qt::PlainText);
    _titleLbl->setWordWrap(false);
    _titleLbl->setObjectName("pageHeading");
    _layout->addWidget(_titleLbl);

    _textBox = new QTextEdit(this);
    _textBox->setReadOnly(true);
    _textBox->setAcceptRichText(false);
    _layout->addWidget(_textBox);

    QString tpl;
    switch (ngApp->currentState()) {
        case InstallerApplication::State::Finished:
            _titleLbl->setText("Finished");
            tpl = ngApp->tx()->finishText();
            break;
        case InstallerApplication::State::Cancelled:
            _titleLbl->setText("Cancelled");
            tpl = ngApp->tx()->cancelText();
            break;
        case InstallerApplication::State::Error:
            _titleLbl->setText("Error");
            tpl = ngApp->tx()->errorText();
            break;
    }
    _textBox->setPlainText(
        tpl.replace("%LOGPATH%", ngApp->tx()->logFileName()));
}

void FinishPage::next() {
    if (ngApp->currentState() == InstallerApplication::State::Finished) {
        ngApp->tx()->runPost();
    }
    QApplication::quit();
}