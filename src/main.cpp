#include <QtWidgets>
#include "Main/AppleGameHelper.h"
#include <QtWidgets/QApplication>
#include <opencv2/opencv.hpp>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);

    app.setStyle("Fusion");
    //QFile file("themes/AppleWoodStyle.qss");
    //if (file.open(QFile::ReadOnly)) {
    //    QString styleSheet = QLatin1String(file.readAll());
    //    app.setStyleSheet(styleSheet);
    //}
    //else {
    //    qDebug() << "fail to load stylesheet!";
    //}

    AppleGameHelper mainWindow;

    mainWindow.move(100, 200); // (왼쪽에서, 위에서)

    mainWindow.show();

    return app.exec();
}
