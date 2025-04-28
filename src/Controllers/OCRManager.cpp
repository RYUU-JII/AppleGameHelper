#include "OCRManager.h"
#include "OCRWorker.h"
#include <QThread>
#include <QDebug>

OCRManager::OCRManager(QObject* parent)
    : QObject(parent), workerThread_(nullptr), worker_(nullptr)
{
    qDebug() << "OCRManager::OCRManager 초기화, this:" << this;
}

OCRManager::~OCRManager() {
    stopOCR();
}

bool OCRManager::isRunning() const {
    return workerThread_ && workerThread_->isRunning();
}

void OCRManager::setupWorkerAndThread(const QImage& image) {
    workerThread_ = new QThread(this);
    worker_ = std::make_unique<OCRWorker>(image);

    worker_->moveToThread(workerThread_);

    connect(workerThread_, &QThread::started, worker_.get(), &OCRWorker::run);
    connect(worker_.get(), &OCRWorker::finished, this, &OCRManager::handleFinished);
    connect(worker_.get(), &OCRWorker::progress, this, &OCRManager::ocrProgress);

    connect(workerThread_, &QThread::finished, worker_.get(), &QObject::deleteLater);
    connect(workerThread_, &QThread::finished, workerThread_, &QObject::deleteLater);
}

void OCRManager::startOCR(const QImage& image) {
    qDebug() << "OCRManager::startOCR called, this:" << this << "image size:" << image.size();

    if (workerThread_ && workerThread_->isRunning()) {
        qDebug() << "이전 OCR 작업이 아직 진행 중입니다.";
        return;
    }

    if (!workerThread_ || !worker_) {
        setupWorkerAndThread(image);
    }

    qDebug() << "Starting workerThread_, worker_:" << worker_.get();
    workerThread_->start();
}

void OCRManager::stopOCR() {
    qDebug() << "OCRManager::stopOCR called, this:" << this;

    if (worker_) {
        // stop 일단 없음
    }

    if (workerThread_) {
        workerThread_->quit();
        if (!workerThread_->wait(2000)) {
            qWarning() << "Worker thread did not stop in time.";
        }
        workerThread_ = nullptr;
    }

    if (worker_) {
        worker_.release();
    }
}

void OCRManager::handleFinished(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path) {
    qDebug() << "OCRManager::handleFinished called, this:" << this;

    emit ocrFinished(grid, path);

    stopOCR();

    qDebug() << "OCRManager::handleFinished 완료";
}
