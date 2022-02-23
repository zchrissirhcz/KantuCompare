#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    QPalette pal = a.palette();
//    pal.setColor(QPalette::Window, Qt::white);
//    a.setPalette(pal);

    return a.exec();
}
