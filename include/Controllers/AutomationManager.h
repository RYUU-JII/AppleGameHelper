#pragma once
#include <QObject>
#include <QThread>
#include "Types.h"
#include "AutoWorker.h"
#include <QPointer>

class AutoWorker;

class AutomationManager : public QObject {
    Q_OBJECT
public:
    explicit AutomationManager(QObject* parent = nullptr);

    void startAutomation(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path,
        int startIndex = 0);
    void stopAutomation();

signals:
    void stepCompleted(int idx, RemovalStep step);
    void automationFinished();

private slots:
    void handleStep(int idx, RemovalStep step);
    void handleFinished();

private:
    QThread* workerThread_ = nullptr;
    QPointer<AutoWorker> worker_;
};