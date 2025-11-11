#include "connection.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("SummerClub");
    db.setUserName("soumaya");
    db.setPassword("esprit18");
    
    return db.open();
}
