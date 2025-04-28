#include "OCRManager.h"
#include "OCRWorker.h"
#include <QDebug>

OCRManager::OCRManager(QObject* parent)
    : QObject(parent)
{
}

void OCRManager::startOCR(const QImage& image) {
    // 기존 작업이 실행 중이면 완료될 때까지 대기
    if (workerThread_ && workerThread_->isRunning()) {
        qDebug() << "이전 OCR 작업이 아직 진행 중입니다.";
        return;
    }

    workerThread_ = new QThread(this);
    worker_ = new OCRWorker(image);
    worker_->moveToThread(workerThread_);

    connect(workerThread_, &QThread::started, worker_, &OCRWorker::run);
    connect(worker_, &OCRWorker::finished, this, &OCRManager::handleFinished);
    connect(worker_, &OCRWorker::progress, this, &OCRManager::ocrProgress);
    connect(workerThread_, &QThread::finished, workerThread_, &QObject::deleteLater);
    connect(workerThread_, &QThread::finished, worker_, &QObject::deleteLater);

    workerThread_->start();
}

void OCRManager::handleFinished(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path) {
    emit ocrFinished(grid, path);
    if (workerThread_) {
        workerThread_->quit();
        workerThread_->wait(2000); // 최대 2초 대기
        workerThread_->deleteLater();
        workerThread_ = nullptr;
    }
    if (worker_) {
        worker_->deleteLater();
        worker_ = nullptr;
    }
}