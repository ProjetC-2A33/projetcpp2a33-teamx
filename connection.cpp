#include "connection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
connection::connection()
{

}
bool connection::createconnect()
{bool test=false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("projetqt");//inserer le nom de la source de données ODBC
    db.setUserName("mariem");//inserer nom de l'utilisateur
    db.setPassword("mariem");//inserer mot de passe de cet utilisateur

    if (db.open())
        test=true;

    return  test;
}
