#include "AutoWorker.h"
#include "AppController.h"
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

AutoWorker::AutoWorker(const std::vector<std::vector<int>>& initialGrid,
    const std::vector<RemovalStep>& removalSteps,
    AppController* controller,
    int startIndex,
    QObject* parent)
    : QObject(parent),
    m_initialGrid(initialGrid),
    m_removalSteps(removalSteps),
    m_controller(controller),
    m_startIndex(startIndex) {
    qDebug() << "AutoWorker 생성, controller:" << m_controller.data();
}

void AutoWorker::run() {
	m_stopped = false; // 초기화
    try {
        qDebug() << "AutoWorker::run 시작. 총 단계:" << m_removalSteps.size();

        for (int i = m_startIndex; i < static_cast<int>(m_removalSteps.size()); ++i) {
            if (m_stopped) {
                qDebug() << "작업 중단됨";
                emit finished();
                return;
            }

            const auto& step = m_removalSteps[i];
            // qDebug() << "AutoWorker::run 단계" << i << ": (" << step.x1 << "," << step.y1 << ") -> (" << step.x2 << "," << step.y2 << ")";

            emit stepCompleted(i, step);

            // QEventLoop로 대기, 인터럽트 즉시 반영
            QEventLoop loop;
            QTimer timer;
            timer.setSingleShot(true);
            connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            connect(QThread::currentThread(), &QThread::requestInterruption, &loop, &QEventLoop::quit);
            timer.start(m_stepDelayMs);
            loop.exec();
            if (QThread::currentThread()->isInterruptionRequested()) {
                qDebug() << "AutoWorker::run 대기 중 인터럽트 감지, 중단";
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
    qDebug() << "AutoWorker::stop 호출";
    m_stopped = true; // 플래그 추가
}