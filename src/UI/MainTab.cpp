#include "MainTab.h"
#include "Controllers/AppController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QSplitter>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QTimer>
#include <QHeaderView>
#include <QThread>
#include <QApplication>

MainTab::MainTab(QWidget* parent)
    : QWidget(parent) {

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    m_controller = new AppController(this);

    modeCombo = new QComboBox(this);
    modeCombo->addItems({ "일반 모드", "점수 모드", "데이터 수집 모드" });
    connect(modeCombo, &QComboBox::currentTextChanged, this, &MainTab::updateUI);
    mainLayout->addWidget(modeCombo);

    initToolBar();
    mainLayout->addWidget(toolBar);

    infoLabel = new QLabel(tr("영역 설정을 완료해 주세요"), this);
    mainLayout->addWidget(infoLabel);

    imageLabel = new QLabel(this);
    imageLabel->setPixmap(QPixmap(":/images/init_image.png"));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(true);
    imageLabel->setFixedSize(300, 200);
    mainLayout->addWidget(imageLabel);

    toggleTableBtn = new QPushButton(tr("Show Table"), this);
    toggleTableBtn->setCheckable(true);

    // 기존 람다 연결 대신 슬롯 함수로 연결
    connect(toggleTableBtn, &QPushButton::toggled, this, &MainTab::onToggleTable);

    mainLayout->addWidget(toggleTableBtn);

    initSplitter();
    mainLayout->addWidget(splitter);

    scoreSpinBox = new QSpinBox(this);
    scoreSpinBox->setRange(0, 170);
    scoreSpinBox->setValue(100);
    scoreSpinBox->setVisible(false);
    connect(scoreSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MainTab::onScoreValueChanged);
    mainLayout->addWidget(scoreSpinBox);

    auto* savePathLayout = new QHBoxLayout;
    savePathEdit = new QLineEdit("./captures", this);
    browsePathBtn = new QPushButton("Browse", this);
    connect(browsePathBtn, &QPushButton::clicked, this, &MainTab::onBrowseSavePath);
    savePathLayout->addWidget(savePathEdit);
    savePathLayout->addWidget(browsePathBtn);
    savePathEdit->setVisible(false);
    browsePathBtn->setVisible(false);
    mainLayout->addLayout(savePathLayout);

    qDebug() << "Connecting AppController signals, this:" << this << "m_controller:" << m_controller;
    connect(m_controller, &AppController::captureCompleted, this, &MainTab::onCaptureCompleted, Qt::QueuedConnection);
    connect(m_controller, &AppController::captureFailed, this, &MainTab::onCaptureFailed, Qt::QueuedConnection);
    connect(m_controller, &AppController::ocrStarted, this, &MainTab::onOCRStarted);
    connect(m_controller, &AppController::ocrProgress, this, &MainTab::onOCRProgress);
    connect(m_controller, &AppController::ocrCompleted, this, &MainTab::onOCRCompleted);
    connect(m_controller, &AppController::ocrFailed, this, &MainTab::onOCRFailed);
    connect(m_controller, &AppController::nextStep, this, &MainTab::onNextStep);
    connect(m_controller, &AppController::nextFinished, this, &MainTab::onNextFinished);
    connect(m_controller, &AppController::autoStep, this, &MainTab::onAutoStep);
    connect(m_controller, &AppController::autoFinished, this, &MainTab::onAutoFinished);

    updateUI(modeCombo->currentText());
}

void MainTab::initToolBar() {
    toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(32, 32));

    actionCapture = new QAction(QIcon(":/icons/crop.svg"), tr("Capture"), this);
    actionOCR = new QAction(QIcon(":/icons/document_scanner.svg"), tr("OCR"), this);
    actionNext = new QAction(QIcon(":/icons/next.svg"), tr("Next"), this);
    actionAuto = new QAction(QIcon(":/icons/auto.svg"), tr("Auto"), this);
    actionSetReset = new QAction(QIcon(":/icons/reset.svg"), tr("Set Reset"), this);
    actionSetStart = new QAction(QIcon(":/icons/play.svg"), tr("Set Start"), this);

    connect(actionCapture, &QAction::triggered, m_controller, &AppController::requestCapture);
    connect(actionOCR, &QAction::triggered, m_controller, &AppController::requestOCR);
    connect(actionNext, &QAction::triggered, m_controller, &AppController::requestNext);
    connect(actionAuto, &QAction::triggered, m_controller, &AppController::requestAuto);
    connect(actionSetReset, &QAction::triggered, m_controller, &AppController::beginResetSelection);
    connect(actionSetStart, &QAction::triggered, m_controller, &AppController::beginStartSelection);

    toolBar->addAction(actionCapture);
    toolBar->addAction(actionOCR);
    toolBar->addAction(actionNext);
    toolBar->addAction(actionAuto);
    toolBar->addAction(actionSetReset);
    toolBar->addAction(actionSetStart);
}

void MainTab::initSplitter() {
    splitter = new QSplitter(Qt::Vertical, this);
    QWidget* dummy = new QWidget(this);
    splitter->addWidget(dummy);
    initTable();
    splitter->addWidget(gameTable);
    splitter->setChildrenCollapsible(true);
    gameTable->setVisible(false);
}

void MainTab::initTable() {
    constexpr int rows = 10, cols = 17, cellSize = 30;
    gameTable = new QTableWidget(rows, cols, this);
    gameTable->setMinimumSize(cols * cellSize + 2, rows * cellSize + 2);
    gameTable->horizontalHeader()->setVisible(false);
    gameTable->verticalHeader()->setVisible(false);
    gameTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    gameTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for (int r = 0; r < rows; ++r) {
        gameTable->setRowHeight(r, cellSize);
        for (int c = 0; c < cols; ++c) {
            gameTable->setColumnWidth(c, cellSize);
            gameTable->setItem(r, c, new QTableWidgetItem);
        }
    }
}

void MainTab::onToggleTable(bool checked)
{
    if (!gameTable || !imageLabel || !splitter) return;

    gameTable->setVisible(checked);
    toggleTableBtn->setText(checked ? tr("Hide Table") : tr("Show Table"));

    // 이미지 크기 조정
    if (checked) {
        imageLabel->setFixedSize(gameTable->width(), gameTable->height());
    }
    else {
        imageLabel->setFixedSize(300, 200);
    }

    // 레이아웃 및 윈도우 크기 갱신
    splitter->updateGeometry();
    layout()->invalidate();

    if (auto* tabWidget = parentWidget()) {
        tabWidget->updateGeometry();
        tabWidget->adjustSize();
    }
    if (auto* mainWindow = qobject_cast<QMainWindow*>(window())) {
        if (auto* central = mainWindow->centralWidget()) {
            central->layout()->invalidate();
            central->updateGeometry();
        }
        QTimer::singleShot(0, this, [mainWindow]() { mainWindow->adjustSize(); });
    }
}


void MainTab::updateUI(const QString& mode) {
    QVariantMap params;
    params["minScore"] = scoreSpinBox->value();
    params["savePath"] = savePathEdit->text();
    m_controller->setMode(mode, params);

    bool isNormal = (mode == "일반 모드");
    bool isScore = (mode == "점수 모드");
    bool isData = (mode == "데이터 수집 모드");

    actionOCR->setVisible(isNormal);
    actionNext->setVisible(isNormal);
    actionAuto->setVisible(isNormal || isScore);
    actionSetReset->setVisible(isScore);
    actionSetStart->setVisible(isScore);
    scoreSpinBox->setVisible(isScore);
    savePathEdit->setVisible(isData);
    browsePathBtn->setVisible(isData);
    toggleTableBtn->setVisible(isNormal || isScore);

    updateButtonStates();
}

void MainTab::updateButtonStates() {
    qDebug() << "MainTab::updateButtonStates called, this:" << this << "mode:" << modeCombo->currentText();
    QString mode = modeCombo->currentText();
    if (mode == "점수 모드") {
        if (m_controller->isResetButtonSet() && m_controller->isStartButtonSet()) {
            actionAuto->setEnabled(true);
            infoLabel->setText(tr("준비 완료! Auto를 누르세요"));
        }
        else {
            actionAuto->setEnabled(false);
            infoLabel->setText(tr("시작 버튼과 리셋 버튼을 설정해 주세요"));
        }
    }
    else {
        bool hasOCR = m_controller->hasValidOCRResult();
        actionAuto->setEnabled(hasOCR);
        actionNext->setEnabled(hasOCR);

        if (m_controller->hasValidCapture()) {
            infoLabel->setText(tr("캡처 완료. OCR 후 Auto를 진행하세요"));
        }
        else {
            infoLabel->setText(tr("영역 설정을 완료해 주세요"));
        }
    }
}

void MainTab::onCaptureCompleted(const QPixmap& pixmap) {
    qDebug() << "MainTab::onCaptureCompleted triggered, this:" << this;
    imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio));
    QApplication::processEvents(); // 강제 갱신
    updateButtonStates();
}

void MainTab::onCaptureFailed(const QString& error) {
    qDebug() << "MainTab::onCaptureFailed triggered, this:" << this << "error:" << error;
    QMessageBox::warning(this, tr("캡처 오류"), error);
    imageLabel->setPixmap(QPixmap(":/images/init_image.png"));
    qDebug() << "Calling updateButtonStates";
    updateButtonStates();
}

void MainTab::onOCRStarted() {
    if (ocrProgressDialog) {
        ocrProgressDialog->deleteLater();
        ocrProgressDialog = nullptr;
    }

    // 새 다이얼로그 생성
    ocrProgressDialog = new QProgressDialog(
        tr("OCR 진행 중입니다..."),
        QString(),
        0,
        100,
        this
    );
    ocrProgressDialog->setWindowModality(Qt::WindowModal);
    ocrProgressDialog->setAutoClose(false);  // 완료 후 자동 닫힘 방지
    //ocrProgressDialog->setAutoReset(false);  // 자동 리셋 방지
    ocrProgressDialog->setMinimumDuration(0);  // 즉시 표시
    ocrProgressDialog->show();

    // 취소 버튼 비활성화
    ocrProgressDialog->setCancelButton(nullptr);
}


void MainTab::onOCRProgress(int percent) {
    if (ocrProgressDialog && percent >= 0 && percent <= 100) {
        ocrProgressDialog->setValue(percent);
    }
}

void MainTab::onOCRCompleted(const std::vector<std::vector<int>>& grid) {
    if (ocrProgressDialog) {
        ocrProgressDialog->close();  // 수동으로 닫기
        ocrProgressDialog->deleteLater();
        ocrProgressDialog = nullptr;
    }

    const int rows = grid.size();
    const int cols = grid.empty() ? 0 : grid[0].size();

    if (rows != gameTable->rowCount() || cols != gameTable->columnCount()) {
        gameTable->setRowCount(rows);
        gameTable->setColumnCount(cols);
    }

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (auto* item = gameTable->item(r, c)) {
                item->setText(QString::number(grid[r][c]));
            }
            else {
                gameTable->setItem(r, c, new QTableWidgetItem(QString::number(grid[r][c])));
            }
        }
    }

    updateButtonStates();
}

void MainTab::onOCRFailed(const QString& error) {
    if (ocrProgressDialog) {
        ocrProgressDialog->hide();
    }
    QMessageBox::warning(this, tr("OCR 실패"), error);
}

void MainTab::onNextStep(int idx, RemovalStep step) {
    // (필요시 추가 기능)
}

void MainTab::onNextFinished() {
    actionNext->setEnabled(false);
}

void MainTab::onAutoStep(int idx, RemovalStep step) {
    // (필요시 추가 기능)
}

void MainTab::onAutoFinished() {
    QMessageBox::information(this, tr("자동 실행 완료"), tr("새 게임을 시작하려면 캡처를 수행하세요."));
    actionAuto->setEnabled(false);
    actionNext->setEnabled(false);
}

void MainTab::onBrowseSavePath() {
    QString path = QFileDialog::getExistingDirectory(this, "Save Directory", savePathEdit->text());
    if (!path.isEmpty()) {
        savePathEdit->setText(path);
        QVariantMap params;
        params["savePath"] = path;
        m_controller->setMode(modeCombo->currentText(), params);
    }
}

void MainTab::onScoreValueChanged(int value) {
    QVariantMap params;
    params["minScore"] = value;
    m_controller->setMode(modeCombo->currentText(), params);
}