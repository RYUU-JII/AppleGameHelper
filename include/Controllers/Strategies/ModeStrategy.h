#pragma once
#include <QObject>
#include <QRect>
#include <QPixmap>
#include "Types.h"

class AppController; // 전방 선언

class ModeStrategy : public QObject {
    Q_OBJECT
public:
    ModeStrategy(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ModeStrategy() = default;
    virtual void handleCapture(const CaptureResult& result, AppController* controller) = 0;
    virtual void handleOCR(const std::vector<std::vector<int>>& grid, const std::vector<RemovalStep>& path, AppController* controller) = 0;
    virtual void startAutomation(AppController* controller) = 0;
    virtual void setParameters(const QVariantMap& params) = 0;
    virtual void handleGridUpdate(const std::vector<std::vector<int>>& grid, AppController* controller) = 0;
};