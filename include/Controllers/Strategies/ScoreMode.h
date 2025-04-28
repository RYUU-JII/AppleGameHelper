// ScoreMode.h
#pragma once

#include "ModeStrategy.h"
#include "Types.h"      // CaptureResult, RemovalStep 등 정의
#include <QVariantMap>
#include <vector>
#include <QPoint>
#include <QObject>

class AppController;

class ScoreMode : public ModeStrategy {
    Q_OBJECT
public:
    explicit ScoreMode(QObject* parent = nullptr);
    void handleCapture(const CaptureResult& result, AppController* controller) override;
    void handleOCR(const std::vector<std::vector<int>>& grid,
        const std::vector<RemovalStep>& path,
        AppController* controller) override;
    void startAutomation(AppController* controller) override;
    void setParameters(const QVariantMap& params) override;
    void handleGridUpdate(const std::vector<std::vector<int>>& grid, AppController* controller) override;

private:
    int calculateScore(const std::vector<RemovalStep>& path) const;
    void simulateClick(const QPoint& pos);
    void simulateResetAndStart(AppController* controller);

    int minScore;
};
