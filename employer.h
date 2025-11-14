#ifndef EMPLOYER_H
#define EMPLOYER_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class employer
{
private:
    int idemp;
    QString nom, prenom, typeposte, typecontrat;
    float salaire;

public:
    // Constructors
    employer();
    employer(int idemp, QString nom, QString prenom, QString typeposte,
             QString typecontrat, float salaire);

    // CRUD Operations
    bool Ajouter();
    QSqlQueryModel* afficher();
    bool Supprime(int idemp);
    bool modifier();

    // Getters
    int getidemp();
    QString getnom();
    QString getprenom();
    QString gettypeposte();
    QString gettypecontrat();
    float getsalaire();

    // Setters
    void setidemp(int idemp);
    void setnom(QString nom);
    void setprenom(QString prenom);
    void settypeposte(QString typeposte);
    void settypecontrat(QString typecontrat);
    void setsalaire(float salaire);
};

#endif // EMPLOYER_H
