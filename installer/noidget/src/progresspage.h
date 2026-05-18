#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <view/page.h>

class InvertProgressBar;

class ProgressPage : public view::Page {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

   public:
    explicit ProgressPage(QWidget* parent = 0);
    void next() override {}
    void back() override {}

    QString title() const override { return _titleLbl->text(); }

   signals:
    void titleChanged(const QString& v);

   private slots:
    void txSectionChange(int i, const QString& sectionTitle);
    void getLog(const QString& text);
    void updateProgressBar(const qint64 progress);

   private:
    QVBoxLayout* _layout;
    QLabel* _titleLbl;
    InvertProgressBar* _progressBar;
    QTextEdit* _logBox;

    int _sectionCount;
    bool _needProgressDownscale = false;
    double _progressDownscaleFactor;

    void startTx();
};