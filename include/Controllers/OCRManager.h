#pragma once
#include <QObject>
#include <QThread>
#include "Types.h"

class OCRWorker;

class OCRManager : public QObject {
    Q_OBJECT
public:
    explicit OCRManager(QObject* parent = nullptr);
    void startOCR(const QImage& image);
    bool isRunning() const { return workerThread_ && workerThread_->isRunning(); }

signals:
    void ocrProgress(int percent);
    void ocrFinished(const std::vector<std::vector<int>>& grid, const std::vector<RemovalStep>& path);

private:
    QThread* workerThread_ = nullptr;
    OCRWorker* worker_ = nullptr;

private slots:
    void handleFinished(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path);
};
