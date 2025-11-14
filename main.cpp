#include "mainwindow.h"
#include "connection.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    connection c;
    c.createconnect();
    w.show();
    return a.exec();
}
