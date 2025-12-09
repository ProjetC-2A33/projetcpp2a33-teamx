
#ifndef ENFANT_H
#define ENFANT_H

#include <QString>
#include <QSqlQueryModel>

class Enfant
{
public:
    Enfant() {}
    Enfant(int id, const QString& nom, int age, const QString& allergie,
           const QString& parent, int num, const QString& email)
        : id_enfant(id),
          nom_prenom(nom),
          age_val(age),
          allergie_remarque(allergie),
          nom_prenom_parents(parent),
          num_val(num),
          email_val(email) {}

    bool ajouter() const;
    bool modifier(int id) const;
    bool supprimer(int id) const;
    QSqlQueryModel* afficher() const;

private:
    int id_enfant = 0;
    QString nom_prenom;
    int age_val = 0;
    QString allergie_remarque;
    QString nom_prenom_parents;
    int num_val = 0;
    QString email_val;
};

#endif // ENFANT_H
