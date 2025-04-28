// NormalMode.cpp
#include "NormalMode.h"
#include "AppController.h"
#include "ImageProcessor.h"
#include <QDebug>


NormalMode::NormalMode(QObject* parent)
    : ModeStrategy(parent) {
}

void NormalMode::handleCapture(const CaptureResult& result, AppController* controller) {
   qDebug() << "NormalMode::handleCapture called, this:" << this << "controller:" << controller;
   if (result.image.isNull()) {
       qDebug() << "Emitting captureFailed: 캡처 실패, controller connections:"
                << controller->metaObject()->method(controller->metaObject()->indexOfSignal("captureFailed(QString)")).methodIndex();
       emit controller->captureFailed("캡처 실패");
       return;
   }

   ImageProcessor processor;
   tesseract::TessBaseAPI api;
   if (api.Init("./Resources/tessdata", "eng") != 0) {
       qDebug() << "Emitting captureFailed: Tesseract 초기화 실패, controller connections:"
                << controller->metaObject()->method(controller->metaObject()->indexOfSignal("captureFailed(QString)")).methodIndex();
       emit controller->captureFailed("Tesseract 초기화 실패");
       return;
   }

   cv::Mat mat = processor.qImageToMat(result.image.toImage());
   if (!processor.quickValidateOCR(mat, api)) {
       qDebug() << "Emitting captureFailed: OCR에 적합하지 않은 영역, controller connections:"
                << controller->metaObject()->method(controller->metaObject()->indexOfSignal("captureFailed(QString)")).methodIndex();
       emit controller->captureFailed("OCR에 적합하지 않은 영역입니다.");
       return;
   }

   qDebug() << "[NormalMode] OCR validation passed";
}

void NormalMode::handleGridUpdate(const std::vector<std::vector<int>>& grid, AppController* controller) {
    controller->updateCurrentGrid(grid);
}

void NormalMode::handleOCR(const std::vector<std::vector<int>>& grid,
    const std::vector<RemovalStep>& path,
    AppController* controller) {
    controller->updateCurrentGrid(grid);
    controller->updateRemovalPath(path);
    emit controller->ocrCompleted(grid);
}

void NormalMode::startAutomation(AppController* controller) {
    if (controller->getRemovalPath().empty()) {
        emit controller->autoFinished();
        return;
    }
    controller->requestAuto();
}
