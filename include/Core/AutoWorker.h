#ifndef AUTOWORKER_H
#define AUTOWORKER_H

#include <windows.h>
#include <QObject>
#include <QRect>
#include <vector>
#include <atomic>
#include "Types.h"
#include "AppController.h"
#include <QPointer>

class AutoWorker : public QObject {
    Q_OBJECT
public:
    explicit AutoWorker(const std::vector<std::vector<int>>& initialGrid,
                      const std::vector<RemovalStep>& removalSteps,
                      AppController* controller,
                      int startIndex = 0,
                      QObject* parent = nullptr);

    void setStepDelay(int ms) { m_stepDelayMs = ms; }
    void stop();

public slots:
    void run();

signals:
    void stepCompleted(int idx, RemovalStep step);
    void finished();
    void errorOccurred(const QString& message);

private:
    const std::vector<std::vector<int>>& m_initialGrid;
    const std::vector<RemovalStep>& m_removalSteps;
    QPointer<AppController> m_controller;
    int m_stepDelayMs = 300;
	std::atomic<bool> m_stopped{ false };
    int m_startIndex = 0;
};

#endif // AUTOWORKER_H