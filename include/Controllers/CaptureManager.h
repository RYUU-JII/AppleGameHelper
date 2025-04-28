#pragma once
#include <QObject>
#include <memory>
#include "Types.h"
#include "Shared/AreaCaptureHelper.h"

class CaptureManager : public QObject {
    Q_OBJECT
public:
    explicit CaptureManager(QObject* parent = nullptr);
    void startCapture();

signals:
    void captureFinished(const CaptureResult& result);
    void captureCanceled();

private:
    std::unique_ptr<AreaCaptureHelper> helper_;

private slots:
    void onCaptureFinished(const CaptureResult& result);
    void onCaptureCanceled();
};
