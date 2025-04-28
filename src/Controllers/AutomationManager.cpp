#include "AutomationManager.h"
#include "AutoWorker.h"
#include <QCoreApplication>
#include <QDebug>

AutomationManager::AutomationManager(QObject* parent) : QObject(parent) {}

void AutomationManager::startAutomation(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path,
    int startIndex) {
    stopAutomation();

    workerThread_ = new QThread(this);
    worker_ = new AutoWorker(grid, path, qobject_cast<AppController*>(parent()), startIndex);

    worker_->moveToThread(workerThread_);
    connect(workerThread_, &QThread::started, worker_, &AutoWorker::run);
    connect(worker_, &AutoWorker::stepCompleted, this, &AutomationManager::handleStep);
    connect(worker_, &AutoWorker::finished, this, &AutomationManager::handleFinished);
    connect(worker_, &AutoWorker::errorOccurred, this, [](const QString& error) {
        qDebug() << "AutomationManager: Worker 오류:" << error;
        });
    connect(workerThread_, &QThread::finished, worker_, &QObject::deleteLater);
    connect(workerThread_, &QThread::finished, workerThread_, &QObject::deleteLater);

    qDebug() << "AutomationManager::startAutomation 시작, worker:" << worker_.data();
    workerThread_->start();
}

void AutomationManager::stopAutomation() {
    qDebug() << "AutomationManager::stopAutomation 시작";
    bool hadAutomation = (worker_ != nullptr || workerThread_ != nullptr);

    if (worker_) {
        worker_->stop(); // 원자적 중단 요청
        worker_->disconnect(); // 모든 시그널 연결 해제
    }
    if (workerThread_) {
        workerThread_->requestInterruption();
        workerThread_->quit();
        if (!workerThread_->wait(1000)) { // 타임아웃 1초
            workerThread_->terminate();
            workerThread_->wait();
        }
        workerThread_->deleteLater(); // 메인 스레드에서 삭제 보장
    }
    worker_ = nullptr;
    workerThread_ = nullptr;

    if (hadAutomation) {
        emit automationFinished();
    }

    qDebug() << "AutomationManager::stopAutomation 완료";
}

void AutomationManager::handleStep(int idx, RemovalStep step) {
    qDebug() << "AutomationManager::handleStep 호출, idx:" << idx;
    emit stepCompleted(idx, step);
}

void AutomationManager::handleFinished() {
    qDebug() << "AutomationManager::handleFinished 호출";
    emit automationFinished();
}