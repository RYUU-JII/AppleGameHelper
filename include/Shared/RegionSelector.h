#pragma once
#include <QWidget>
#include <QRect>

class RegionSelector : public QWidget {
    Q_OBJECT
public:
    explicit RegionSelector(QWidget* parent = nullptr);
    void startSelection();

signals:
    void regionSelected(const QRect& rect);
    void selectionCanceled();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    bool selecting_ = false;
    QPoint origin_;
    QRect selectionRect_;
};
