#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

class Connection
{
public:
    Connection();
    bool createconnect();

    QSqlDatabase db; // garder la base accessible
};

#endif // CONNECTION_H
