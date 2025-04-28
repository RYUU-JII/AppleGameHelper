#pragma once

#include <QWidget>
#include <QPixmap>
#include <QVariantMap>
#include <vector>
#include "Types.h"
#include <QSplitter>

class AppController;
class QToolBar;
class QLabel;
class QPushButton;
class QTableWidget;
class QComboBox;
class QSpinBox;
class QLineEdit;
class QProgressDialog;

class MainTab : public QWidget {
    Q_OBJECT
public:
    explicit MainTab(QWidget* parent = nullptr);
    AppController* controller() const { return m_controller; }

private:
    void initToolBar();
    void initTable();
    void initSplitter();
    void updateUI(const QString& mode);
    void updateButtonStates();

private slots:
    void onCaptureCompleted(const QPixmap& pixmap);
    void onCaptureFailed(const QString& error);
    void onOCRStarted();
    void onOCRProgress(int percent);
    void onOCRCompleted(const std::vector<std::vector<int>>& grid);
    void onOCRFailed(const QString& error);
    void onNextStep(int idx, RemovalStep step);
    void onNextFinished();
    void onAutoStep(int idx, RemovalStep step);
    void onAutoFinished();
    void onBrowseSavePath();
    void onScoreValueChanged(int value);
    void onToggleTable(bool checked);

protected:
    void hideEvent(QHideEvent* event) override;

private:
    AppController* m_controller = nullptr;

    QToolBar* toolBar = nullptr;
    QAction* actionCapture = nullptr;
    QAction* actionOCR = nullptr;
    QAction* actionNext = nullptr;
    QAction* actionAuto = nullptr;
    QAction* actionSetReset = nullptr;
    QAction* actionSetStart = nullptr;

    QLabel* infoLabel = nullptr;
    QLabel* imageLabel = nullptr;
    QPushButton* toggleTableBtn = nullptr;
    QSplitter* splitter = nullptr;
    QTableWidget* gameTable = nullptr;
    QComboBox* modeCombo = nullptr;
    QSpinBox* scoreSpinBox = nullptr;
    QLineEdit* savePathEdit = nullptr;
    QPushButton* browsePathBtn = nullptr;
    QProgressDialog* ocrProgressDialog = nullptr;
};
