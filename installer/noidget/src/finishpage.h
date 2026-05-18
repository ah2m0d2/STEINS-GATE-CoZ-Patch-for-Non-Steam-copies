#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <view/page.h>

class FinishPage : public view::Page {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

   public:
    explicit FinishPage(QWidget* parent = 0);
    void next() override;
    void back() override {}

    QString title() const override { return _titleLbl->text(); }

   signals:
    void titleChanged(const QString& v);

   private:
    QVBoxLayout* _layout;
    QLabel* _titleLbl;
    QTextEdit* _textBox;
};