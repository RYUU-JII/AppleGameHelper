#include "AreaCaptureHelper.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include "Types.h"

AreaCaptureHelper::AreaCaptureHelper(QObject* parent) : QObject(parent) {}

void AreaCaptureHelper::startInteractiveCapture() {
    selector_ = std::make_unique<RegionSelector>();
    connect(selector_.get(), &RegionSelector::regionSelected, this, &AreaCaptureHelper::handleRegionSelected);
    connect(selector_.get(), &RegionSelector::selectionCanceled, this, &AreaCaptureHelper::handleSelectionCanceled);
    selector_->startSelection();
}

void AreaCaptureHelper::handleRegionSelected(const QRect& rect) {
    captureRegion(rect);
    if (selector_) {
        selector_->deleteLater();
        selector_.release();
    }
}

void AreaCaptureHelper::handleSelectionCanceled() {
    selector_.reset();
    emit captureCanceled();
}

void AreaCaptureHelper::captureRegion(const QRect& rect) {
    CaptureResult result;
    if (auto screen = QGuiApplication::primaryScreen()) {
        QRect validRect = rect.intersected(screen->geometry());
        if (validRect.width() < 10 || validRect.height() < 10) {
            qWarning() << "Invalid capture rect:" << validRect;
        }
        else {
            result.image = screen->grabWindow(0, validRect.x(), validRect.y(), validRect.width(), validRect.height());
            result.rect = validRect;
        }
    }
    emit captureFinished(result);
}
