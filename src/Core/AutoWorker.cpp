#include "AutoWorker.h"
#include <QThread>
#include <QDebug>

AutoWorker::AutoWorker(QObject* parent) : QObject(parent) {
    qDebug() << "AutoWorker::AutoWorker 초기화, this:" << this;
}

void AutoWorker::setSteps(const std::vector<RemovalStep>& steps, int startIndex, int delayMs) {
    qDebug() << "AutoWorker::setSteps called, steps size:" << steps.size()
        << "startIndex:" << startIndex << "delayMs:" << delayMs;
    m_removalSteps = steps;
    m_startIndex = startIndex;
    m_stepDelayMs = delayMs;
}

void AutoWorker::run() {
    m_stopped = false;
    qDebug() << "AutoWorker::run 시작, this:" << this << "total steps:" << m_removalSteps.size();
    try {
        for (int i = m_startIndex; i < static_cast<int>(m_removalSteps.size()); ++i) {
            if (m_stopped) {
                qDebug() << "AutoWorker::run stopped early, step:" << i;
                emit finished();
                return;
            }

            const auto& step = m_removalSteps[i];
            qDebug() << "AutoWorker::run emitting stepCompleted, step:" << i;
            emit stepCompleted(i, step);

            int elapsed = 0;
            const int interval = 10;

            while (elapsed < m_stepDelayMs && !m_stopped) {
                QThread::msleep(interval);
                elapsed += interval;
            }

            if (m_stopped) {
                qDebug() << "AutoWorker::run stopped during delay, step:" << i;
                emit finished();
                return;
            }
        }

        qDebug() << "AutoWorker::run 모든 단계 완료";
        emit finished();
    }
    catch (const std::exception& e) {
        qDebug() << "AutoWorker::run 오류:" << e.what();
        emit errorOccurred(QString("AutoWorker 오류: %1").arg(e.what()));
    }
}

void AutoWorker::stop() {
    m_stopped = true;
}