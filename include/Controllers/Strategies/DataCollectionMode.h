// DataCollectionMode.h
#pragma once
#include <QObject>
#include "ModeStrategy.h"
#include "Types.h"
#include "AppController.h"

class DataCollectionMode : public ModeStrategy {
	Q_OBJECT
public:
    DataCollectionMode(QObject* parent = nullptr);
    void handleCapture(const CaptureResult& result, AppController* controller) override;
    void handleOCR(const std::vector<std::vector<int>>& grid, const std::vector<RemovalStep>& path, AppController* controller) override;
    void startAutomation(AppController* controller) override;
    void setParameters(const QVariantMap& params) override;
    void handleGridUpdate(const std::vector<std::vector<int>>& grid, AppController* controller) override;
private:
    QString savePath;
    void saveGridData(const std::vector<std::vector<int>>& grid);
};