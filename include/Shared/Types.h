#ifndef TYPES_H
#define TYPES_H

#include <windows.h>
#include <QPixmap>
#include <QRect>
#include <QPoint>
#include <QMetaType>
#include <vector>

struct RemovalStep {
    int x1;
    int y1;
    int x2;
    int y2;
};

Q_DECLARE_METATYPE(RemovalStep)
Q_DECLARE_METATYPE(std::vector<RemovalStep>)

struct CaptureResult {
    QPixmap image;
    QRect rect;
};

Q_DECLARE_METATYPE(CaptureResult)
Q_DECLARE_METATYPE(std::vector<std::vector<int>>)

#endif