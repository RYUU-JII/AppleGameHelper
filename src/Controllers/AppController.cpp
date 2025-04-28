#include "AppController.h"
#include "CaptureManager.h"
#include "OCRManager.h"
#include "AutomationManager.h"
#include "Strategies/NormalMode.h"
#include "Strategies/ScoreMode.h"
#include "Strategies/DataCollectionMode.h"
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <Windows.h>
#include <QEventLoop>
#include <QMessageBox>

AppController::AppController(QObject* parent)
    : QObject(parent),
    captureMgr_(std::make_unique<CaptureManager>(this)),
    ocrMgr_(std::make_unique<OCRManager>(this)),
    autoMgr_(std::make_unique<AutomationManager>(this)) {
    connect(captureMgr_.get(), &CaptureManager::captureFinished, this, &AppController::handleCapture);
    connect(captureMgr_.get(), &CaptureManager::captureCanceled, this, [&]() {
        emit captureFailed("캡처 취소");
        });
    connect(ocrMgr_.get(), &OCRManager::ocrFinished, this, &AppController::handleOCRResult);
    connect(ocrMgr_.get(), &OCRManager::ocrProgress, this, &AppController::ocrProgress);
    connect(autoMgr_.get(), &AutomationManager::stepCompleted, this, &AppController::handleAutoStep);
    connect(autoMgr_.get(), &AutomationManager::automationFinished, this, &AppController::handleAutoFinished);
    qDebug() << "AppController 생성";
}

AppController::~AppController() {
    qDebug() << "AppController 소멸";
}

void AppController::updateCurrentGrid(const std::vector<std::vector<int>>& grid) {
    state_.currentGrid = grid;
}

void AppController::updateRemovalPath(const std::vector<RemovalStep>& path) {
    state_.removalPath = path;
}

const std::vector<RemovalStep>& AppController::getRemovalPath() const {
    return state_.removalPath;
}

QPoint AppController::getResetButton() const {
    return state_.resetBtn;
}

QPoint AppController::getStartButton() const {
    return state_.startBtn;
}

void AppController::requestCapture() {
    state_.clear();
    emit captureStarted();
    captureMgr_->startCapture();
}

void AppController::handleCapture(const CaptureResult& result) {
    if (result.image.isNull()) {
        state_.capturedPixmap = QPixmap();
        emit captureFailed("유효하지 않은 영역");
        return;
    }

    state_.capturedPixmap = result.image;
    state_.captureArea = result.rect;

    emit captureCompleted(result.image);

    if (strategy_) {
        strategy_->handleCapture(result, this);
    }
}

void AppController::requestOCR() {
    if (state_.capturedPixmap.isNull()) {
        emit ocrFailed("캡처 먼저 수행");
        return;
    }
    if (ocrMgr_->isRunning()) {  // 추가: 실행 중인 작업 확인
        qDebug() << "OCR이 이미 실행 중입니다.";
        return;
    }
    emit ocrStarted();
    ocrMgr_->startOCR(state_.capturedPixmap.toImage());
}

void AppController::handleOCRResult(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path) {
    state_.currentGrid = grid;
    state_.removalPath = path;
    state_.currentStep = 0;
    state_.currentAutoStep = 0;
    emit ocrCompleted(grid);

    if (strategy_) strategy_->handleOCR(grid, path, this);
}

void AppController::requestAuto() {
    if (!strategy_) return;

    if (state_.removalPath.empty()) {
        QMessageBox::warning(
            nullptr,
            tr("자동 실행 오류"),
            tr("먼저 OCR을 수행하여 제거할 경로를 생성하세요.")
        );
        return;
    }

    emit automationStarted();
    autoMgr_->startAutomation(state_.currentGrid, state_.removalPath, state_.currentAutoStep);
}

void AppController::handleAutoStep(int idx, RemovalStep step) {
    qDebug() << "AppController::handleAutoStep 호출, idx:" << idx;
    state_.currentAutoStep = idx;
    performRemovalStep(step);
    emit autoStep(idx, step);
}

void AppController::handleAutoFinished() {
    qDebug() << "AppController::handleAutoFinished 호출";
    state_.isAutoCompleted = true;
    emit autoFinished();
}

void AppController::requestNext() {
    if (state_.removalPath.empty() || state_.currentStep >= state_.removalPath.size()) {
        emit nextFinished();
        return;
    }
    auto step = state_.removalPath[state_.currentStep++];
    performRemovalStep(step);
    emit nextStep(state_.currentStep, step);
    if (state_.currentStep >= state_.removalPath.size())
        emit nextFinished();
}

void AppController::beginResetSelection() {
    captureMgr_->startCapture();
}

void AppController::beginStartSelection() {
    captureMgr_->startCapture();
}

void AppController::setMode(const QString& mode, const QVariantMap& params) {
    if (mode == "일반 모드")      strategy_ = std::make_unique<NormalMode>();
    else if (mode == "점수 모드") strategy_ = std::make_unique<ScoreMode>();
    else                         strategy_ = std::make_unique<DataCollectionMode>();
    if (strategy_) strategy_->setParameters(params);
}

void AppController::performRemovalStep(const RemovalStep& step) {
    if (state_.captureArea.isNull() || state_.currentGrid.empty()) {
        qDebug() << "유효하지 않은 상태: 캡처 영역 또는 그리드 없음";
        return;
    }

    // 그리드 셀 크기 계산
    const int rows = state_.currentGrid.size();
    const int cols = state_.currentGrid[0].size();
    const int cellWidth = state_.captureArea.width() / cols;
    const int cellHeight = state_.captureArea.height() / rows;

    // 시작/종료 좌표 계산 (셀 중심 기준)
    const int startX = state_.captureArea.x() + step.y1 * cellWidth;
    const int startY = state_.captureArea.y() + step.x1 * cellHeight;
    const int endX = state_.captureArea.x() + (step.y2 + 1) * cellWidth;
    const int endY = state_.captureArea.y() + (step.x2 + 1) * cellHeight;

    // Windows API를 이용한 드래그 구현
    POINT originalPos;
    GetCursorPos(&originalPos);

    auto toAbsolute = [](int coord, int max) {
        return static_cast<LONG>((coord * 65535) / max);
        };

    constexpr int steps = 15;
    constexpr int delayMs = 5;

    INPUT startMove = { INPUT_MOUSE };
    startMove.mi.dx = toAbsolute(startX, GetSystemMetrics(SM_CXSCREEN));
    startMove.mi.dy = toAbsolute(startY, GetSystemMetrics(SM_CYSCREEN));
    startMove.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &startMove, sizeof(INPUT));
    QThread::msleep(5);

    INPUT mouseDown = { INPUT_MOUSE };
    mouseDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &mouseDown, sizeof(INPUT));
    QThread::msleep(5);

    for (int i = 1; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        int curX = startX + t * (endX - startX);
        int curY = startY + t * (endY - startY);

        INPUT dragMove = { INPUT_MOUSE };
        dragMove.mi.dx = toAbsolute(curX, GetSystemMetrics(SM_CXSCREEN));
        dragMove.mi.dy = toAbsolute(curY, GetSystemMetrics(SM_CYSCREEN));
        dragMove.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

        SendInput(1, &dragMove, sizeof(INPUT));
        QThread::msleep(delayMs);
    }

    INPUT mouseUp = { INPUT_MOUSE };
    mouseUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &mouseUp, sizeof(INPUT));

    SetCursorPos(originalPos.x, originalPos.y);
}

bool AppController::isResetButtonSet() const {
    return state_.isResetButtonSet();
}

bool AppController::isStartButtonSet() const {
    return state_.isStartButtonSet();
}

bool AppController::hasValidOCRResult() const {
    return !state_.currentGrid.empty();
}

bool AppController::hasValidCapture() const {
    return !state_.capturedPixmap.isNull();
}

// AppController.cpp
void AppController::onStopRequest() {
    if (autoMgr_) {
        autoMgr_->stopAutomation();
    }
}