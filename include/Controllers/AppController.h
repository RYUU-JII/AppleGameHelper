#pragma once
#include <QObject>
#include <memory>
#include "Strategies/ModeStrategy.h"
#include "AppState.h"
#include "Types.h"

class CaptureManager;
class OCRManager;
class AutomationManager;

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    ~AppController();

    // public API
    void requestCapture();
    void requestOCR();
    void requestAuto();
    void requestNext();
    void beginResetSelection();
    void beginStartSelection();
    void setMode(const QString& mode, const QVariantMap& params);
    bool isResetButtonSet() const;
    bool isStartButtonSet() const;
    bool hasValidOCRResult() const;
    bool hasValidCapture() const;
    void onStopRequest();  // MainTab에서 호출할 수 있도록
    void performRemovalStep(const RemovalStep& step);
    void updateCurrentGrid(const std::vector<std::vector<int>>& grid);
    void updateRemovalPath(const std::vector<RemovalStep>& path);
    const std::vector<RemovalStep>& getRemovalPath() const;
    QPoint getResetButton() const;
    QPoint getStartButton() const;

signals:
    // Capture
    void captureStarted();
    void captureCompleted(const QPixmap& pixmap);
    void captureFailed(const QString& error);
    // OCR
    void ocrStarted();
    void ocrCompleted(const std::vector<std::vector<int>>& grid);
    void ocrFailed(const QString& error);
    void ocrProgress(int percent);
    // Next / Auto
    void nextStep(int idx, RemovalStep step);
    void nextFinished();
    void autoStep(int idx, RemovalStep step);
    void autoFinished();
    void automationStarted();

private:
    AppState state_;
    std::unique_ptr<CaptureManager> captureMgr_;
    std::unique_ptr<OCRManager> ocrMgr_;
    std::unique_ptr<AutomationManager> autoMgr_;
    std::unique_ptr<ModeStrategy> strategy_;

    // internal handlers
    void handleCapture(const CaptureResult& result);
    void handleOCRResult(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path);
    void handleAutoStep(int idx, RemovalStep step);
    void handleAutoFinished();
    
    // perform actual mouse drag for a step
    
};
