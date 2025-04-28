#include "CaptureManager.h"
#include "AreaCaptureHelper.h"

CaptureManager::CaptureManager(QObject* parent)
    : QObject(parent),
    helper_(std::make_unique<AreaCaptureHelper>(this))
{
    connect(helper_.get(), &AreaCaptureHelper::captureFinished,
        this, &CaptureManager::onCaptureFinished);
    connect(helper_.get(), &AreaCaptureHelper::captureCanceled,
        this, &CaptureManager::onCaptureCanceled);
}

void CaptureManager::startCapture() {
    helper_->startInteractiveCapture();
}

void CaptureManager::onCaptureFinished(const CaptureResult& result) {
    qDebug() << "▶ CaptureManager::onCaptureFinished";
    emit captureFinished(result);
}

void CaptureManager::onCaptureCanceled() {
    emit captureCanceled();
}
