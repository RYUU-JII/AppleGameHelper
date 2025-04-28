#pragma once
#include <QPixmap>
#include <QRect>
#include <QPoint>
#include <vector>
#include "Types.h"  // CaptureResult, RemovalStep 등 정의

class AppState {
public:
    QPixmap capturedPixmap;
    QRect captureArea;
    std::vector<std::vector<int>> currentGrid;
    std::vector<RemovalStep> removalPath;
    QPoint startBtn;
    QPoint resetBtn;
    int currentStep = 0;
    int currentAutoStep = 0;
    bool autoPaused = false;
    bool isAutoCompleted = false;

    bool isResetButtonSet() const { return !resetBtn.isNull(); }
    bool isStartButtonSet() const { return !startBtn.isNull(); }

    void clear() {
        capturedPixmap = QPixmap();
        captureArea = QRect();
        currentGrid.clear();
        removalPath.clear();
        startBtn = QPoint();
        resetBtn = QPoint();
        currentStep = currentAutoStep = 0;
        autoPaused = isAutoCompleted = false;
    }
};
