#pragma once

#include <QMainWindow>
#include "StopControllable.h"
#include "ImageProcessor.h"

class MainTab;  // 새로운 중앙 위젯
class HotKeyManager;

class AppleGameHelper : public QMainWindow
{
    Q_OBJECT
public:
    explicit AppleGameHelper(QWidget* parent = nullptr);
    ~AppleGameHelper();

private:
    void initHotkeys();
    void initMetaTypes();

    MainTab* mainTab;  // 중앙 위젯
};
