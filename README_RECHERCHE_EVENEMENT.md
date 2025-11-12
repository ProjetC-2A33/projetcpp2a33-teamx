# Fonctionnalité : Recherche d'Événement par ID

## 🔍 Objectif

Permettre à l'utilisateur de rechercher un événement spécifique en entrant son ID dans le champ `id_event_3` et en cliquant sur le bouton `recherche_event`.

---

## ✅ Modifications Effectuées

### 1. **Fichier `mainwindow.h`**

- ✅ Ajout du slot `void on_recherche_event_clicked();`

### 2. **Fichier `mainwindow.cpp`**

#### a) Connexion du bouton dans `setupEventManagement()` :

```cpp
connect(ui->recherche_event, &QPushButton::clicked, this, &MainWindow::on_recherche_event_clicked);
```

#### b) Implémentation de `on_recherche_event_clicked()` :

```cpp
void MainWindow::on_recherche_event_clicked()
{
    // Récupérer l'ID depuis le champ de recherche
    QString idText = ui->id_event_3->text().trimmed();

    // Validation de l'ID
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "ID requis", "Veuillez entrer un ID d'événement à rechercher.");
        return;
    }

    bool ok;
    int id = idText.toInt(&ok);

    if (!ok || id <= 0) {
        QMessageBox::warning(this, "ID invalide", "L'ID doit être un nombre entier positif.");
        return;
    }

    // Rechercher dans la base de données
    QSqlQuery query;
    query.prepare("SELECT ID_EVENT, LIEUX, DATE_EVENT, TYPE_EV, CONFIDENTIALITE, RESPONSABLE, CAPACITE "
                  "FROM EVENEMENT WHERE ID_EVENT = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.next()) {
            // Événement trouvé - afficher uniquement cet événement dans le tableau
            ui->table_event->setRowCount(1);
            ui->table_event->setColumnCount(7);

            for (int j = 0; j < 7; ++j) {
                QTableWidgetItem* item = new QTableWidgetItem(query.value(j).toString());
                ui->table_event->setItem(0, j, item);
            }

            ui->table_event->resizeColumnsToContents();
            QMessageBox::information(this, "Trouvé", "Événement ID " + QString::number(id) + " trouvé !");

            // Charger automatiquement les données dans le formulaire
            loadEventData(0);
        } else {
            // Aucun événement trouvé
            QMessageBox::information(this, "Non trouvé", "Aucun événement avec l'ID " + QString::number(id) + " n'a été trouvé.");
            ui->table_event->setRowCount(0);
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la recherche : " + query.lastError().text());
    }
}
```

---

## 🎯 Fonctionnement

### Étapes d'utilisation :

1. **Entrer l'ID** : L'utilisateur tape un ID d'événement dans le champ `id_event_3`
2. **Cliquer sur Recherche** : Clic sur le bouton `recherche_event`
3. **Validation** :
   - Vérifie que l'ID n'est pas vide
   - Vérifie que c'est un nombre entier positif
4. **Recherche SQL** :
   - Exécute `SELECT ... FROM EVENEMENT WHERE ID_EVENT = :id`
5. **Affichage des résultats** :
   - **Si trouvé** :
     - Le tableau affiche uniquement cet événement
     - Les données sont automatiquement chargées dans le formulaire
     - Message de succès affiché
   - **Si non trouvé** :
     - Le tableau est vidé
     - Message informatif affiché

---

## 📋 Widgets Utilisés

| Widget            | Type         | Description                         |
| ----------------- | ------------ | ----------------------------------- |
| `id_event_3`      | QLineEdit    | Champ pour entrer l'ID à rechercher |
| `recherche_event` | QPushButton  | Bouton pour lancer la recherche     |
| `table_event`     | QTableWidget | Tableau pour afficher le résultat   |

---

## 🔧 Fonctionnalités Supplémentaires

### Pour afficher tous les événements après une recherche :

L'utilisateur peut cliquer sur le bouton "Annuler" (`anuler`) qui appellera `afficherEvenements()` pour réafficher tous les événements.

Ou vous pouvez ajouter un bouton "Afficher Tout" :

```cpp
// Dans setupEventManagement()
connect(ui->afficher_tout_event, &QPushButton::clicked, this, &MainWindow::afficherEvenements);
```

---

## 🧪 Tests

### Test 1 : Recherche réussie

1. Insérer un événement avec ID = 1
2. Entrer "1" dans `id_event_3`
3. Cliquer sur `recherche_event`
4. ✅ Résultat : L'événement s'affiche dans le tableau et le formulaire

### Test 2 : ID inexistant

1. Entrer "9999" dans `id_event_3`
2. Cliquer sur `recherche_event`
3. ✅ Résultat : Message "Non trouvé", tableau vide

### Test 3 : Validation des entrées

1. Entrer "abc" dans `id_event_3`
2. Cliquer sur `recherche_event`
3. ✅ Résultat : Message "ID invalide"

### Test 4 : Champ vide

1. Laisser `id_event_3` vide
2. Cliquer sur `recherche_event`
3. ✅ Résultat : Message "ID requis"

---

## 🐛 Gestion des Erreurs

| Erreur           | Message                                           | Action                             |
| ---------------- | ------------------------------------------------- | ---------------------------------- |
| Champ vide       | "Veuillez entrer un ID d'événement à rechercher." | Aucune requête SQL exécutée        |
| ID non numérique | "L'ID doit être un nombre entier positif."        | Validation avec `toInt(&ok)`       |
| ID négatif ou 0  | "L'ID doit être un nombre entier positif."        | Vérifie `id <= 0`                  |
| Erreur SQL       | "Erreur lors de la recherche : [détails]"         | Affiche `query.lastError().text()` |

---

## 💡 Améliorations Possibles

### 1. Recherche par texte (au lieu de l'ID uniquement)

```cpp
// Rechercher par lieu, type, ou responsable
query.prepare("SELECT * FROM EVENEMENT WHERE "
              "UPPER(LIEUX) LIKE UPPER(:critere) OR "
              "UPPER(TYPE_EV) LIKE UPPER(:critere) OR "
              "UPPER(RESPONSABLE) LIKE UPPER(:critere)");
query.bindValue(":critere", "%" + critere + "%");
```

### 2. Recherche en temps réel (pendant la frappe)

```cpp
// Dans setupEventManagement()
connect(ui->id_event_3, &QLineEdit::textChanged, this, &MainWindow::on_recherche_event_clicked);
```

### 3. Bouton "Réinitialiser la recherche"

```cpp
void MainWindow::on_reset_recherche_clicked()
{
    ui->id_event_3->clear();
    afficherEvenements();  // Afficher tous les événements
}
```

---

## 🎉 Résultat Final

- ✅ Recherche par ID fonctionnelle
- ✅ Validation des entrées utilisateur
- ✅ Affichage automatique dans le tableau
- ✅ Chargement automatique dans le formulaire
- ✅ Messages d'erreur clairs et informatifs

---

**✨ La fonctionnalité de recherche est opérationnelle !**
