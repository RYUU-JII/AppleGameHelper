// ScoreMode.cpp
#include "ScoreMode.h"
#include "AppController.h"
#include "ImageProcessor.h"
#include <QThread>
#include <windows.h>

ScoreMode::ScoreMode(QObject* parent)
    : ModeStrategy(parent), minScore(100) {
}

void ScoreMode::handleCapture(const CaptureResult& result, AppController* controller) {
    if (result.image.isNull()) {
        emit controller->captureFailed("캡처 실패");
        return;
    }

    ImageProcessor processor;
    tesseract::TessBaseAPI api;
    if (api.Init("./Resources/tessdata", "eng") != 0) {
        emit controller->captureFailed("Tesseract 초기화 실패");
        return;
    }

    cv::Mat mat = processor.qImageToMat(result.image.toImage());
    if (!processor.quickValidateOCR(mat, api)) {
        emit controller->captureFailed("OCR에 적합하지 않은 영역입니다.");
        return;
    }

    emit controller->captureCompleted(result.image);
    controller->requestOCR();
}

void ScoreMode::handleGridUpdate(const std::vector<std::vector<int>>& grid, AppController* controller) {
    controller->updateCurrentGrid(grid);
}

void ScoreMode::handleOCR(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path,
    AppController* controller) {
    int score = calculateScore(path);
    if (score >= minScore) {
        controller->updateCurrentGrid(grid);
        controller->updateRemovalPath(path);
        startAutomation(controller);
    }
    else {
        simulateResetAndStart(controller);
    }
}

void ScoreMode::startAutomation(AppController* controller) {
    if (!controller) return;
    controller->requestAuto();
}

void ScoreMode::setParameters(const QVariantMap& params) {
    if (params.contains("minScore")) {
        minScore = params["minScore"].toInt();
    }
}

int ScoreMode::calculateScore(const std::vector<RemovalStep>& path) const {
    return static_cast<int>(path.size()) * 10;
}

void ScoreMode::simulateClick(const QPoint& pos) {
    INPUT inputs[3] = {};

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    double xRatio = 65535.0 / screenW;
    double yRatio = 65535.0 / screenH;

    LONG x = static_cast<LONG>(pos.x() * xRatio);
    LONG y = static_cast<LONG>(pos.y() * yRatio);

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dx = x;
    inputs[0].mi.dy = y;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[2].type = INPUT_MOUSE;
    inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(3, inputs, sizeof(inputs));
}

void ScoreMode::simulateResetAndStart(AppController* controller) {
    if (!controller->isResetButtonSet() || !controller->isStartButtonSet()) {
        qWarning() << "Reset 또는 Start 버튼이 설정되지 않았습니다.";
        return;
    }

    simulateClick(controller->getResetButton());
    QThread::msleep(800);

    simulateClick(controller->getStartButton());
    QThread::msleep(1000);

    controller->requestCapture();
}

