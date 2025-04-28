#pragma once
#include <QObject>
#include <memory>
#include "Types.h"
#include "QThread"

class AutoWorker;
class AppController;

class AutomationManager : public QObject {
    Q_OBJECT
public:
    explicit AutomationManager(AppController* parent = nullptr);
    ~AutomationManager();
    void startAutomation(const std::vector<RemovalStep>& steps, int startIndex = 0, int delayMs = 200);
    void stopAutomation();
    bool isRunning() const { return workerThread_ && workerThread_->isRunning(); }

signals:
    void automationStarted();
    void automationFinished();
    void stepCompleted(int idx, RemovalStep step);
    void errorOccurred(const QString& error);

private:
    void setupWorkerAndThread();
    void onWorkerFinished();
    std::unique_ptr<AutoWorker> worker_;
    QThread* workerThread_ = nullptr;
};