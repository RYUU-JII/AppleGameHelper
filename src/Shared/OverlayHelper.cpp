#include "OverlayHelper.h"
#include <QGuiApplication>
#include <QScreen>

OverlayHelper::OverlayHelper(QWidget* parent)
    : QObject(parent), overlayLabel(new QLabel(nullptr))  // 부모 없는 QLabel로 설정
{
    overlayLabel->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    overlayLabel->setAttribute(Qt::WA_TranslucentBackground, false);
    overlayLabel->setAutoFillBackground(true);

    overlayLabel->setStyleSheet(R"(
        QLabel {
            background-color: rgba(30, 30, 30, 200);
            color: #FF4444;
            padding: 8px 12px;
            border-radius: 6px;
            font-weight: bold;
            font-size: 14px;
        }
    )");
    overlayLabel->hide();
}

void OverlayHelper::showOverlay(const QString& msg) {
    overlayLabel->setText(msg);
    overlayLabel->adjustSize();

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int centerX = screenGeometry.x() + (screenGeometry.width() - overlayLabel->width()) / 2;
    int centerY = screenGeometry.y() + screenGeometry.height() - overlayLabel->height() - 80;

    overlayLabel->move(centerX, centerY);
    overlayLabel->show();
}

void OverlayHelper::hideOverlay() {
    if (overlayLabel) {
        overlayLabel->hide();
    }
}
