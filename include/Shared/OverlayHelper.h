// OverlayHelper.h
#pragma once

#include <QObject>
#include <QLabel>
#include <QString>

class OverlayHelper : public QObject {
    Q_OBJECT
public:
    explicit OverlayHelper(QWidget* parent);
    void showOverlay(const QString& msg);
    void hideOverlay();

private:
    QLabel* overlayLabel = nullptr;
};
