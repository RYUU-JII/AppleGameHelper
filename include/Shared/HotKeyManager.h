#pragma once

#include <windows.h>
#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QMutex>

class HotKeyManager : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT
public:
    static HotKeyManager* instance();

    bool registerHotKey(int id, UINT modifier, UINT key);
    void unregisterHotKey(int id);

signals:
    void hotKeyPressed(int id);

protected:
    // Qt6 시그니처로 정확히 override
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    HotKeyManager(QObject* parent = nullptr);
    ~HotKeyManager() override;

    QMutex mutex;
};
