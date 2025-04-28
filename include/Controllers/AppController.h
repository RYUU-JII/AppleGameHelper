#pragma once  
#include <QObject>
#include <memory>
#include "CaptureManager.h"
#include "AutomationManager.h"
#include "OCRManager.h"
#include "ModeStrategy.h"  
#include "AppState.h"  
#include "Types.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    ~AppController();

    // Public API for UI interactions
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
    void onStopRequest();  // Called by MainTab to stop automation
    void performRemovalStep(const RemovalStep& step);
    void updateCurrentGrid(const std::vector<std::vector<int>>& grid);
    void updateRemovalPath(const std::vector<RemovalStep>& path);
    const std::vector<RemovalStep>& getRemovalPath() const;
    QPoint getResetButton() const;
    QPoint getStartButton() const;

    bool isAutomationRunning() const;

signals:
    // Capture signals
    void captureStarted();
    void captureCompleted(const QPixmap& pixmap);  // Emitted when capture succeeds
    void captureFailed(const QString& error);      // Emitted when capture fails
    // OCR signals
    void ocrStarted();
    void ocrCompleted(const std::vector<std::vector<int>>& grid);  // Emitted when OCR completes
    void ocrFailed(const QString& error);                         // Emitted when OCR fails
    void ocrProgress(int percent);                                // OCR progress updates
    // Next / Auto signals
    void nextStep(int idx, RemovalStep step);     // Emitted for each manual step
    void nextFinished();                          // Emitted when manual steps complete
    void autoStep(int idx, RemovalStep step);     // Emitted for each auto step
    void autoFinished();                          // Emitted when automation completes
    void automationStarted();                     // Emitted when automation starts


private:
    AppState state_;
    std::unique_ptr<CaptureManager> captureMgr_;
    std::unique_ptr<OCRManager> ocrMgr_;
    std::unique_ptr<AutomationManager> autoMgr_;
    std::unique_ptr<ModeStrategy> strategy_;

    // Internal handlers
    void handleCapture(const CaptureResult& result);
    void handleOCRResult(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path);
    void handleAutoStep(int idx, RemovalStep step);
    void handleAutoFinished();
    static int getSignalReceivers(const QObject* obj, const char* signal);
};