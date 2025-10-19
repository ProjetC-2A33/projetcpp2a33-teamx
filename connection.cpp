#include "connection.h"
#include <QMessageBox>
#include <QSqlError>

Connection::Connection() {}

bool Connection::createconnect()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("SummerClub"); // nom du DSN ODBC
    db.setUserName("soumaya");  // login
    db.setPassword("esprit18");   // mot de passe

    if(db.open()) {
        return true;
    } else {
        return false;
    }
}
