#ifndef EMPLOYER_H
#define EMPLOYER_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>

class employer
{
private:
    int idemp;
    QString nom, prenom, typeposte, typecontrat, mdp, indice_mdp;
    QString lastErrorMessage;
    float salaire;

public:
    // Constructors
    employer();
    employer(int idemp, QString nom, QString prenom, QString typeposte,
             QString typecontrat, float salaire, QString mdp, QString indice_mdp);

    // CRUD Operations
    bool Ajouter();
    QSqlQueryModel* afficher();
    QSqlQueryModel* trierParSalaire(bool asc = true);
    QSqlQueryModel* trierParNom(bool asc = true);
    QSqlQueryModel* trierParTypeContrat(bool asc = true);
    QSqlQueryModel* chercherParId(int idemp);
    QSqlQueryModel* chercherParLettreOuNumero(const QString &searchTerm);
    QMap<QString, int> getStatistiquesParTypeContrat();
    bool Supprime(int idemp);
    bool modifier();

    // Getters
    int getidemp();
    QString getnom();
    QString getprenom();
    QString gettypeposte();
    QString gettypecontrat();
    QString getmdp();
    QString getindice_mdp();
    float getsalaire();

    // Setters
    void setidemp(int idemp);
    void setnom(QString nom);
    void setprenom(QString prenom);
    void settypeposte(QString typeposte);
    void settypecontrat(QString typecontrat);
    void setmdp(QString mdp);
    void setindice_mdp(QString indice_mdp);
    void setsalaire(float salaire);
    QString lastError() const;

    bool login(int id, QString mdp);

    QPair<QString, QString> getNameById(const QString &id);
};

#endif // EMPLOYER_H
