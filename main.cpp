#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerThread serverThread;
    serverThread.start();
    MainWindow w;
    w.show();
    return a.exec();
}
