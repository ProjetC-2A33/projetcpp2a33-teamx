
#include "enfant.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

bool Enfant::ajouter() const
{
    QSqlQuery query;
    query.prepare("INSERT INTO genfant (id_enfant, nom_prenom, age, allergie_remarque, "
                  "nom_prenom_parents, num, email) "
                  "VALUES (:id, :nom, :age, :allergie, :parent, :num, :email)");
    query.bindValue(":id", id_enfant);
    query.bindValue(":nom", nom_prenom);
    query.bindValue(":age", age_val);
    query.bindValue(":allergie", allergie_remarque);
    query.bindValue(":parent", nom_prenom_parents);
    query.bindValue(":num", num_val);
    query.bindValue(":email", email_val);
    if (!query.exec()) {
        qDebug() << "Erreur INSERT:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Enfant::supprimer(int id) const
{
    QSqlQuery query;
    query.prepare("DELETE FROM genfant WHERE id_enfant = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "Erreur DELETE:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* Enfant::afficher() const
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT id_enfant, nom_prenom, age, allergie_remarque, "
                    "nom_prenom_parents, num, email FROM genfant");
    return model;
}

bool Enfant::modifier(int id) const
{
    QSqlQuery query;
    query.prepare("UPDATE genfant SET nom_prenom = :nom, age = :age, allergie_remarque = :allergie, "
                  "nom_prenom_parents = :parent, num = :num, email = :email WHERE id_enfant = :id");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom_prenom);
    query.bindValue(":age", age_val);
    query.bindValue(":allergie", allergie_remarque);
    query.bindValue(":parent", nom_prenom_parents);
    query.bindValue(":num", num_val);
    query.bindValue(":email", email_val);
    if (!query.exec()) {
        qDebug() << "Erreur UPDATE:" << query.lastError().text();
        return false;
    }
    return true;
}
