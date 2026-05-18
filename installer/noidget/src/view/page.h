#pragma once

#include <QWidget>

namespace view {

class Page : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

   public:
    explicit Page(QWidget *parent = 0) : QWidget(parent) {}
    virtual ~Page() {}
    virtual void next() = 0;
    virtual void back() = 0;
    virtual void attached() {}
    virtual QString title() const = 0;

   signals:
    void titleChanged(const QString &v);
    void popRequested();
    void nextEnabled(bool enabled);
    void backEnabled(bool enabled);
};

}  // namespace view