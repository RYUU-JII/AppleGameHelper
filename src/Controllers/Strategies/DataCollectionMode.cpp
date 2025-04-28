// DataCollectionMode.cpp
#include "DataCollectionMode.h"
#include "AppController.h"
#include <QDateTime>

DataCollectionMode::DataCollectionMode(QObject* parent)
    : ModeStrategy(parent), savePath("./captures") {
}

void DataCollectionMode::handleCapture(const CaptureResult& result, AppController* controller) {
    if (result.image.isNull()) {
        emit controller->captureFailed("캡처 실패");
        return;
    }
    QString fileName = savePath + "/capture_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
    result.image.save(fileName);
    emit controller->captureCompleted(result.image);
}


void DataCollectionMode::saveGridData(const std::vector<std::vector<int>>& grid) {
	/*QString fileName = savePath + "/grid_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".txt";
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		for (const auto& row : grid) {
			for (const auto& cell : row) {
				out << cell << " ";
			}
			out << "\n";
		}
		file.close();
	}*/
}

void DataCollectionMode::handleGridUpdate(const std::vector<std::vector<int>>& grid, AppController* controller) {
    saveGridData(grid);              // OCR 결과 저장
    controller->updateCurrentGrid(grid);  // 상태 업데이트
}


void DataCollectionMode::handleOCR(const std::vector<std::vector<int>>& grid, const std::vector<RemovalStep>& path, AppController* controller) {
    // OCR 불필요
}

void DataCollectionMode::startAutomation(AppController* controller) {
    // 자동화 불필요
}

void DataCollectionMode::setParameters(const QVariantMap& params) {
    if (params.contains("savePath")) {
        savePath = params["savePath"].toString();
    }
}