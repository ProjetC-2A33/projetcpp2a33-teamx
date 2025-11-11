# Fonctionnalité : Tri des Événements

## 🎯 Objectif

Permettre à l'utilisateur de trier les événements affichés dans le tableau selon différents critères (Date, Lieu, ID) en utilisant le ComboBox `comboBox_6` et le bouton `certificat_10`.

---

## ✅ Modifications Effectuées

### 1. **Fichier `mainwindow.h`**

- ✅ Ajout du slot `void on_certificat_10_clicked();`

### 2. **Fichier `mainwindow.cpp`**

#### a) Connexion du bouton dans `setupEventManagement()` :

```cpp
connect(ui->certificat_10, &QPushButton::clicked, this, &MainWindow::on_certificat_10_clicked);
```

#### b) Implémentation de `on_certificat_10_clicked()` :

```cpp
void MainWindow::on_certificat_10_clicked()
{
    // Récupérer le critère de tri sélectionné dans le ComboBox
    QString critere = ui->comboBox_6->currentText();

    QString colonneSQL;
    QString nomColonne;

    // Déterminer la colonne SQL selon le critère
    if (critere == "Date") {
        colonneSQL = "DATE_EVENT";
        nomColonne = "date";
    } else if (critere.contains("lieux", Qt::CaseInsensitive) || critere == "lieux d evenement") {
        colonneSQL = "LIEUX";
        nomColonne = "lieu";
    } else if (critere.contains("ID", Qt::CaseInsensitive)) {
        colonneSQL = "ID_EVENT";
        nomColonne = "ID";
    } else {
        // Par défaut, trier par ID
        colonneSQL = "ID_EVENT";
        nomColonne = "ID";
    }

    // Utiliser la méthode trierPar de la classe Evenement
    Evenement ev;
    QSqlQueryModel* model = ev.trierPar(colonneSQL, "ASC");

    // Vérifier si la requête a réussi
    if (model->lastError().isValid()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du tri : " + model->lastError().text());
        delete model;
        return;
    }

    // Remplir le tableau avec les données triées
    ui->table_event->setRowCount(model->rowCount());
    ui->table_event->setColumnCount(model->columnCount());

    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(model->data(model->index(i, j)).toString());
            ui->table_event->setItem(i, j, item);
        }
    }

    // Ajuster les colonnes
    ui->table_event->resizeColumnsToContents();

    delete model;

    // Message de confirmation
    QMessageBox::information(this, "Tri effectué",
                           QString("Événements triés par %1 (ordre croissant).").arg(nomColonne));
}
```

---

## 🎯 Fonctionnement

### Étapes d'utilisation :

1. **Sélectionner le critère** : L'utilisateur choisit dans `comboBox_6` :

   - "Date" : Tri par date d'événement
   - "lieux d evenement" : Tri par lieu
   - "ID" (si ajouté) : Tri par ID d'événement

2. **Cliquer sur le bouton Tri** : Clic sur `certificat_10`

3. **Traitement** :

   - Récupération du critère sélectionné
   - Mapping vers la colonne SQL appropriée
   - Appel à `ev.trierPar(colonneSQL, "ASC")`
   - Ordre croissant (ASC) : A→Z, 1→9, dates anciennes→récentes

4. **Affichage** :
   - Le tableau est vidé et rempli avec les données triées
   - Les colonnes sont redimensionnées automatiquement
   - Message de confirmation affiché

---

## 📋 Options de Tri Actuelles

| Option ComboBox       | Colonne SQL  | Description                                          |
| --------------------- | ------------ | ---------------------------------------------------- |
| **Date**              | `DATE_EVENT` | Tri chronologique (date la plus ancienne en premier) |
| **lieux d evenement** | `LIEUX`      | Tri alphabétique par lieu                            |
| **ID** (optionnel)    | `ID_EVENT`   | Tri numérique par identifiant                        |

---

## 🔧 Widgets Utilisés

| Widget          | Type         | Description                            |
| --------------- | ------------ | -------------------------------------- |
| `comboBox_6`    | QComboBox    | Sélection du critère de tri            |
| `certificat_10` | QPushButton  | Bouton pour appliquer le tri           |
| `table_event`   | QTableWidget | Tableau affichant les événements triés |

---

## 💡 Améliorations Possibles

### Option 1 : Ajouter l'option "ID" dans le ComboBox

Modifiez `mainwindow.ui` dans Qt Designer :

1. Sélectionner `comboBox_6`
2. Ajouter un nouvel item : "ID événement"

Le code détectera automatiquement "ID" grâce à :

```cpp
if (critere.contains("ID", Qt::CaseInsensitive))
```

### Option 2 : Tri Ascendant/Descendant

Ajouter un second ComboBox pour l'ordre :

```cpp
QString ordre = ui->comboBox_ordre->currentText() == "Croissant" ? "ASC" : "DESC";
QSqlQueryModel* model = ev.trierPar(colonneSQL, ordre);
```

### Option 3 : Tri automatique au changement de sélection

Connecter le signal `currentIndexChanged` du ComboBox :

```cpp
// Dans setupEventManagement()
connect(ui->comboBox_6, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MainWindow::on_certificat_10_clicked);
```

Ainsi, le tri s'applique automatiquement dès qu'on change la sélection.

### Option 4 : Sans message de confirmation

Si le message est trop intrusif :

```cpp
// Supprimer ces lignes :
// QMessageBox::information(this, "Tri effectué",
//                        QString("Événements triés par %1 (ordre croissant).").arg(nomColonne));
```

---

## 🧪 Tests

### Test 1 : Tri par Date

1. Ajouter 3 événements avec dates : 2024-01-15, 2024-03-20, 2024-02-10
2. Sélectionner "Date" dans `comboBox_6`
3. Cliquer sur `certificat_10`
4. ✅ Résultat : Ordre affiché = 15/01/2024, 10/02/2024, 20/03/2024

### Test 2 : Tri par Lieu

1. Ajouter événements : "Paris", "Marseille", "Lyon"
2. Sélectionner "lieux d evenement"
3. Cliquer sur `certificat_10`
4. ✅ Résultat : Ordre = Lyon, Marseille, Paris

### Test 3 : Tri par ID (si ajouté)

1. Créer événements ID = 5, 2, 8
2. Sélectionner "ID événement"
3. Cliquer sur `certificat_10`
4. ✅ Résultat : Ordre = 2, 5, 8

### Test 4 : Table vide

1. Supprimer tous les événements
2. Sélectionner n'importe quel critère
3. Cliquer sur `certificat_10`
4. ✅ Résultat : Tableau vide, message de tri affiché

---

## 🔄 Intégration avec Autres Fonctionnalités

Cette fonction fonctionne avec :

- ✅ **Afficher Tout** (`certificat_15`) : Peut trier après avoir réaffiché tous les événements
- ✅ **Recherche par ID** : Peut trier les résultats de recherche (si plusieurs)
- ✅ **CRUD** : Le tri peut être réappliqué après ajout/modification/suppression

---

## 📊 Ordre de Tri (ASC - Croissant)

### Pour les Dates :

```
2024-01-01
2024-02-15
2024-12-31
```

### Pour les Lieux (Alphabétique) :

```
Bordeaux
Lyon
Marseille
Paris
Toulouse
```

### Pour les ID (Numérique) :

```
1
2
10
25
100
```

---

## ⚠️ Notes Importantes

1. **Ordre fixe** : Actuellement en ordre croissant (ASC)

   - Pour inverser : remplacer `"ASC"` par `"DESC"` dans le code

2. **Méthode existante** : Utilise `ev.trierPar()` de la classe `Evenement`

   - Cette méthode doit accepter le nom de colonne et l'ordre

3. **Gestion des erreurs** :

   - Vérifie `model->lastError().isValid()`
   - Affiche un message d'erreur SQL si problème

4. **Performance** :
   - Le tri est fait par SQL (très rapide)
   - Pas de tri côté client (QTableWidget)

---

## 🎉 Résultat Final

- ✅ Tri par Date fonctionnel
- ✅ Tri par Lieu fonctionnel
- ✅ Tri par ID disponible (si ajouté au ComboBox)
- ✅ Message de confirmation
- ✅ Gestion des erreurs SQL
- ✅ Interface intuitive

---

**✨ Les utilisateurs peuvent maintenant organiser les événements selon leurs besoins !**
