// HotKeyManager.cpp

#include "HotKeyManager.h"
#include <QCoreApplication>
#include <QDebug>

#ifdef Q_OS_WIN
#  include <windows.h>
#endif

HotKeyManager* HotKeyManager::instance() {
    static HotKeyManager inst;
    return &inst;
}

HotKeyManager::HotKeyManager(QObject* parent)
    : QObject(parent) {
    // 이제 타입이 맞으므로 this를 전달할 수 있습니다
    QCoreApplication::instance()->installNativeEventFilter(this);
}

HotKeyManager::~HotKeyManager() {
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

bool HotKeyManager::registerHotKey(int id, UINT mod, UINT vk) {
#ifdef Q_OS_WIN
    QMutexLocker locker(&mutex);
    if (!RegisterHotKey(nullptr, id, mod, vk)) {
        qWarning() << "Failed to register hotkey:" << GetLastError();
        return false;
    }
    return true;
#else
    return false;
#endif
}

void HotKeyManager::unregisterHotKey(int id) {
#ifdef Q_OS_WIN
    QMutexLocker locker(&mutex);
    UnregisterHotKey(nullptr, id);
#endif
}

bool HotKeyManager::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) {
#ifdef Q_OS_WIN
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        emit hotKeyPressed(int(msg->wParam));
        if (result) *result = 0;
        return true;
    }
#endif
    return false;
}
