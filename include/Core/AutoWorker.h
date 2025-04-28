#pragma once
#include <QObject>
#include "Types.h"

class AutoWorker : public QObject {
    Q_OBJECT
public:
    explicit AutoWorker(QObject* parent = nullptr);
    void setSteps(const std::vector<RemovalStep>& steps, int startIndex, int delayMs);
    void stop();

public slots:
    void run();

signals:
    void stepCompleted(int idx, RemovalStep step);
    void finished();
    void errorOccurred(const QString& error);

private:
    std::vector<RemovalStep> m_removalSteps;
    int m_startIndex = 0;
    int m_stepDelayMs = 50;
    bool m_stopped = false;
};