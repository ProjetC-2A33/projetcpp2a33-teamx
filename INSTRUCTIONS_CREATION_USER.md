# Guide de création de l'utilisateur Oracle SOUMAYA

## Prérequis

- Oracle Database installé et en cours d'exécution
- Accès à un compte administrateur (SYSTEM ou SYS)
- SQL\*Plus, SQL Developer, ou autre client Oracle

## Méthode 1: Via SQL Developer (Recommandé pour débutants)

### Étape 1: Se connecter en tant qu'administrateur

1. Ouvrir **Oracle SQL Developer**
2. Créer une nouvelle connexion:
   - **Nom de connexion**: `SYSTEM` (ou `Admin`)
   - **Nom d'utilisateur**: `SYSTEM` (ou `SYS`)
   - **Mot de passe**: `votre_mot_de_passe_admin`
   - **Rôle**: `SYSDBA` (si vous utilisez SYS)
   - **Nom d'hôte**: `localhost`
   - **Port**: `1521`
   - **SID**: `xe` (ou votre SID)
3. Cliquer sur **Tester** puis **Connexion**

### Étape 2: Exécuter le script de création

1. Ouvrir le fichier `create_user_soumaya.sql`
2. Cliquer sur **Exécuter le script** (icône de document avec flèche verte) ou F5
3. Vérifier qu'il n'y a pas d'erreurs dans la sortie

### Étape 3: Créer une connexion pour SOUMAYA

1. Créer une nouvelle connexion dans SQL Developer:
   - **Nom de connexion**: `SummerClub` ou `SOUMAYA`
   - **Nom d'utilisateur**: `soumaya`
   - **Mot de passe**: `esprit18`
   - **Nom d'hôte**: `localhost`
   - **Port**: `1521`
   - **SID**: `xe` (ou votre SID)
2. Tester la connexion

### Étape 4: Créer les tables

1. Se connecter avec l'utilisateur **soumaya**
2. Exécuter le script `create_summerclub.sql`
3. Vérifier que toutes les tables sont créées

## Méthode 2: Via SQL\*Plus (Ligne de commande)

### Étape 1: Se connecter en tant que SYSTEM

```cmd
sqlplus system/votre_mot_de_passe@localhost:1521/xe
```

Ou en tant que SYS:

```cmd
sqlplus sys/votre_mot_de_passe@localhost:1521/xe as sysdba
```

### Étape 2: Exécuter le script

```sql
@c:\Users\Raouf\Desktop\c++\create_user_soumaya.sql
```

### Étape 3: Se connecter avec le nouvel utilisateur

```cmd
sqlplus soumaya/esprit18@localhost:1521/xe
```

### Étape 4: Créer les tables

```sql
@c:\Users\Raouf\Desktop\c++\create_summerclub.sql
```

## Méthode 3: Création manuelle via SQL Developer

### Étape 1: Interface graphique

1. Se connecter en tant que SYSTEM
2. Dans l'arborescence, cliquer droit sur **"Autres utilisateurs"**
3. Sélectionner **"Créer un utilisateur"**
4. Remplir les informations:
   - **Nom d'utilisateur**: `soumaya`
   - **Mot de passe**: `esprit18`
   - **Confirmer le mot de passe**: `esprit18`
   - **Tablespace par défaut**: `USERS`
   - **Tablespace temporaire**: `TEMP`
   - **Quota sur USERS**: `Illimité`

### Étape 2: Accorder les rôles

1. Onglet **"Rôles accordés"**
2. Cocher: `CONNECT`, `RESOURCE`

### Étape 3: Accorder les privilèges système

1. Onglet **"Privilèges système"**
2. Ajouter:
   - `CREATE SESSION`
   - `CREATE TABLE`
   - `CREATE VIEW`
   - `CREATE SEQUENCE`
   - `CREATE TRIGGER`
   - `CREATE PROCEDURE`

### Étape 4: Appliquer

1. Cliquer sur **"Appliquer"**
2. Vérifier que l'utilisateur apparaît dans la liste

## Configuration du DSN ODBC "SummerClub"

### Windows - Configuration ODBC

#### Étape 1: Ouvrir l'administrateur ODBC

1. Appuyer sur **Win + R**
2. Taper `odbcad32` et Entrée
3. Ou rechercher **"Sources de données ODBC"** dans le menu Démarrer

#### Étape 2: Créer un nouveau DSN

1. Onglet **"DSN utilisateur"** ou **"DSN système"** (recommandé: système)
2. Cliquer sur **"Ajouter"**
3. Sélectionner **"Oracle in OraClient..."** ou **"Microsoft ODBC for Oracle"**
4. Cliquer sur **"Terminer"**

#### Étape 3: Configurer le DSN

1. **Nom de la source de données**: `SummerClub`
2. **Description**: `Base de données SummerClub`
3. **TNS Service Name**: Sélectionner votre service (ex: `XE`, `ORCL`)

   OU configurer manuellement:

   - **Serveur**: `localhost`
   - **Port**: `1521`
   - **SID/Service**: `xe` (ou votre SID)

4. Cliquer sur **"Tester la connexion"**:

   - **ID utilisateur**: `soumaya`
   - **Mot de passe**: `esprit18`

5. Si le test réussit, cliquer sur **"OK"**

## Vérification de la configuration

### Test 1: Vérifier l'utilisateur créé

```sql
-- Se connecter en tant que SYSTEM
SELECT username, account_status, created
FROM dba_users
WHERE username = 'SOUMAYA';
```

Résultat attendu:

```
USERNAME    ACCOUNT_STATUS    CREATED
SOUMAYA     OPEN             26-OCT-2025
```

### Test 2: Vérifier les privilèges

```sql
-- Se connecter en tant que SYSTEM
SELECT privilege
FROM dba_sys_privs
WHERE grantee = 'SOUMAYA'
ORDER BY privilege;
```

### Test 3: Vérifier les tables créées

```sql
-- Se connecter en tant que SOUMAYA
SELECT table_name
FROM user_tables
ORDER BY table_name;
```

Résultat attendu:

```
TABLE_NAME
ACTIVITES
EMPLOYES
EVENEMENT
FOURNISSEUR
INSCRIPTION
PLANNING
```

### Test 4: Tester une insertion

```sql
-- Se connecter en tant que SOUMAYA
INSERT INTO EMPLOYES (NOM, PRENOM, POSTE, CONTRAT, SALAIRE)
VALUES ('Test', 'Utilisateur', 'Testeur', 'CDD', 1500);

COMMIT;

SELECT * FROM EMPLOYES;
```

## Résolution des problèmes courants

### Erreur: "ORA-01017: invalid username/password"

- Vérifier que l'utilisateur `soumaya` existe
- Vérifier le mot de passe: `esprit18`
- Vérifier que le compte n'est pas verrouillé:
  ```sql
  ALTER USER soumaya ACCOUNT UNLOCK;
  ```

### Erreur: "ORA-01950: no privileges on tablespace 'USERS'"

```sql
-- Se connecter en tant que SYSTEM
ALTER USER soumaya QUOTA UNLIMITED ON USERS;
```

### Erreur: "ORA-01031: insufficient privileges"

```sql
-- Se connecter en tant que SYSTEM
GRANT CONNECT, RESOURCE TO soumaya;
GRANT CREATE SESSION TO soumaya;
```

### Le DSN n'apparaît pas dans la liste

1. Vérifier qu'Oracle Client est installé
2. Réinstaller Oracle Client si nécessaire
3. Vérifier le fichier `tnsnames.ora` (dans `ORACLE_HOME\network\admin`)

### Erreur de connexion ODBC

1. Vérifier que le service Oracle est démarré:

   - Ouvrir **Services** (Win + R → `services.msc`)
   - Chercher `OracleServiceXE` ou similaire
   - Vérifier qu'il est **Démarré**

2. Tester avec SQL\*Plus d'abord:
   ```cmd
   sqlplus soumaya/esprit18@localhost:1521/xe
   ```

## Récapitulatif des identifiants

### Utilisateur Oracle

- **Username**: `soumaya`
- **Password**: `esprit18`
- **Schéma**: `SOUMAYA`

### DSN ODBC

- **Nom DSN**: `SummerClub`
- **Type**: Système ou Utilisateur
- **Driver**: Oracle ODBC Driver
- **Server**: `localhost:1521/xe`

### Connexion dans le code C++

```cpp
db = QSqlDatabase::addDatabase("QODBC");
db.setDatabaseName("SummerClub");  // Nom du DSN
db.setUserName("soumaya");
db.setPassword("esprit18");
```

## Commandes utiles

### Changer le mot de passe

```sql
-- En tant que SYSTEM
ALTER USER soumaya IDENTIFIED BY nouveau_mot_de_passe;
```

### Réinitialiser l'utilisateur

```sql
-- En tant que SYSTEM
DROP USER soumaya CASCADE;
-- Puis réexécuter create_user_soumaya.sql
```

### Voir tous les objets de l'utilisateur

```sql
-- En tant que SOUMAYA
SELECT object_type, object_name
FROM user_objects
ORDER BY object_type, object_name;
```

## Prochaines étapes

1. ✅ Créer l'utilisateur SOUMAYA
2. ✅ Configurer le DSN ODBC
3. ✅ Créer les tables avec `create_summerclub.sql`
4. ✅ Compiler et exécuter l'application Qt
5. ✅ Tester le CRUD des événements

## Support

En cas de problème, vérifier dans cet ordre:

1. Oracle Database est démarré
2. L'utilisateur SOUMAYA existe et n'est pas verrouillé
3. Le DSN ODBC est correctement configuré
4. Les privilèges sont accordés
5. Les tables sont créées dans le bon schéma
