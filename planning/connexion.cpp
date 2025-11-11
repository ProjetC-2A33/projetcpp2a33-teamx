#include "connexion.h"
#include <QMessageBox>
#include <QSqlError>

Connection::Connection() {}

bool Connection::createconnect()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("projet"); // nom du DSN ODBC
    db.setUserName("seifeddine");  // login
    db.setPassword("esprit123456789");   // mot de passe

    if(db.open()) {
        return true;
    } else {
        return false;
    }
}
