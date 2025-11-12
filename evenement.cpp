#include "evenement.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>

// Constructeur par défaut
Evenement::Evenement()
    : id_event(0), capacite(0), id_employe(0)
{
}

// Constructeur avec paramètres
Evenement::Evenement(int id, QString lieux, QDate date, QString type, QString conf, QString resp, int cap, int idEmp)
    : id_event(id), lieux(lieux), date_event(date), type_event(type),
      confidentialite(conf), responsable(resp), capacite(cap), id_employe(idEmp)
{
}

// Ajouter un événement
bool Evenement::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO EVENEMENT (LIEUX, DATE_EVENT, TYPE_EV, CONFIDENTIALITE, RESPONSABLE, CAPACITE, ID_EMPLOYE) "
                  "VALUES (:lieux, :date, :type, :conf, :resp, :cap, :idemp)");
    
    query.bindValue(":lieux", lieux);
    query.bindValue(":date", date_event);
    query.bindValue(":type", type_event);
    query.bindValue(":conf", confidentialite);
    query.bindValue(":resp", responsable);
    query.bindValue(":cap", capacite);
    
    // Si id_employe est 0, on met NULL
    if (id_employe > 0) {
        query.bindValue(":idemp", id_employe);
    } else {
        query.bindValue(":idemp", QVariant(QVariant::Int));
    }

    if (!query.exec()) {
        qDebug() << "Erreur ajout événement:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Événement ajouté avec succès";
    return true;
}

// Modifier un événement
bool Evenement::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE EVENEMENT SET LIEUX=:lieux, DATE_EVENT=:date, TYPE_EV=:type, "
                  "CONFIDENTIALITE=:conf, RESPONSABLE=:resp, CAPACITE=:cap, ID_EMPLOYE=:idemp "
                  "WHERE ID_EVENT=:id");
    
    query.bindValue(":id", id_event);
    query.bindValue(":lieux", lieux);
    query.bindValue(":date", date_event);
    query.bindValue(":type", type_event);
    query.bindValue(":conf", confidentialite);
    query.bindValue(":resp", responsable);
    query.bindValue(":cap", capacite);
    
    if (id_employe > 0) {
        query.bindValue(":idemp", id_employe);
    } else {
        query.bindValue(":idemp", QVariant(QVariant::Int));
    }

    if (!query.exec()) {
        qDebug() << "Erreur modification événement:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Événement modifié avec succès";
    return true;
}

// Supprimer un événement
bool Evenement::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM EVENEMENT WHERE ID_EVENT=:id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression événement:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Événement supprimé avec succès";
    return true;
}

// Afficher tous les événements
QSqlQueryModel* Evenement::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EVENT, LIEUX, DATE_EVENT, TYPE_EV, CONFIDENTIALITE, RESPONSABLE, CAPACITE "
                    "FROM EVENEMENT ORDER BY ID_EVENT");
    
    // Définir les en-têtes des colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Événement"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Lieux"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Confidentialité"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Responsable"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Capacité"));
    
    if (model->lastError().isValid()) {
        qDebug() << "Erreur affichage:" << model->lastError().text();
    }
    
    return model;
}

// Rechercher des événements selon un critère
QSqlQueryModel* Evenement::rechercher(const QString &critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    
    query.prepare("SELECT ID_EVENT, LIEUX, DATE_EVENT, TYPE_EV, CONFIDENTIALITE, RESPONSABLE, CAPACITE "
                  "FROM EVENEMENT "
                  "WHERE UPPER(LIEUX) LIKE UPPER(:critere) "
                  "OR UPPER(TYPE_EV) LIKE UPPER(:critere) "
                  "OR UPPER(RESPONSABLE) LIKE UPPER(:critere) "
                  "OR UPPER(CONFIDENTIALITE) LIKE UPPER(:critere) "
                  "ORDER BY ID_EVENT");
    
    query.bindValue(":critere", "%" + critere + "%");
    query.exec();
    
    model->setQuery(query);
    
    // En-têtes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Événement"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Lieux"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Confidentialité"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Responsable"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Capacité"));
    
    if (model->lastError().isValid()) {
        qDebug() << "Erreur recherche:" << model->lastError().text();
    }
    
    return model;
}

// Trier par colonne
QSqlQueryModel* Evenement::trierPar(const QString &colonne, const QString &ordre)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = QString("SELECT ID_EVENT, LIEUX, DATE_EVENT, TYPE_EV, CONFIDENTIALITE, RESPONSABLE, CAPACITE "
                               "FROM EVENEMENT ORDER BY %1 %2").arg(colonne, ordre);
    
    model->setQuery(queryStr);
    
    // En-têtes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Événement"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Lieux"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Confidentialité"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Responsable"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Capacité"));
    
    if (model->lastError().isValid()) {
        qDebug() << "Erreur tri:" << model->lastError().text();
    }
    
    return model;
}
