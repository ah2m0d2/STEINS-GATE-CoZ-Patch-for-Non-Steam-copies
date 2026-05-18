#include "installerwindow.h"
#include "ui_installerwindow.h"
#include <view/page.h>
#include "installerapplication.h"
#include <tx/transaction.h>
#include "finishpage.h"

#include <QMouseEvent>
#include <QToolButton>
#include <QMessageBox>
#include <QFileInfo>

InstallerWindow::InstallerWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::InstallerWindow) {
    setWindowFlag(Qt::FramelessWindowHint);
    ui->setupUi(this);

    ui->headerImage->setStyleSheet("background-color: #000");

    ui->headerImage->installEventFilter(this);

    if (QFileInfo(":/userdata/header.png").exists()) {
        ui->headerImage->setPixmap(QPixmap(":/userdata/header.png"));
    }
    if (QFileInfo(":/userdata/left.png").exists()) {
        ui->leftImage->setPixmap(QPixmap(":/userdata/left.png"));
        QMargins margins = ui->belowHeaderColumns->contentsMargins();
        margins.setLeft(margins.left() - 4);
        ui->belowHeaderColumns->setContentsMargins(margins);
    } else {
        ui->belowHeaderColumns->removeWidget(
            ui->leftImage);  // get rid of spacing
    }
    if (QFileInfo(":/userdata/right.png").exists()) {
        ui->rightImage->setPixmap(QPixmap(":/userdata/right.png"));
        QMargins margins = ui->belowHeaderColumns->contentsMargins();
        margins.setLeft(margins.right() - 4);
        ui->belowHeaderColumns->setContentsMargins(margins);
    } else {
        ui->belowHeaderColumns->removeWidget(
            ui->rightImage);  // get rid of spacing
    }
    if (QFileInfo(":/userdata/next_button.png").exists()) {
        ui->nextButton->setIcon(QIcon(":/userdata/next_button.png"));
        ui->nextButton->setIconSize(QSize(24, 24));
        ui->nextButton->setProperty("hasIcon", true);
        // following is needed for dynamic property based styles to update
        ui->nextButton->style()->unpolish(ui->nextButton);
        ui->nextButton->style()->polish(ui->nextButton);
    }
    if (QFileInfo(":/userdata/back_button.png").exists()) {
        ui->backButton->setIcon(QIcon(":/userdata/back_button.png"));
        ui->backButton->setIconSize(QSize(24, 24));
        ui->backButton->setProperty("hasIcon", true);
        // following is needed for dynamic property based styles to update
        ui->backButton->style()->unpolish(ui->backButton);
        ui->backButton->style()->polish(ui->backButton);
    }

    // gross but I haven't found any way to overlay a container over other
    // widgets while passing through input events that don't hit any of its
    // child widgets
    QToolButton *crossButton = new QToolButton(this);
    crossButton->setStyleSheet(
        "QToolButton { image: url(':/cross.png'); }"
        "QToolButton:hover { image: url(':/cross_hover.png'); }");
    crossButton->setCursor(QCursor(Qt::PointingHandCursor));
    crossButton->resize(18, 18);
    crossButton->move(width() - (crossButton->width() + 12), 12);
    connect(crossButton, &QAbstractButton::clicked, this,
            &InstallerWindow::cancelRequested);
    _muteButton = new QToolButton(this);
    _muteButton->setStyleSheet(
        "QToolButton { image: url(':/mute.png'); }"
        "QToolButton:hover { image: url(':/mute_hover.png'); }");
    _muteButton->setCursor(QCursor(Qt::PointingHandCursor));
    _muteButton->resize(18, 18);
    _muteButton->move(crossButton->x() - (_muteButton->width() + 8), 12);
    connect(_muteButton, &QAbstractButton::clicked, this,
            &InstallerWindow::on_muteButton_clicked);

    bgmPlayer = new BgmPlayer(this);

    // only show muteButton when BGM has started playing
    connect(bgmPlayer, &BgmPlayer::bgmAvailabilityChanged, this,
            &InstallerWindow::onBgmAvailabilityChanged);
    _muteButton->setVisible(false);

    connect(ngApp, &InstallerApplication::currentStateChanged, this,
            &InstallerWindow::handleAppStateChange);
    connect(ui->cancelButton, &QAbstractButton::clicked, this,
            &InstallerWindow::cancelRequested);
}

InstallerWindow::~InstallerWindow() { delete ui; }

bool InstallerWindow::eventFilter(QObject *watched, QEvent *event) {
    static QPoint lastPos;
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        lastPos = mouseEvent->globalPos();
        event->accept();
        return true;
    }
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        move(pos() + (mouseEvent->globalPos() - lastPos));
        lastPos = mouseEvent->globalPos();
        event->accept();
        return true;
    }

    return false;
}

void InstallerWindow::handleAppStateChange(
    InstallerApplication::State newState) {
    switch (newState) {
        case InstallerApplication::State::Installing:
            ui->nextButton->setEnabled(false);
            ui->backButton->setEnabled(false);
            break;
        case InstallerApplication::State::Finished:
            ui->nextButton->setText("Finish");
            ui->nextButton->setEnabled(true);
            ui->backButton->setEnabled(false);
            ui->cancelButton->setEnabled(false);
            push(new FinishPage(this));
            break;
        case InstallerApplication::State::Cancelled:
        case InstallerApplication::State::Error:
            ui->nextButton->setText("Quit");
            ui->nextButton->setEnabled(true);
            ui->backButton->setEnabled(false);
            ui->cancelButton->setEnabled(false);
            push(new FinishPage(this));
            break;
    }
}

void InstallerWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    cancelRequested();
}

void InstallerWindow::cancelRequested() {
    if (ngApp->currentState() == InstallerApplication::State::Cancelled ||
        ngApp->currentState() == InstallerApplication::State::Error) {
        // X button, alt+f4
        QApplication::quit();
        return;
    }
    if (ngApp->currentState() == InstallerApplication::State::Finished) {
        // Post-finish commands
        currentPage()->next();
        return;
    }

    QString question = "Really abort the process?";
    if (ngApp->currentState() == InstallerApplication::State::Installing) {
        question += "\n\nChanges already made will not be undone!";
    }
    QMessageBox mb(this);
    mb.setText(question);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (mb.exec() != QMessageBox::Yes) return;

    // This is kinda racey...
    if (ngApp->currentState() == InstallerApplication::State::Installing) {
        ngApp->setCurrentState(InstallerApplication::State::Cancelled);
    } else {
        QApplication::quit();
    }
}

void InstallerWindow::on_muteButton_clicked() { bgmPlayer->togglePaused(); }

void InstallerWindow::push(view::Page *page) {
    connect(page, &view::Page::nextEnabled, this,
            &InstallerWindow::page_nextEnabled);
    connect(page, &view::Page::backEnabled, this,
            &InstallerWindow::page_backEnabled);
    connect(page, &view::Page::popRequested, this,
            &InstallerWindow::page_popRequested);
    const int i = ui->stackedWidget->addWidget(page);
    ui->stackedWidget->setCurrentIndex(i);  // TODO necessary?
    page->attached();
}

void InstallerWindow::pop() {
    view::Page *top = currentPage();
    removePage(top);
}

view::Page *InstallerWindow::currentPage() {
    return qobject_cast<view::Page *>(ui->stackedWidget->currentWidget());
}

void InstallerWindow::setBgm(const QString &url, uint32_t loopStart,
                             uint32_t loopEnd) {
    bgmPlayer->setBgm(url, loopStart, loopEnd);
}

void InstallerWindow::onBgmAvailabilityChanged(bool available) {
    if (_muteButton) {
        _muteButton->setVisible(available);
    }
}

void InstallerWindow::on_nextButton_clicked() {
    view::Page *top = currentPage();
    if (top != nullptr) {
        top->next();
    }
}

void InstallerWindow::on_backButton_clicked() {
    view::Page *top = currentPage();
    if (top != nullptr) {
        top->back();
    }
}

void InstallerWindow::on_stackedWidget_currentChanged(int i) {
    if (ui->stackedWidget->count() <= 1) {
        ui->backButton->setEnabled(false);
    }
}

void InstallerWindow::page_nextEnabled(bool enabled) {
    ui->nextButton->setEnabled(enabled);
}

void InstallerWindow::page_backEnabled(bool enabled) {
    ui->backButton->setEnabled(ui->stackedWidget->count() > 1 && enabled);
}

void InstallerWindow::page_popRequested() {
    view::Page *page = qobject_cast<view::Page *>(sender());
    removePage(page);
}

void InstallerWindow::removePage(view::Page *page) {
    if (page != nullptr) {
        disconnect(page, 0, this, 0);
        disconnect(this, 0, page, 0);

        ui->stackedWidget->removeWidget(page);
        delete page;
    }
}