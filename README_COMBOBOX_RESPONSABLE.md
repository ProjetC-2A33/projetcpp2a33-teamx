# Modification : ComboBox Responsable avec Liste des Employés

## 🎯 Objectif

Remplacer le champ texte `responsable` par un **ComboBox** (`comboBox_4`) qui affiche automatiquement la liste des employés de la table `EMPLOYES`.

---

## ✅ Modifications Effectuées

### 1. **Fichier `mainwindow.h`**

- ✅ Ajout de la méthode `void chargerEmployesDansCombo();`
- Cette méthode charge les employés depuis la base de données

### 2. **Fichier `mainwindow.cpp`**

#### a) Imports ajoutés :

```cpp
#include <QSqlQuery>
#include <QSqlError>
```

#### b) Constructeur modifié :

```cpp
MainWindow::MainWindow(QWidget *parent)
{
    // ...
    chargerEmployesDansCombo();  // Charger les employés au démarrage
    afficherEvenements();
}
```

#### c) Nouvelle méthode `chargerEmployesDansCombo()` :

```cpp
void MainWindow::chargerEmployesDansCombo()
{
    ui->comboBox_4->clear();
    ui->comboBox_4->addItem("-- Sélectionner un responsable --", 0);

    QSqlQuery query;
    query.prepare("SELECT ID_EMPLOYE, NOM, PRENOM FROM EMPLOYES ORDER BY NOM, PRENOM");

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString nom = query.value(1).toString();
            QString prenom = query.value(2).toString();
            QString nomComplet = nom + " " + prenom;

            ui->comboBox_4->addItem(nomComplet, id);  // Stocke l'ID comme data
        }
    }
}
```

#### d) Méthode `on_ajout_event_clicked()` modifiée :

- ✅ Remplacé `ui->responsable->text()` par `ui->comboBox_4->currentText()`
- ✅ Récupère l'ID de l'employé avec `ui->comboBox_4->currentData().toInt()`
- ✅ Validation ajoutée : vérifier que `idEmploye != 0`
- ✅ Passage de `idEmploye` au constructeur `Evenement`

#### e) Méthode `on_modifier_event_clicked()` modifiée :

- ✅ Mêmes modifications que pour l'ajout
- ✅ Utilise `comboBox_4` au lieu de `responsable` (QLineEdit)

#### f) Méthode `loadEventData()` modifiée :

```cpp
// Sélectionner le responsable dans le ComboBox
QString responsable = ui->table_event->item(row, 5)->text();
int index = ui->comboBox_4->findText(responsable);
if (index >= 0) {
    ui->comboBox_4->setCurrentIndex(index);
}
```

- ✅ Recherche et sélectionne automatiquement le bon employé dans le ComboBox

#### g) Méthode `clearEventFields()` modifiée :

```cpp
ui->comboBox_4->setCurrentIndex(0);  // Réinitialiser au premier élément
```

- ✅ Remet le ComboBox à l'état initial

### 3. **Fichier `evenement.cpp`**

- ✅ **FIX CRITIQUE** : Ajout de `EVENEMENT_SEQ.NEXTVAL` dans la requête INSERT
- ✅ Résout l'erreur **ORA-01400: cannot insert NULL into ID_EVENT**

**Avant :**

```cpp
INSERT INTO EVENEMENT (LIEUX, DATE_EVENT, ...) VALUES (:lieux, :date, ...)
```

**Après :**

```cpp
INSERT INTO EVENEMENT (ID_EVENT, LIEUX, DATE_EVENT, ...)
VALUES (EVENEMENT_SEQ.NEXTVAL, :lieux, :date, ...)
```

---

## 📋 Prérequis : Insérer des Employés

**Si la table `EMPLOYES` est vide**, le ComboBox sera vide aussi !

### Solution : Exécuter le script `insert_employes_test.sql`

```sql
-- Connectez-vous à SQL*Plus ou SQL Developer en tant que SOUMAYA
sqlplus soumaya/esprit18@Source_Projet2A

-- Puis exécutez :
@insert_employes_test.sql
```

Cela va insérer 5 employés de test :

- Dupont Jean
- Martin Sophie
- Bernard Pierre
- Dubois Marie
- Thomas Laurent

---

## 🔧 Comment Tester

1. **Vérifier que les employés existent** :

```sql
SELECT * FROM EMPLOYES;
```

2. **Recompiler le projet** :

```cmd
cd "c:\Users\Raouf\Desktop\c++"
qmake
mingw32-make clean
mingw32-make
```

3. **Lancer l'application** :

- Aller sur la page "Gestion Événements" (page_5)
- Le ComboBox `comboBox_4` doit afficher les noms des employés
- Sélectionner un responsable
- Remplir les autres champs
- Cliquer sur "Ajouter"
- ✅ L'événement doit être inséré avec l'ID de l'employé

4. **Vérifier dans la base** :

```sql
SELECT E.ID_EVENT, E.LIEUX, E.RESPONSABLE, EMP.NOM, EMP.PRENOM
FROM EVENEMENT E
LEFT JOIN EMPLOYES EMP ON E.ID_EMPLOYE = EMP.ID_EMPLOYE
ORDER BY E.ID_EVENT DESC;
```

---

## 🐛 Problèmes Résolus

| Problème                                               | Solution                                                        |
| ------------------------------------------------------ | --------------------------------------------------------------- |
| **ORA-01400: cannot insert NULL into ID_EVENT**        | Utiliser `EVENEMENT_SEQ.NEXTVAL` explicitement dans INSERT      |
| **ComboBox vide**                                      | Vérifier que la table EMPLOYES contient des données             |
| **Responsable non rempli après sélection d'une ligne** | Utiliser `findText()` pour retrouver l'employé dans le ComboBox |
| **ID_EMPLOYE toujours 0**                              | Utiliser `currentData().toInt()` au lieu de `currentText()`     |

---

## 📊 Structure de la Table EVENEMENT

```sql
CREATE TABLE EVENEMENT (
    ID_EVENT       NUMBER NOT NULL,           -- Auto-incrémenté par EVENEMENT_SEQ
    LIEUX          VARCHAR2(20 BYTE),
    DATE_EVENT     DATE,
    TYPE_EV        NVARCHAR2(20),
    CONFIDENTIALITE VARCHAR2(20 BYTE),
    RESPONSABLE    VARCHAR2(20 BYTE),         -- Nom complet (affiché)
    CAPACITE       NUMBER,
    ID_EMPLOYE     NUMBER,                    -- FK vers EMPLOYES.ID_EMPLOYE
    CONSTRAINT EVENEMENT_PK PRIMARY KEY (ID_EVENT),
    CONSTRAINT EVENEMENT_FK_EMP FOREIGN KEY (ID_EMPLOYE) REFERENCES EMPLOYES (ID_EMPLOYE)
);
```

---

## 🎉 Résultat Final

- ✅ Le ComboBox affiche automatiquement tous les employés
- ✅ L'ID de l'employé est correctement stocké dans `ID_EMPLOYE`
- ✅ Le nom complet est stocké dans `RESPONSABLE` pour affichage
- ✅ Les événements sont créés sans erreur ORA-01400
- ✅ La modification et la sélection fonctionnent correctement

---

## 📝 Notes Importantes

1. **comboBox_4** : C'est le widget utilisé dans `mainwindow.ui` à la ligne 7615
2. **ID stocké comme data** : `addItem(nomComplet, id)` permet de stocker l'ID invisible
3. **Clé étrangère** : Oracle vérifie automatiquement que `ID_EMPLOYE` existe dans EMPLOYES
4. **NULL autorisé** : Si vous voulez permettre des événements sans responsable, modifiez la validation

---

**✨ Bon travail ! Le système de sélection des responsables est maintenant opérationnel !**
