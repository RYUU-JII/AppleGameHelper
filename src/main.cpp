#include <QtWidgets>
#include "Main/AppleGameHelper.h"
#include <QtWidgets/QApplication>
#include <opencv2/opencv.hpp>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);

    app.setStyle("Fusion");
    //QFile file("themes/AppleWoodStyle.qss");  // 또는 "../MaterialDark.qss" 등 상대 경로 조정
    //if (file.open(QFile::ReadOnly)) {
    //    QString styleSheet = QLatin1String(file.readAll());
    //    app.setStyleSheet(styleSheet);  // 애플리케이션 전역에 스타일 적용
    //}
    //else {
    //    qDebug() << "fail to load stylesheet!";
    //}

    AppleGameHelper mainWindow;
    mainWindow.show();  // 메인 윈도우를 표시합니다.

    return app.exec();  // 이벤트 루프 실행
}
