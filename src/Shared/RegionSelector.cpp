#include "RegionSelector.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

RegionSelector::RegionSelector(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    if (auto screen = QGuiApplication::primaryScreen()) {
        setGeometry(screen->geometry());
    }
}

void RegionSelector::startSelection() {
    show();
    activateWindow();
    grabMouse();
}

void RegionSelector::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 100));
    if (selecting_) {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(selectionRect_, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawRect(selectionRect_);
    }
}

void RegionSelector::mousePressEvent(QMouseEvent* event) {
    selecting_ = true;
    origin_ = event->pos();
    selectionRect_ = QRect(origin_, QSize());
    update();
}

void RegionSelector::mouseMoveEvent(QMouseEvent* event) {
    if (selecting_) {
        selectionRect_ = QRect(origin_, event->pos()).normalized();
        update();
    }
}

void RegionSelector::mouseReleaseEvent(QMouseEvent* event) {
    if (selecting_) {
        selecting_ = false;
        releaseMouse();
        QRect rect = QRect(origin_, event->pos()).normalized();
        QScreen* screen = QGuiApplication::primaryScreen();
        rect = rect.intersected(screen->geometry());
        if (rect.isEmpty() || rect.width() < 10 || rect.height() < 10) {
            emit selectionCanceled();
        }
        else {
            emit regionSelected(rect);
        }
        QTimer::singleShot(50, this, &RegionSelector::close);
    }
}

void RegionSelector::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        releaseMouse();
        emit selectionCanceled();
        QTimer::singleShot(0, this, &RegionSelector::close);
    }
}
