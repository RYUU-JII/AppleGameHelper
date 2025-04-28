#include "OCRWorker.h"
#include "ImageProcessor.h"
#include <tesseract/baseapi.h>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <QThread>
#include "GridRemover.h"

OCRWorker::OCRWorker(const QImage& image, QObject* parent)
    : QObject(parent), m_image(image) {
    qDebug() << "OCRWorker::OCRWorker 초기화, this:" << this << "image size:" << image.size();
}

OCRWorker::~OCRWorker() {}

void OCRWorker::run() {
    try {
        qDebug() << "OCRWorker::run 시작, this:" << this;

        if (m_image.isNull()) {
            qDebug() << "OCRWorker::run - 이미지 없음, 취소";
            emit finished({}, {});
            return;
        }

        cv::Mat inputImage(m_image.height(), m_image.width(), CV_8UC4,
            const_cast<uchar*>(m_image.bits()), m_image.bytesPerLine());

        tesseract::TessBaseAPI api;
        if (api.Init("./Resources/tessdata", "eng") != 0) {
            qDebug() << "[OCRWorker] Tesseract init 실패";
            emit finished({}, {});
            return;
        }

        ImageProcessor processor;

        qDebug() << "OCRWorker::run - performOCR 호출";
        auto numberGrid = processor.performOCR(
            inputImage,
            api,
            [&](int percent) {
                qDebug() << "OCRWorker::run - 진행률" << percent;
                emit progress(percent);
            }
        );
        qDebug() << "OCRWorker::run - performOCR 종료, grid size:" << numberGrid.size();

        api.End();

        GridRemover gridRemover;
        gridRemover.setGrid(numberGrid);
        gridRemover.performHeuristicRemovals();

        qDebug() << "OCRWorker::run - finished emit, path size:" << gridRemover.getRemovalPath().size();
        emit finished(numberGrid, gridRemover.getRemovalPath());
    }
    catch (const std::exception& e) {
        qCritical() << "OCRWorker 오류:" << e.what();
        emit finished({}, {});
    }
}