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
    mdp = "";
    indice_mdp = "";
}

employer::employer(int idemp, QString nom, QString prenom, QString typeposte,
                   QString typecontrat, float salaire, QString mdp, QString indice_mdp)
{
    this->idemp = idemp;
    this->nom = nom;
    this->prenom = prenom;
    this->typeposte = typeposte;
    this->typecontrat = typecontrat;
    this->salaire = salaire;
    this->mdp = mdp;
    this->indice_mdp = indice_mdp;

}

// === Getters ===
int employer::getidemp() { return idemp; }
QString employer::getnom() { return nom; }
QString employer::getprenom() { return prenom; }
QString employer::gettypeposte() { return typeposte; }
QString employer::gettypecontrat() { return typecontrat; }
float employer::getsalaire() { return salaire; }
QString employer::getmdp() { return mdp; }
QString employer::getindice_mdp() { return indice_mdp; }


// === Setters ===
void employer::setidemp(int idemp) { this->idemp = idemp; }
void employer::setnom(QString nom) { this->nom = nom; }
void employer::setprenom(QString prenom) { this->prenom = prenom; }
void employer::settypeposte(QString typeposte) { this->typeposte = typeposte; }
void employer::settypecontrat(QString typecontrat) { this->typecontrat = typecontrat; }
void employer::setsalaire(float salaire) { this->salaire = salaire; }
void employer::setmdp(QString mdp) { this->mdp = mdp; }
void employer::setindice_mdp(QString indice_mdp) { this->indice_mdp = indice_mdp; }

// === CRUD Operations ===
bool employer::Ajouter()
{
    QSqlQuery query;
    QString salaire_string = QString::number(salaire, 'f', 2);
    lastErrorMessage.clear();

    query.prepare("INSERT INTO employer (idemp, nom, prenom, typeposte, typecontrat, salaire, mdp, indice_mdp) "
                  "VALUES (:idemp, :nom, :prenom, :typeposte, :typecontrat, :salaire, :mdp, :indice_mdp)");
    query.bindValue(":idemp", idemp);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":typeposte", typeposte);
    query.bindValue(":typecontrat", typecontrat);
    query.bindValue(":salaire", salaire_string);
    query.bindValue(":mdp", mdp);
    query.bindValue(":indice_mdp", indice_mdp);

    if (!query.exec()) {
        lastErrorMessage = query.lastError().text();
        qDebug() << "Erreur Ajout employer:" << lastErrorMessage;
        return false;
    }
    lastErrorMessage.clear();
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
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Indice MDP"));

    return model;
}

QSqlQueryModel* employer::trierParSalaire(bool asc)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(QString("SELECT * FROM employer ORDER BY salaire %1")
                        .arg(asc ? "ASC" : "DESC"));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Poste"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type Contrat"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Salaire"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Indice MDP"));

    return model;
}

QSqlQueryModel* employer::trierParNom(bool asc)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(QString("SELECT * FROM employer ORDER BY nom %1")
                        .arg(asc ? "ASC" : "DESC"));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Poste"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type Contrat"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Salaire"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Indice MDP"));

    return model;
}

QSqlQueryModel* employer::trierParTypeContrat(bool asc)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(QString("SELECT * FROM employer ORDER BY typecontrat %1")
                        .arg(asc ? "ASC" : "DESC"));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Poste"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type Contrat"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Salaire"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Indice MDP"));

    return model;
}

QSqlQueryModel* employer::chercherParId(int idemp)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(QString("SELECT * FROM employer WHERE idemp = %1").arg(idemp));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Poste"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type Contrat"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Salaire"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Indice MDP"));

    return model;
}

QSqlQueryModel* employer::chercherParLettreOuNumero(const QString &searchTerm)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    // Échapper les caractères spéciaux SQL pour LIKE (% et _)
    QString escapedTerm = searchTerm;
    escapedTerm.replace("'", "''");  // Échapper les apostrophes
    escapedTerm.replace("%", "\\%");  // Échapper les % sauf ceux qu'on ajoute
    escapedTerm.replace("_", "\\_");  // Échapper les _
    QString searchPattern = "%" + escapedTerm + "%";
    
    // Vérifier si c'est un nombre
    bool isNumber = false;
    int idValue = searchTerm.toInt(&isNumber);
    
    QString queryStr;
    if (isNumber && idValue > 0) {
        // Si c'est un nombre, chercher dans l'ID et aussi dans les champs texte
        queryStr = QString("SELECT * FROM employer WHERE "
                          "idemp = %1 OR "
                          "nom LIKE '%2' ESCAPE '\\' OR "
                          "prenom LIKE '%2' ESCAPE '\\' OR "
                          "typeposte LIKE '%2' ESCAPE '\\' OR "
                          "typecontrat LIKE '%2' ESCAPE '\\'")
                   .arg(idValue)
                   .arg(searchPattern);
    } else {
        // Si c'est du texte, chercher dans les champs texte uniquement
        queryStr = QString("SELECT * FROM employer WHERE "
                          "nom LIKE '%1' ESCAPE '\\' OR "
                          "prenom LIKE '%1' ESCAPE '\\' OR "
                          "typeposte LIKE '%1' ESCAPE '\\' OR "
                          "typecontrat LIKE '%1' ESCAPE '\\'")
                   .arg(searchPattern);
    }
    
    model->setQuery(queryStr);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Poste"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type Contrat"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Salaire"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("MDP"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Indice MDP"));

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
    lastErrorMessage.clear();

    query.prepare("UPDATE employer SET nom=:nom, prenom=:prenom, typeposte=:typeposte, "
                  "typecontrat=:typecontrat, salaire=:salaire, mdp=:mdp, indice_mdp=:indice_mdp WHERE idemp=:idemp");
    query.bindValue(":idemp", idemp);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":typeposte", typeposte);
    query.bindValue(":typecontrat", typecontrat);
    query.bindValue(":salaire", salaire_string);
    query.bindValue(":mdp", mdp);
    query.bindValue(":indice_mdp", indice_mdp);

    if (!query.exec()) {
        lastErrorMessage = query.lastError().text();
        qDebug() << "Erreur modification:" << lastErrorMessage;
        return false;
    }
    lastErrorMessage.clear();
    return true;
}

bool employer::login(int id, QString mdp)
{
    QSqlQuery query;
    QString idString = QString::number(id);

    query.prepare("SELECT mdp FROM employer WHERE idemp = :id");
    query.bindValue(":id", idString);

    if (!query.exec()) {
        qDebug() << "SQL Error:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString mdpBD = query.value(0).toString();

        if (mdpBD == mdp) {
            return true;
        }
    }

    return false;
}

QMap<QString, int> employer::getStatistiquesParTypeContrat()
{
    QMap<QString, int> statistiques;
    QSqlQuery query;

    query.prepare("SELECT typecontrat, COUNT(*) as nombre FROM employer GROUP BY typecontrat");
    
    if (query.exec()) {
        while (query.next()) {
            QString typeContrat = query.value(0).toString();
            int nombre = query.value(1).toInt();
            statistiques[typeContrat] = nombre;
        }
    } else {
        qDebug() << "Erreur lors de la récupération des statistiques:" << query.lastError().text();
    }

    return statistiques;
}

QPair<QString, QString> employer::getNameById(const QString &id)
{
    QSqlQuery query;
    query.prepare("SELECT NOM, PRENOM FROM employer WHERE IDEMP = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        QString nom = query.value(0).toString();
        QString prenom = query.value(1).toString();
        return qMakePair(nom, prenom);
    }

    return qMakePair(QString(), QString()); // not found
}


