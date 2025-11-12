# Gestion des Événements - CRUD Complet

## Vue d'ensemble

Ce module implémente un système CRUD (Create, Read, Update, Delete) complet pour la gestion des événements dans l'application SummerClub.

## Fichiers créés/modifiés

### Nouveaux fichiers

- **evenement.h** : Déclaration de la classe Evenement avec tous les attributs et méthodes CRUD
- **evenement.cpp** : Implémentation des méthodes CRUD et interactions avec la base de données Oracle

### Fichiers modifiés

- **mainwindow.h** : Ajout des slots et méthodes pour la gestion des événements
- **mainwindow.cpp** : Implémentation de l'interface utilisateur et connexion avec la classe Evenement
- **Atelier_Connexion.pro** : Ajout des nouveaux fichiers au projet

## Structure de la table EVENEMENT

```sql
CREATE TABLE EVENEMENT (
    ID_EVENT       NUMBER NOT NULL PRIMARY KEY,
    LIEUX          VARCHAR2(20 BYTE),
    DATE_EVENT     DATE,
    TYPE_EV        NVARCHAR2(20),
    CONFIDENTIALITE VARCHAR2(20 BYTE),
    RESPONSABLE    VARCHAR2(20 BYTE),
    CAPACITE       NUMBER,
    ID_EMPLOYE     NUMBER
);
```

## Fonctionnalités implémentées

### 1. Ajouter un événement

- **Bouton** : `ajout_event`
- **Champs requis** :
  - Lieux de l'événement (`lieux_event`)
  - Date de l'événement (`date_event`)
  - Type d'événement (`type_event`)
  - Confidentialité (radio buttons : `confidentialit_prive` ou `confidentialit_public`)
  - Responsable (`responsable`)
  - Capacité (`capacite`)
- **Validation** : Tous les champs doivent être remplis, capacité > 0
- **Résultat** : Insertion dans la base de données et rafraîchissement du tableau

### 2. Modifier un événement

- **Bouton** : `modifier_event`
- **Processus** :
  1. Sélectionner un événement dans le tableau (`table_event`)
  2. Les champs se remplissent automatiquement
  3. Modifier les valeurs souhaitées
  4. Cliquer sur "Modifier"
- **Validation** : ID requis, tous les champs doivent être valides
- **Résultat** : Mise à jour dans la base de données

### 3. Afficher les événements

- **Tableau** : `table_event`
- **Colonnes** :
  - ID Événement
  - Lieux d'événements
  - Date
  - Type
  - Confidentialité
  - Responsable
  - Capacité
- **Mise à jour** : Automatique après ajout/modification/suppression
- **Interaction** : Clic sur une ligne pour charger les données dans le formulaire

### 4. Supprimer un événement

- **Méthode** : `supprimerEvenement()`
- **Processus** :
  1. Sélectionner un événement
  2. Le système demande confirmation
  3. Suppression de la base de données
- **Note** : À connecter à un bouton si nécessaire

### 5. Annuler/Effacer

- **Bouton** : `anuler`
- **Action** : Efface tous les champs du formulaire et réinitialise les sélections

### 6. Recherche (prévu)

- **Méthode** : `rechercherEvenement()`
- **Fonctionnalité** : Recherche multi-critères (lieux, type, responsable, confidentialité)
- **Note** : Prêt à être connecté à un champ de recherche

### 7. Tri

- **Méthode** : `trierPar(colonne, ordre)`
- **Fonctionnalité** : Tri par n'importe quelle colonne en ordre ASC ou DESC
- **Note** : Peut être connecté aux en-têtes de colonnes du tableau

## Widgets de l'interface (page_5)

### Formulaire d'ajout/modification (groupBox_13)

- `id_event` : QLineEdit - ID événement (auto-généré, visible en modification)
- `lieux_event` : QLineEdit - Lieux
- `date_event` : QDateTimeEdit - Date de l'événement
- `type_event` : QLineEdit - Type d'événement
- `confidentialit_prive` : QRadioButton - Confidentialité privée
- `confidentialit_public` : QRadioButton - Confidentialité publique
- `responsable` : QLineEdit - Nom du responsable
- `capacite` : QLineEdit - Capacité maximale

### Boutons d'action

- `ajout_event` : Ajouter un événement
- `modifier_event` : Modifier l'événement sélectionné
- `anuler` : Annuler/Effacer les champs

### Tableau d'affichage

- `table_event` : QTableWidget - Affiche tous les événements

## Utilisation

### Ajouter un événement

1. Remplir tous les champs du formulaire
2. Sélectionner la confidentialité (Privée ou Public)
3. Cliquer sur "Ajouter"
4. Message de confirmation
5. Le tableau se met à jour automatiquement

### Modifier un événement

1. Cliquer sur une ligne du tableau
2. Les champs se remplissent automatiquement
3. Modifier les valeurs souhaitées
4. Cliquer sur "Modifier"
5. Message de confirmation

### Supprimer un événement

1. Sélectionner un événement dans le tableau
2. Appeler `supprimerEvenement()` (à connecter à un bouton)
3. Confirmer la suppression
4. L'événement est supprimé

## Points techniques

### Gestion de la base de données

- Utilise `QSqlQuery` pour les opérations CRUD
- Gestion des erreurs avec `QSqlError`
- Logs debug pour faciliter le débogage
- Utilisation de requêtes préparées (protection contre injection SQL)

### Validation des données

- Vérification des champs obligatoires
- Validation de la capacité (doit être > 0)
- Gestion des dates avec `QDate`
- Conversion automatique des types de données

### Interface utilisateur

- Sélection de ligne entière dans le tableau
- Mode lecture seule pour le tableau
- Chargement automatique des données au clic
- Messages informatifs (succès/erreur/confirmation)

## Améliorations possibles

1. **Bouton Supprimer** : Ajouter un bouton explicite dans l'UI pour la suppression
2. **Recherche en temps réel** : Connecter un QLineEdit pour filtrer les résultats
3. **Tri interactif** : Clic sur les en-têtes de colonnes pour trier
4. **Export PDF** : Générer un rapport PDF des événements
5. **Validation avancée** : Vérifier les doublons, les dates passées, etc.
6. **Liaison avec employés** : ComboBox pour sélectionner un employé responsable
7. **Statistiques** : Afficher le nombre d'événements, capacité totale, etc.
8. **Filtres avancés** : Par date, par type, par confidentialité

## Compilation et exécution

### Windows (Qt Creator)

1. Ouvrir `Atelier_Connexion.pro` dans Qt Creator
2. Configurer le kit de build (MinGW ou MSVC)
3. Build → Run (ou Ctrl+R)

### Windows (ligne de commande)

```cmd
cd c:\Users\Raouf\Desktop\c++
qmake
mingw32-make
```

## Prérequis

- Qt 5.x ou supérieur (avec module QtSql)
- Base de données Oracle configurée
- DSN ODBC "SummerClub" configuré
- Tables créées (voir `create_summerclub.sql`)

## Support

En cas de problème :

1. Vérifier la connexion à la base de données
2. Consulter les logs debug dans la console
3. Vérifier que les noms des widgets dans `mainwindow.ui` correspondent
4. S'assurer que la table EVENEMENT existe dans la base

## Auteurs

Développé pour le projet SummerClub - Gestion d'événements
