#ifndef EVENEMENT_H
#define EVENEMENT_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Evenement
{
public:
    // Constructeurs
    Evenement();
    Evenement(int id, QString lieux, QDate date, QString type, QString conf, QString resp, int cap, int idEmp);

    // Getters
    int getIdEvent() const { return id_event; }
    QString getLieux() const { return lieux; }
    QDate getDateEvent() const { return date_event; }
    QString getTypeEvent() const { return type_event; }
    QString getConfidentialite() const { return confidentialite; }
    QString getResponsable() const { return responsable; }
    int getCapacite() const { return capacite; }
    int getIdEmploye() const { return id_employe; }

    // Setters
    void setIdEvent(int id) { id_event = id; }
    void setLieux(const QString &l) { lieux = l; }
    void setDateEvent(const QDate &d) { date_event = d; }
    void setTypeEvent(const QString &t) { type_event = t; }
    void setConfidentialite(const QString &c) { confidentialite = c; }
    void setResponsable(const QString &r) { responsable = r; }
    void setCapacite(int c) { capacite = c; }
    void setIdEmploye(int id) { id_employe = id; }

    // Méthodes CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &critere);
    QSqlQueryModel* trierPar(const QString &colonne, const QString &ordre = "ASC");

private:
    int id_event;
    QString lieux;
    QDate date_event;
    QString type_event;
    QString confidentialite;
    QString responsable;
    int capacite;
    int id_employe;
};

#endif // EVENEMENT_H
