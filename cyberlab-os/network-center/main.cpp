#include <QApplication>
#include "network_ui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("CyberLab Network Center");
    app.setApplicationVersion("1.0.0");

    NetworkCenterWindow window;
    window.show();
    return app.exec();
}
