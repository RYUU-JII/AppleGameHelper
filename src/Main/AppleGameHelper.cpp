#include "Main/AppleGameHelper.h"
#include "Tabs/MainTab.h"
#include "HotKeyManager.h"
#include "AppController.h"

#include <QMetaType>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QDebug>

AppleGameHelper::AppleGameHelper(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Apple Game Helper");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    initMetaTypes();

    // 중앙 위젯 생성
    mainTab = new MainTab(this);
    setCentralWidget(mainTab);

    // 핫키 등록
    initHotkeys();

    adjustSize();
    qDebug() << "AppleGameHelper initialized.";
}

AppleGameHelper::~AppleGameHelper()
{
}

void AppleGameHelper::initHotkeys()
{
    auto mgr = HotKeyManager::instance();
    mgr->registerHotKey(1, MOD_CONTROL | MOD_SHIFT, 'S');

    connect(mgr, &HotKeyManager::hotKeyPressed, this, [this](int id) {
        if (id == 1 && mainTab) {
            mainTab->controller()->onStopRequest();
        }
        });
}

void AppleGameHelper::initMetaTypes()
{
    qRegisterMetaType<RemovalStep>("RemovalStep");
    qRegisterMetaType<std::vector<RemovalStep>>("std::vector<RemovalStep>");
    qRegisterMetaType<std::vector<std::vector<int>>>("std::vector<std::vector<int>>");
    qRegisterMetaType<CaptureResult>("CaptureResult");
}
