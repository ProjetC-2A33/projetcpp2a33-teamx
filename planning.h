#ifndef PLANNING_H
#define PLANNING_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Planning
{
private:
    int id_planning;
    int salle;
    QDate date;
    QTime heure_debut;
    QTime heure_fin;
    QString statut;
    QString type_activite;

public:
    // Constructeurs
    Planning() {}
    Planning(int salle, QDate date, QTime heureDebut, QTime heureFin, QString statut, QString typeActivite);

    // Getters
    int getIdPlanning() const;
    int getSalle() const;
    QDate getDate() const;
    QTime getHeureDebut() const;
    QTime getHeureFin() const;
    QString getStatut() const;
    QString getTypeActivite() const;

    // Setters
    void setIdPlanning(int id);
    void setSalle(int s);
    void setDate(const QDate &d);
    void setHeureDebut(const QTime &hd);
    void setHeureFin(const QTime &hf);
    void setStatut(const QString &s);
    void setTypeActivite(const QString &t);

    // Méthodes CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id_planning);
    bool modifier();
};

#endif // PLANNING_H
