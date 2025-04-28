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
#include <QMetaobject>
#include <QMetaMethod>

// ----------------------------------------------------------------------------
// 시그널 연결 개수 디버깅 매크로
#define DEBUG_SIGNAL_CONNECTIONS(obj, signal) \
    qDebug() << "Signal connections for " #signal ":" \
             << getSignalReceivers(obj, #signal)

// 시그널 문자열로 QObject::receivers() 호출
// Replace the existing getSignalReceivers function with the following implementation

int AppController::getSignalReceivers(const QObject* obj, const char* signal) {
    QByteArray sig = QMetaObject::normalizedSignature(signal);
    int index = obj->metaObject()->indexOfSignal(sig);
    if (index < 0) {
        qWarning() << "Signal not found:" << signal;
        return 0;
    }

    int connectionCount = 0;
    const QMetaObject* metaObject = obj->metaObject();
    for (int i = 0; i < metaObject->methodCount(); ++i) {
        QMetaMethod method = metaObject->method(i);
        if (method.methodType() == QMetaMethod::Signal && method.methodSignature() == sig) {
            connectionCount++;
        }
    }
    return connectionCount;
}
// ----------------------------------------------------------------------------

AppController::AppController(QObject* parent)
    : QObject(parent),
    captureMgr_(std::make_unique<CaptureManager>(this)),
    ocrMgr_(std::make_unique<OCRManager>(this)),
    autoMgr_(std::make_unique<AutomationManager>(this))
{
    qDebug() << "AppController 생성, this =" << this;

    // 캡처 완료/취소
    connect(captureMgr_.get(), &CaptureManager::captureFinished,
        this, &AppController::handleCapture);
    connect(captureMgr_.get(), &CaptureManager::captureCanceled,
        this, [this]() {
            qDebug() << "Capture canceled, emitting captureFailed";
            DEBUG_SIGNAL_CONNECTIONS(this, captureFailed(QString));
            emit captureFailed("캡처 취소");
        });

    // OCR 완료/진행
    connect(ocrMgr_.get(), &OCRManager::ocrFinished,
        this, &AppController::handleOCRResult);
    connect(ocrMgr_.get(), &OCRManager::ocrProgress,
        this, &AppController::ocrProgress);

    // 자동 실행 단계/완료
    connect(autoMgr_.get(), &AutomationManager::stepCompleted,
        this, &AppController::handleAutoStep);
    connect(autoMgr_.get(), &AutomationManager::automationFinished,
        this, &AppController::handleAutoFinished);
}

AppController::~AppController() {
    qDebug() << "AppController 소멸, this =" << this;
}

void AppController::updateCurrentGrid(const std::vector<std::vector<int>>& grid) {
    qDebug() << "updateCurrentGrid 호출, grid size =" << grid.size();
    state_.currentGrid = grid;
}

void AppController::updateRemovalPath(const std::vector<RemovalStep>& path) {
    qDebug() << "updateRemovalPath 호출, path size =" << path.size();
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
    qDebug() << "requestCapture called, this =" << this;
    state_.clear();
    emit captureStarted();
    captureMgr_->startCapture();
}

void AppController::handleCapture(const CaptureResult& result) {
    qDebug() << "handleCapture called, this =" << this
        << " imageNull =" << result.image.isNull();

    if (result.image.isNull()) {
        state_.capturedPixmap = QPixmap();
        DEBUG_SIGNAL_CONNECTIONS(this, captureFailed(QString));
        emit captureFailed("유효하지 않은 영역");
        return;
    }

    state_.capturedPixmap = result.image;
    state_.captureArea = result.rect;

    DEBUG_SIGNAL_CONNECTIONS(this, captureCompleted(QPixmap));
    emit captureCompleted(result.image);

    if (strategy_) {
        qDebug() << "strategy_->handleCapture 호출";
        strategy_->handleCapture(result, this);
    }
    else {
        qDebug() << "strategy_ 미설정";
    }
}

void AppController::requestOCR() {
    qDebug() << "requestOCR called, this =" << this;
    if (state_.capturedPixmap.isNull()) {
        qDebug() << "OCR 실패: 캡처 먼저 수행";
        emit ocrFailed("캡처 먼저 수행");
        return;
    }
    if (ocrMgr_->isRunning()) {
        qDebug() << "이전 OCR 작업이 아직 진행 중입니다.";
        return;
    }
    emit ocrStarted();
    ocrMgr_->startOCR(state_.capturedPixmap.toImage());
}

void AppController::handleOCRResult(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path)
{
    qDebug() << "handleOCRResult called, this =" << this
        << " gridSize =" << grid.size()
        << " pathSize =" << path.size();

    state_.currentGrid = grid;
    state_.removalPath = path;
    state_.currentStep = 0;
    state_.currentAutoStep = 0;

    DEBUG_SIGNAL_CONNECTIONS(this, ocrCompleted(std::vector<std::vector<int>>));
    emit ocrCompleted(grid);

    if (strategy_) {
        qDebug() << "strategy_->handleOCR 호출";
        strategy_->handleOCR(grid, path, this);
    }
}

void AppController::requestAuto() {
    qDebug() << "requestAuto called, this =" << this;
    if (!strategy_) {
        qDebug() << "자동 실행 취소: strategy_ 미설정";
        return;
    }
    if (state_.removalPath.empty()) {
        qDebug() << "자동 실행 오류: path 비어있음";
        QMessageBox::warning(nullptr,
            tr("자동 실행 오류"),
            tr("먼저 OCR을 수행하여 제거할 경로를 생성하세요."));
        return;
    }

    DEBUG_SIGNAL_CONNECTIONS(this, automationStarted());
    emit automationStarted();

    autoMgr_->startAutomation(state_.removalPath, state_.currentAutoStep);
}

void AppController::handleAutoStep(int idx, RemovalStep step) {
    qDebug() << "handleAutoStep called, idx =" << idx
        << " step =" << "(" << step.x1 << "," << step.y1 << ") -> ("
        << step.x2 << "," << step.y2 << ")";

    state_.currentAutoStep = idx;
    performRemovalStep(step);
    emit autoStep(idx, step);
}

void AppController::handleAutoFinished() {
    qDebug() << "handleAutoFinished called, this =" << this;
    state_.isAutoCompleted = true;

    DEBUG_SIGNAL_CONNECTIONS(this, autoFinished());
    emit autoFinished();
}

void AppController::requestNext() {
    qDebug() << "requestNext called, this =" << this
        << " currentStep =" << state_.currentStep
        << " pathSize =" << state_.removalPath.size();

    if (state_.removalPath.empty() ||
        state_.currentStep >= state_.removalPath.size())
    {
        qDebug() << "nextFinished emit";
        emit nextFinished();
        return;
    }

    auto step = state_.removalPath[state_.currentStep++];
    performRemovalStep(step);

    qDebug() << "nextStep emit, idx =" << state_.currentStep;
    emit nextStep(state_.currentStep, step);

    if (state_.currentStep >= state_.removalPath.size()) {
        qDebug() << "nextFinished emit";
        emit nextFinished();
    }
}

void AppController::beginResetSelection() {
    qDebug() << "beginResetSelection called";
    captureMgr_->startCapture();
}

void AppController::beginStartSelection() {
    qDebug() << "beginStartSelection called";
    captureMgr_->startCapture();
}

void AppController::setMode(const QString& mode, const QVariantMap& params) {
    qDebug() << "setMode called, mode =" << mode;
    if (mode == "일반 모드")
        strategy_ = std::make_unique<NormalMode>();
    else if (mode == "점수 모드")
        strategy_ = std::make_unique<ScoreMode>();
    else
        strategy_ = std::make_unique<DataCollectionMode>();

    if (strategy_) {
        qDebug() << "strategy_->setParameters 호출";
        strategy_->setParameters(params);
    }
}

void AppController::performRemovalStep(const RemovalStep& step) {
    qDebug() << "performRemovalStep called, step ="
        << "(" << step.x1 << "," << step.y1 << ") -> ("
        << step.x2 << "," << step.y2 << ")";

    if (state_.captureArea.isNull() || state_.currentGrid.empty()) {
        qDebug() << "유효하지 않은 상태: 캡처 영역 또는 그리드 없음";
        return;
    }

    const int rows = state_.currentGrid.size();
    const int cols = state_.currentGrid[0].size();
    const int cellWidth = state_.captureArea.width() / cols;
    const int cellHeight = state_.captureArea.height() / rows;

    const int startX = state_.captureArea.x() + step.y1 * cellWidth;
    const int startY = state_.captureArea.y() + step.x1 * cellHeight;
    const int endX = state_.captureArea.x() + (step.y2 + 1) * cellWidth;
    const int endY = state_.captureArea.y() + (step.x2 + 1) * cellHeight;

    POINT originalPos;
    GetCursorPos(&originalPos);

    auto toAbsolute = [](int coord, int max) {
        return static_cast<LONG>((coord * 65535) / max);
        };

    constexpr int steps = 15;
    constexpr int delayMs = 3;

    // 마우스 이동 → 다운 → 드래그 → 업
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

bool AppController::isAutomationRunning() const {
    return autoMgr_ && autoMgr_->isRunning();
}

void AppController::onStopRequest() {
    qDebug() << "onStopRequest called";
    if (autoMgr_) {
        autoMgr_->stopAutomation();
    }
}
