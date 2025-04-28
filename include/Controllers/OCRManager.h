#pragma once
#include <QObject>
#include <memory>
#include "Types.h"
#include <QThread>

class OCRWorker;

class OCRManager : public QObject {
    Q_OBJECT

public:
    explicit OCRManager(QObject* parent = nullptr);
    ~OCRManager();
    
    void startOCR(const QImage& image);
    void stopOCR();
    bool isRunning() const;

signals:
    void ocrFinished(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path);
    void ocrProgress(int percent);

private slots:
    void handleFinished(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path);

private:
    void setupWorkerAndThread(const QImage& image);

private:
    QThread* workerThread_;        // OCRWorker를 돌리는 전용 스레드
    std::unique_ptr<OCRWorker> worker_; // OCR 실제 처리하는 워커
};