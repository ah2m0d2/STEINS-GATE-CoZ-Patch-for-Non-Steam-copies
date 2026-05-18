#pragma once

#include <QException>

// For cross-thread exceptions, see http://doc.qt.io/qt-5/qexception.html
class NgException : public QException {
   public:
    void raise() const { throw *this; }
    NgException* clone() const { return new NgException(*this); }

    NgException(const QString& what_arg) : QException(), _what(what_arg) {}
    NgException(const std::string& what_arg) : QException() {
        _what = QString::fromStdString(what_arg);
    }
    NgException(const char* what_arg) : QException() {
        _what = QString::fromUtf8(what_arg);
    }

    const char* what() const noexcept { return _what.toUtf8().constData(); }
    QString qWhat() const noexcept { return _what; }

   private:
    QString _what;
};