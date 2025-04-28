#include "AutomationManager.h"
#include "AppController.h"
#include "AutoWorker.h"
#include <QThread>
#include <QDebug>

AutomationManager::AutomationManager(AppController* parent)
    : QObject(parent), workerThread_(nullptr), worker_(nullptr)
{
    qDebug() << "AutomationManager::AutomationManager 초기화, this:" << this;
}

AutomationManager::~AutomationManager() {
    stopAutomation();
}

void AutomationManager::setupWorkerAndThread() {
    workerThread_ = new QThread(this);
    worker_ = std::make_unique<AutoWorker>();

    worker_->moveToThread(workerThread_);

    connect(worker_.get(), &AutoWorker::stepCompleted, this, &AutomationManager::stepCompleted);
    connect(worker_.get(), &AutoWorker::finished, this, &AutomationManager::onWorkerFinished);
    connect(worker_.get(), &AutoWorker::errorOccurred, this, &AutomationManager::errorOccurred);
    connect(workerThread_, &QThread::started, worker_.get(), &AutoWorker::run);
    connect(workerThread_, &QThread::finished, worker_.get(), &QObject::deleteLater);
    connect(workerThread_, &QThread::finished, workerThread_, &QObject::deleteLater);
}

void AutomationManager::startAutomation(const std::vector<RemovalStep>& steps, int startIndex, int delayMs) {
    qDebug() << "AutomationManager::startAutomation called, this:" << this
        << "steps size:" << steps.size()
        << "startIndex:" << startIndex
        << "delayMs:" << delayMs;

    if (workerThread_ && workerThread_->isRunning()) {
        qDebug() << "이전 자동화 작업이 아직 진행 중입니다.";
        return;
    }

    // 필요하면 새로 생성
    if (!workerThread_ || !worker_) {
        setupWorkerAndThread();
    }

    worker_->setSteps(steps, startIndex, delayMs);

    qDebug() << "Starting workerThread_, worker_:" << worker_.get();
    workerThread_->start();
    emit automationStarted();
}

void AutomationManager::stopAutomation() {
    qDebug() << "AutomationManager::stopAutomation 시작, this:" << this
        << "worker_:" << worker_.get()
        << "workerThread_:" << workerThread_;

    const bool wasRunning = (worker_ != nullptr && workerThread_ && workerThread_->isRunning());

    if (worker_) {
        qDebug() << "Calling worker_->stop, worker_:" << worker_.get();
        worker_->stop();
    }

    if (workerThread_) {
        qDebug() << "Stopping workerThread_, isRunning:" << workerThread_->isRunning();
        workerThread_->quit();
        if (!workerThread_->wait(1000)) {
            qWarning() << "Worker thread did not stop in time, thread:" << workerThread_;
        }
        else {
            qDebug() << "Worker thread stopped successfully";
        }
        workerThread_ = nullptr;
    }

    if (worker_) {
        worker_.release();
    }

    qDebug() << "AutomationManager::stopAutomation 완료";
}

void AutomationManager::onWorkerFinished() {
    qDebug() << "AutomationManager::onWorkerFinished called, cleaning up";

    workerThread_ = nullptr;
    worker_.release();
    emit automationFinished();
}
