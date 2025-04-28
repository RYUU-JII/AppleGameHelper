#pragma once
#include <QObject>
#include <memory>
#include "RegionSelector.h"
#include "Types.h"

class AreaCaptureHelper : public QObject {
    Q_OBJECT
public:
    explicit AreaCaptureHelper(QObject* parent = nullptr);
    void startInteractiveCapture();

signals:
    void captureFinished(const CaptureResult& result);
    void captureCanceled();

private slots:
    void handleRegionSelected(const QRect& rect);
    void handleSelectionCanceled();

private:
    void captureRegion(const QRect& rect);

private:
    std::unique_ptr<RegionSelector> selector_;
};
