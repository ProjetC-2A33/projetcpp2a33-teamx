#include "authpage.h"
#include "connection.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    authpage ap;
    connection c;
    c.createconnect();
    ap.exec();
    return a.exec();
}
