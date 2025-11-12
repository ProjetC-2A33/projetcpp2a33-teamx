# Fonctionnalité : Statistiques des Événements par Mois

## 🎯 Objectif

Afficher des statistiques détaillées sur les événements regroupés par mois et année, avec possibilité d'export en PDF.

---

## ✅ Modifications Effectuées

### 1. **Fichier `mainwindow.h`**

- ✅ Ajout du slot `void on_certificat_11_clicked();`

### 2. **Fichier `mainwindow.cpp`**

#### a) Connexion du bouton dans `setupEventManagement()` :

```cpp
connect(ui->certificat_11, &QPushButton::clicked, this, &MainWindow::on_certificat_11_clicked);
```

#### b) Implémentation de `on_certificat_11_clicked()` :

Requête SQL qui regroupe les événements par mois et année :

```sql
SELECT
    TO_CHAR(DATE_EVENT, 'YYYY') AS ANNEE,
    TO_CHAR(DATE_EVENT, 'MM') AS MOIS,
    TO_CHAR(DATE_EVENT, 'Month', 'NLS_DATE_LANGUAGE=FRENCH') AS NOM_MOIS,
    COUNT(*) AS NOMBRE
FROM EVENEMENT
GROUP BY TO_CHAR(DATE_EVENT, 'YYYY'), TO_CHAR(DATE_EVENT, 'MM'), TO_CHAR(DATE_EVENT, 'Month', 'NLS_DATE_LANGUAGE=FRENCH')
ORDER BY ANNEE DESC, MOIS DESC
```

---

## 🎯 Fonctionnalités

### 📊 Affichage des Statistiques

Quand l'utilisateur clique sur le bouton `certificat_11`, une boîte de dialogue s'ouvre avec :

1. **Tableau détaillé** :

   - Colonne "Année"
   - Colonne "Mois" (en français : Janvier, Février, etc.)
   - Colonne "Nombre d'Événements"
   - Colonne "Pourcentage" (par rapport au total)

2. **Ligne de total** :

   - Total général des événements
   - Pourcentage = 100%

3. **Résumé statistique** :
   - 📈 **Total Événements** : Nombre total d'événements
   - 📅 **Mois Actifs** : Nombre de mois différents ayant des événements
   - 📊 **Moyenne/Mois** : Nombre moyen d'événements par mois

### 💾 Export PDF

- Bouton **"Save"** dans la boîte de dialogue
- Permet d'exporter les statistiques en PDF avec le même design professionnel
- Format A4 portrait
- Fichier par défaut : `statistiques_evenements.pdf`

---

## 🎨 Design du Rapport

### Couleurs :

- **En-têtes** : Dégradé vert (#10B981 → #059669)
- **Lignes paires** : Gris clair (#F9FAFB)
- **Lignes impaires** : Blanc (#FFFFFF)
- **Hover** : Vert très clair (#ECFDF5)
- **Ligne total** : Bleu clair dégradé
- **Statistiques** : Encadrés blancs avec ombre

### Mise en forme :

- Police : Arial, sans-serif
- Arrondis sur les encadrés
- Ombres portées pour profondeur
- Centrage des données numériques
- Grand titre avec icône 📊

---

## 📋 Exemple de Résultat

### Tableau affiché :

| Année     | Mois      | Nombre d'Événements | Pourcentage |
| --------- | --------- | ------------------- | ----------- |
| 2025      | Novembre  | 15                  | 30.0%       |
| 2025      | Octobre   | 12                  | 24.0%       |
| 2025      | Septembre | 10                  | 20.0%       |
| 2025      | Août      | 8                   | 16.0%       |
| 2024      | Décembre  | 5                   | 10.0%       |
| **TOTAL** |           | **50**              | **100%**    |

### Résumé :

- 📈 **Total Événements** : 50
- 📅 **Mois Actifs** : 5
- 📊 **Moyenne/Mois** : 10.0

---

## 🧪 Tests

### Test 1 : Événements répartis sur plusieurs mois

1. Créer des événements :
   - 5 en janvier 2025
   - 3 en février 2025
   - 7 en mars 2025
2. Cliquer sur `certificat_11`
3. ✅ Résultat :
   - Mars 2025 : 7 (46.7%)
   - Février 2025 : 3 (20.0%)
   - Janvier 2025 : 5 (33.3%)
   - Total : 15

### Test 2 : Tous les événements dans le même mois

1. Créer 10 événements tous en décembre 2024
2. Cliquer sur `certificat_11`
3. ✅ Résultat :
   - Décembre 2024 : 10 (100%)
   - Moyenne/Mois : 10.0

### Test 3 : Aucun événement

1. Base de données vide
2. Cliquer sur `certificat_11`
3. ✅ Résultat : Tableau vide avec total = 0

### Test 4 : Export PDF

1. Afficher les statistiques
2. Cliquer sur "Save"
3. Choisir un emplacement
4. ✅ Résultat : PDF créé avec le même contenu stylé

---

## 🔧 Widget Utilisé

| Widget          | Type        | Description                           |
| --------------- | ----------- | ------------------------------------- |
| `certificat_11` | QPushButton | Bouton pour afficher les statistiques |

---

## 💡 Améliorations Possibles

### Option 1 : Graphique en barres

Ajouter un graphique visuel avec QtCharts :

```cpp
QT += charts  // Dans le .pro
```

### Option 2 : Filtrer par année

Ajouter un ComboBox pour sélectionner une année spécifique :

```sql
WHERE TO_CHAR(DATE_EVENT, 'YYYY') = :annee
```

### Option 3 : Statistiques par type d'événement

```sql
SELECT TYPE_EV, COUNT(*)
FROM EVENEMENT
GROUP BY TYPE_EV
```

### Option 4 : Comparaison année N vs année N-1

```sql
SELECT
    TO_CHAR(DATE_EVENT, 'Month') AS MOIS,
    SUM(CASE WHEN TO_CHAR(DATE_EVENT, 'YYYY') = '2025' THEN 1 ELSE 0 END) AS ANNEE_2025,
    SUM(CASE WHEN TO_CHAR(DATE_EVENT, 'YYYY') = '2024' THEN 1 ELSE 0 END) AS ANNEE_2024
FROM EVENEMENT
GROUP BY TO_CHAR(DATE_EVENT, 'Month')
```

### Option 5 : Ajouter un graphique circulaire (pie chart)

Pour visualiser la répartition en pourcentage.

---

## 🔄 Intégration avec Autres Fonctionnalités

Cette fonction complète le système CRUD en offrant :

- ✅ **Vue analytique** des données existantes
- ✅ **Export PDF** indépendant du tableau principal
- ✅ **Aide à la décision** (quels mois sont les plus chargés)
- ✅ **Planification** (identifier les tendances saisonnières)

---

## 📊 Requête SQL Expliquée

```sql
SELECT
    TO_CHAR(DATE_EVENT, 'YYYY') AS ANNEE,           -- Année (2025)
    TO_CHAR(DATE_EVENT, 'MM') AS MOIS,              -- Numéro du mois (01-12)
    TO_CHAR(DATE_EVENT, 'Month', 'NLS_DATE_LANGUAGE=FRENCH') AS NOM_MOIS,  -- Nom en français
    COUNT(*) AS NOMBRE                               -- Comptage
FROM EVENEMENT
GROUP BY TO_CHAR(DATE_EVENT, 'YYYY'),
         TO_CHAR(DATE_EVENT, 'MM'),
         TO_CHAR(DATE_EVENT, 'Month', 'NLS_DATE_LANGUAGE=FRENCH')
ORDER BY ANNEE DESC, MOIS DESC                      -- Plus récent d'abord
```

**Note** : `NLS_DATE_LANGUAGE=FRENCH` force l'affichage en français (Janvier, Février, etc.)

---

## ⚠️ Points Importants

1. **Oracle uniquement** : La requête utilise `TO_CHAR` d'Oracle

   - Pour MySQL/PostgreSQL, adapter la syntaxe

2. **Dates NULL** : Les événements sans date ne sont pas comptés

   - Ajouter `WHERE DATE_EVENT IS NOT NULL` si nécessaire

3. **Performance** :

   - La requête est rapide même avec milliers d'événements
   - Utilise un GROUP BY au lieu de boucles

4. **Langue** :
   - Les noms de mois sont en français
   - Dépend de la configuration Oracle

---

## 🎉 Résultat Final

- ✅ Statistiques par mois fonctionnelles
- ✅ Calcul automatique des pourcentages
- ✅ Affichage du total et de la moyenne
- ✅ Design professionnel avec couleurs
- ✅ Export PDF des statistiques
- ✅ Interface intuitive (bouton unique)

---

**✨ Les utilisateurs peuvent maintenant analyser la répartition temporelle de leurs événements !**
