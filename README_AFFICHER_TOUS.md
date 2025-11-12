# Fonctionnalité : Afficher Tous les Événements

## 🎯 Objectif

Ajouter un bouton (`certificat_15`) qui permet de réafficher tous les événements dans le tableau, particulièrement utile après avoir effectué une recherche par ID.

---

## ✅ Modifications Effectuées

### 1. **Fichier `mainwindow.h`**

- ✅ Ajout du slot `void on_certificat_15_clicked();`

### 2. **Fichier `mainwindow.cpp`**

#### a) Connexion du bouton dans `setupEventManagement()` :

```cpp
connect(ui->certificat_15, &QPushButton::clicked, this, &MainWindow::on_certificat_15_clicked);
```

#### b) Implémentation de `on_certificat_15_clicked()` :

```cpp
void MainWindow::on_certificat_15_clicked()
{
    // Effacer le champ de recherche si présent
    ui->id_event_3->clear();

    // Afficher tous les événements dans le tableau
    afficherEvenements();

    // Message optionnel
    QMessageBox::information(this, "Affichage complet",
                           "Tous les événements sont maintenant affichés dans le tableau.");
}
```

---

## 🎯 Fonctionnement

### Actions effectuées par le bouton :

1. **Vider le champ de recherche** : `ui->id_event_3->clear()`

   - Efface l'ID qui pourrait être resté dans le champ de recherche

2. **Afficher tous les événements** : `afficherEvenements()`

   - Appelle la méthode existante qui récupère tous les événements de la base de données
   - Remplit le tableau `table_event` avec tous les enregistrements

3. **Message de confirmation** :
   - Affiche une boîte de dialogue informant l'utilisateur que tous les événements sont affichés

---

## 📋 Cas d'Utilisation

### Scénario 1 : Après une recherche

1. L'utilisateur recherche un événement par ID
2. Le tableau affiche uniquement cet événement
3. L'utilisateur clique sur `certificat_15`
4. ✅ Le tableau affiche maintenant tous les événements

### Scénario 2 : Rafraîchir l'affichage

1. Après avoir ajouté/modifié/supprimé des événements
2. L'utilisateur clique sur `certificat_15`
3. ✅ Le tableau est rafraîchi avec toutes les données actuelles

### Scénario 3 : Réinitialiser après filtrage

1. Après avoir appliqué un filtre ou une recherche
2. L'utilisateur veut revenir à l'affichage complet
3. Clic sur `certificat_15`
4. ✅ Vue complète restaurée

---

## 🔧 Widget Utilisé

| Widget          | Type        | Localisation             | Description                              |
| --------------- | ----------- | ------------------------ | ---------------------------------------- |
| `certificat_15` | QPushButton | mainwindow.ui ligne 7739 | Bouton pour afficher tous les événements |

---

## 💡 Améliorations Possibles

### Option 1 : Sans message de confirmation

Si vous trouvez le message de confirmation intrusif, vous pouvez le supprimer :

```cpp
void MainWindow::on_certificat_15_clicked()
{
    ui->id_event_3->clear();
    afficherEvenements();
    // Pas de message - action silencieuse
}
```

### Option 2 : Avec compteur d'événements

Afficher le nombre d'événements dans le message :

```cpp
void MainWindow::on_certificat_15_clicked()
{
    ui->id_event_3->clear();
    afficherEvenements();

    int count = ui->table_event->rowCount();
    QMessageBox::information(this, "Affichage complet",
                           QString("%1 événement(s) affiché(s).").arg(count));
}
```

### Option 3 : Réinitialiser aussi le formulaire

Vider également les champs du formulaire :

```cpp
void MainWindow::on_certificat_15_clicked()
{
    ui->id_event_3->clear();
    clearEventFields();  // Vide aussi le formulaire
    afficherEvenements();

    QMessageBox::information(this, "Affichage complet",
                           "Tous les événements sont affichés. Formulaire réinitialisé.");
}
```

---

## 🎨 Personnalisation du Bouton

Vous pouvez personnaliser le texte et l'icône du bouton dans Qt Designer :

1. **Texte** : "Afficher Tout" ou "Réinitialiser" ou "Tous les Événements"
2. **Icône** : Ajouter une icône de rafraîchissement ou de liste
3. **Tooltip** : Ajouter une info-bulle "Afficher tous les événements dans le tableau"

---

## 🧪 Tests

### Test 1 : Après recherche par ID

1. Rechercher événement ID = 1
2. Tableau affiche uniquement l'événement 1
3. Cliquer sur `certificat_15`
4. ✅ Résultat : Tous les événements s'affichent

### Test 2 : Champ de recherche vidé

1. Entrer "123" dans `id_event_3`
2. Cliquer sur `certificat_15`
3. ✅ Résultat : Le champ `id_event_3` est vide, tous les événements affichés

### Test 3 : Base de données vide

1. Supprimer tous les événements
2. Cliquer sur `certificat_15`
3. ✅ Résultat : Tableau vide, message affiché

---

## 🔄 Intégration avec Autres Fonctionnalités

Cette fonction s'intègre parfaitement avec :

- ✅ **Recherche par ID** : Permet de revenir à la vue complète
- ✅ **Ajout d'événement** : Rafraîchit après ajout
- ✅ **Modification** : Actualise le tableau
- ✅ **Suppression** : Recharge tous les événements restants
- ✅ **Bouton Annuler** : Complète la réinitialisation

---

## 🎉 Résultat Final

- ✅ Bouton `certificat_15` fonctionnel
- ✅ Affiche tous les événements en un clic
- ✅ Efface le champ de recherche
- ✅ Message de confirmation
- ✅ Intégration parfaite avec les autres fonctions CRUD

---

**✨ L'utilisateur peut maintenant facilement revenir à l'affichage complet après une recherche !**
