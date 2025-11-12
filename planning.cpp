#include "planning.h"
#include <QVariant>
#include <QDebug>

// Constructeurs
Planning::Planning(int salle, QDate date, QTime heureDebut, QTime heureFin, QString statut, QString typeActivite)
{
    this->salle = salle;
    this->date = date;
    this->heure_debut = heureDebut;
    this->heure_fin = heureFin;
    this->statut = statut;
    this->type_activite = typeActivite;
}

// Getters
int Planning::getIdPlanning() const { return id_planning; }
int Planning::getSalle() const { return salle; }
QDate Planning::getDate() const { return date; }
QTime Planning::getHeureDebut() const { return heure_debut; }
QTime Planning::getHeureFin() const { return heure_fin; }
QString Planning::getStatut() const { return statut; }
QString Planning::getTypeActivite() const { return type_activite; }

// Setters
void Planning::setIdPlanning(int id) { id_planning = id; }
void Planning::setSalle(int s) { salle = s; }
void Planning::setDate(const QDate &d) { date = d; }
void Planning::setHeureDebut(const QTime &hd) { heure_debut = hd; }
void Planning::setHeureFin(const QTime &hf) { heure_fin = hf; }
void Planning::setStatut(const QString &s) { statut = s; }
void Planning::setTypeActivite(const QString &t) { type_activite = t; }

// Ajouter
bool Planning::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO PLANNING (SALLE, \"DATE\", HEURE_DEBUT, HEURE_FIN, STATUT, TYPE_ACTIVITE) "
                  "VALUES (:salle, :date, :heure_debut, :heure_fin, :statut, :type_activite)");

    query.bindValue(":salle", salle);
    query.bindValue(":date", date);
    query.bindValue(":heure_debut", heure_debut);
    query.bindValue(":heure_fin", heure_fin);
    query.bindValue(":statut", statut);
    query.bindValue(":type_activite", type_activite);

    return query.exec();
}

// Afficher
QSqlQueryModel* Planning::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM PLANNING");
    return model;
}

// Supprimer
bool Planning::supprimer(int id_planning)
{
    QSqlQuery query;
    query.prepare("DELETE FROM PLANNING WHERE ID_PLANNING = :id");
    query.bindValue(":id", id_planning);
    return query.exec();
}


// Modifier
bool Planning::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE PLANNING SET SALLE=:salle, \"DATE\"=:date, HEURE_DEBUT=:heure_debut, "
                  "HEURE_FIN=:heure_fin, STATUT=:statut, TYPE_ACTIVITE=:type_activite "
                  "WHERE ID_PLANNING=:id");

    query.bindValue(":id", id_planning);
    query.bindValue(":salle", salle);
    query.bindValue(":date", date);
    query.bindValue(":heure_debut", heure_debut);
    query.bindValue(":heure_fin", heure_fin);
    query.bindValue(":statut", statut);
    query.bindValue(":type_activite", type_activite);

    return query.exec();
}
