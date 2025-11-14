#include "employer.h"
#include <QtDebug>
#include <QObject>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

employer::employer()
{
    idemp = 0;
    nom = "";
    prenom = "";
    typeposte = "";
    typecontrat = "";
    salaire = 0.0f;
}

employer::employer(int idemp, QString nom, QString prenom, QString typeposte,
                   QString typecontrat, float salaire)
{
    this->idemp = idemp;
    this->nom = nom;
    this->prenom = prenom;
    this->typeposte = typeposte;
    this->typecontrat = typecontrat;
    this->salaire = salaire;
}

// === Getters ===
int employer::getidemp() { return idemp; }
QString employer::getnom() { return nom; }
QString employer::getprenom() { return prenom; }
QString employer::gettypeposte() { return typeposte; }
QString employer::gettypecontrat() { return typecontrat; }
float employer::getsalaire() { return salaire; }

// === Setters ===
void employer::setidemp(int idemp) { this->idemp = idemp; }
void employer::setnom(QString nom) { this->nom = nom; }
void employer::setprenom(QString prenom) { this->prenom = prenom; }
void employer::settypeposte(QString typeposte) { this->typeposte = typeposte; }
void employer::settypecontrat(QString typecontrat) { this->typecontrat = typecontrat; }
void employer::setsalaire(float salaire) { this->salaire = salaire; }

// === CRUD Operations ===
bool employer::Ajouter()
{
    QSqlQuery query;
    QString salaire_string = QString::number(salaire, 'f', 2);

    query.prepare("INSERT INTO employer (idemp, nom, prenom, typeposte, typecontrat, salaire) "
                  "VALUES (:idemp, :nom, :prenom, :typeposte, :typecontrat, :salaire)");
    query.bindValue(":idemp", idemp);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":typeposte", typeposte);
    query.bindValue(":typecontrat", typecontrat);
    query.bindValue(":salaire", salaire_string);

    if (!query.exec()) {
        qDebug() << "Erreur Ajout employer:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* employer::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM employer");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Poste"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type Contrat"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Salaire"));

    return model;
}

bool employer::Supprime(int idemp)
{
    QSqlQuery query;
    query.prepare("DELETE FROM employer WHERE idemp = :idemp");
    query.bindValue(":idemp", idemp);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(nullptr, QObject::tr("Succès !"),
                                     QObject::tr("Employé supprimé avec succès."), QMessageBox::Ok);
            return true;
        } else {
            QMessageBox::critical(nullptr, QObject::tr("Erreur !"),
                                  QObject::tr("Aucun employé trouvé avec cet ID.\n"
                                              "Suppression échouée."), QMessageBox::Cancel);
            return false;
        }
    } else {
        qDebug() << "Erreur suppression:" << query.lastError().text();
        return false;
    }
}

bool employer::modifier()
{
    QSqlQuery query;
    QString salaire_string = QString::number(salaire, 'f', 2);

    query.prepare("UPDATE employer SET nom=:nom, prenom=:prenom, typeposte=:typeposte, "
                  "typecontrat=:typecontrat, salaire=:salaire WHERE idemp=:idemp");
    query.bindValue(":idemp", idemp);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":typeposte", typeposte);
    query.bindValue(":typecontrat", typecontrat);
    query.bindValue(":salaire", salaire_string);

    if (!query.exec()) {
        qDebug() << "Erreur modification:" << query.lastError().text();
        return false;
    }
    return true;
}
