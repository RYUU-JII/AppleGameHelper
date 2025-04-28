#pragma once

#include <windows.h>
#include <QObject>
#include <vector>
#include <QImage>
#include "CancellationToken.h"
#include "Types.h"

class OCRWorker : public QObject {
    Q_OBJECT
public:
    explicit OCRWorker(const QImage& image, QObject* parent = nullptr);
    ~OCRWorker() override;

    void run();

signals:
    void progress(int percent);
    void finished(const std::vector<std::vector<int>>& result, const std::vector<RemovalStep>& removalPath);

private:
    QImage m_image;
};
