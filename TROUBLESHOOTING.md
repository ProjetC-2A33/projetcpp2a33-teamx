# Guide de résolution des problèmes de connexion Oracle

## Erreur: "Database is not open" / "Connection failed"

Cette erreur signifie que l'application Qt ne peut pas se connecter à la base de données Oracle via le DSN ODBC "SummerClub". Voici comment résoudre le problème **étape par étape**.

---

## 🔍 Diagnostic rapide

Après avoir modifié le code, l'application affichera maintenant un message détaillé avec le code d'erreur exact. Les erreurs courantes sont :

### Erreur ORA-12154: "TNS:could not resolve the connect identifier"

**Cause**: Le DSN ODBC n'existe pas ou est mal configuré.
**Solution**: Voir [Étape 1](#étape-1-créer-le-dsn-odbc-summerclub) ci-dessous.

### Erreur ORA-01017: "invalid username/password; logon denied"

**Cause**: L'utilisateur `soumaya` n'existe pas ou le mot de passe est incorrect.
**Solution**: Voir [Étape 2](#étape-2-créer-lutilisateur-oracle-soumaya) ci-dessous.

### Erreur ORA-12541: "TNS:no listener"

**Cause**: Le service Oracle n'est pas démarré.
**Solution**: Voir [Étape 3](#étape-3-démarrer-le-service-oracle) ci-dessous.

---

## ✅ Solution complète (dans l'ordre)

### Étape 1: Créer le DSN ODBC "SummerClub"

#### Méthode A: Via l'interface graphique Windows

1. **Ouvrir l'administrateur ODBC**

   - Appuyez sur `Win + R`
   - Tapez `odbcad32` et appuyez sur Entrée
   - OU: Recherchez "Sources de données ODBC" dans le menu Démarrer

2. **Choisir l'onglet**

   - Onglet **"DSN système"** (recommandé) ou "DSN utilisateur"

3. **Ajouter une nouvelle source**

   - Cliquez sur le bouton **"Ajouter..."**
   - Dans la liste, sélectionnez:
     - **"Oracle in OraClient..."** (si vous avez Oracle Client)
     - OU **"Microsoft ODBC for Oracle"**
   - Cliquez sur **"Terminer"**

4. **Configurer le DSN**

   - **Nom de la source de données**: `SummerClub` (exactement ce nom!)
   - **Description**: `Base SummerClub - Gestion événements`

   **Si vous utilisez Oracle in OraClient:**

   - **TNS Service Name**: Sélectionnez votre service (par ex: `XE`, `ORCL`)

   **Si vous configurez manuellement:**

   - **Serveur** ou **Host**: `localhost`
   - **Port**: `1521`
   - **SID** ou **Service Name**: `xe` (ou le nom de votre instance Oracle)

5. **Tester la connexion**
   - Cliquez sur **"Tester la connexion"** ou **"Test Connection"**
   - Entrez:
     - **User ID / Nom d'utilisateur**: `soumaya`
     - **Password / Mot de passe**: `esprit18`
   - Si le test échoue avec "invalid username/password" → passez à l'Étape 2
   - Si le test réussit → Cliquez sur **"OK"** pour sauvegarder

#### Méthode B: Via un fichier tnsnames.ora (Oracle Client)

Si vous avez Oracle Client installé:

1. Localisez le fichier `tnsnames.ora`:
   ```
   C:\app\oracle\product\<version>\client_1\network\admin\tnsnames.ora
   ```
2. Ajoutez cette entrée (adapter selon votre configuration):

   ```
   SUMMERCLUB =
     (DESCRIPTION =
       (ADDRESS = (PROTOCOL = TCP)(HOST = localhost)(PORT = 1521))
       (CONNECT_DATA =
         (SERVER = DEDICATED)
         (SERVICE_NAME = xe)
       )
     )
   ```

3. Ensuite, créez le DSN ODBC qui pointe vers `SUMMERCLUB`

---

### Étape 2: Créer l'utilisateur Oracle "soumaya"

#### Option A: Via SQL Developer (Recommandé)

1. **Se connecter en tant qu'administrateur**

   - Ouvrez **Oracle SQL Developer**
   - Créez une connexion:
     - **Nom**: `Admin`
     - **Utilisateur**: `system`
     - **Mot de passe**: (votre mot de passe SYSTEM)
     - **Rôle**: Laisser vide OU `SYSDBA` si vous utilisez SYS
     - **Nom d'hôte**: `localhost`
     - **Port**: `1521`
     - **SID**: `xe` (ou votre SID)
   - Cliquez sur **Tester** puis **Connexion**

2. **Exécuter le script de création**

   - Ouvrez le fichier `create_user_soumaya.sql`
   - Cliquez sur **"Exécuter le script"** (icône document avec flèche verte, ou F5)
   - Vérifiez qu'il n'y a pas d'erreurs

3. **Vérifier que l'utilisateur existe**

   ```sql
   SELECT username, account_status FROM dba_users WHERE username = 'SOUMAYA';
   ```

   Résultat attendu: `SOUMAYA | OPEN`

4. **Créer les tables**
   - Créez une nouvelle connexion avec l'utilisateur `soumaya`
   - Ouvrez et exécutez `create_summerclub.sql`

#### Option B: Via SQL\*Plus (Ligne de commande)

1. **Ouvrir une invite de commandes**

   ```cmd
   sqlplus system/votre_mot_de_passe@localhost:1521/xe
   ```

   OU si vous êtes SYS:

   ```cmd
   sqlplus sys/votre_mot_de_passe@localhost:1521/xe as sysdba
   ```

2. **Exécuter le script**

   ```sql
   @c:\Users\Raouf\Desktop\c++\create_user_soumaya.sql
   ```

3. **Se connecter avec le nouvel utilisateur**

   ```cmd
   sqlplus soumaya/esprit18@localhost:1521/xe
   ```

4. **Créer les tables**
   ```sql
   @c:\Users\Raouf\Desktop\c++\create_summerclub.sql
   ```

---

### Étape 3: Démarrer le service Oracle

#### Vérifier si Oracle est démarré:

1. **Ouvrir les Services Windows**

   - Appuyez sur `Win + R`
   - Tapez `services.msc` et Entrée

2. **Chercher les services Oracle**

   - Cherchez `OracleServiceXE` (ou `OracleServiceORCL`, etc.)
   - Et aussi `OracleOraDB...TNSListener`

3. **Démarrer les services**
   - Clic droit sur chaque service → **"Démarrer"**
   - OU: Clic droit → **"Propriétés"** → Type de démarrage: **"Automatique"**

#### Via ligne de commande (en Administrateur):

```cmd
net start OracleServiceXE
net start OracleOraDB21Home1TNSListener
```

---

## 🧪 Tests de validation

### Test 1: Vérifier la connexion Oracle (sans ODBC)

```cmd
sqlplus soumaya/esprit18@localhost:1521/xe
```

Si ça fonctionne, Oracle est OK. Le problème vient du DSN ODBC.

### Test 2: Vérifier le DSN ODBC

1. Ouvrir l'administrateur ODBC (`odbcad32`)
2. Onglet **"DSN Système"**
3. Sélectionner **"SummerClub"**
4. Cliquer sur **"Configurer"**
5. Cliquer sur **"Tester la connexion"**
6. Entrer: user=`soumaya`, pass=`esprit18`

### Test 3: Test depuis Qt

Après avoir fait les corrections ci-dessus, recompilez et exécutez l'application:

```cmd
cd c:\Users\Raouf\Desktop\c++
qmake
mingw32-make
.\release\Atelier_Connexion.exe
```

Le nouveau message d'erreur détaillé vous indiquera exactement ce qui ne va pas.

---

## 📝 Checklist complète

Cochez au fur et à mesure:

- [ ] **Oracle Database est installé**

  - Version: Oracle XE, Standard, ou Enterprise
  - Localisation: `C:\app\oracle\...`

- [ ] **Service Oracle est démarré**

  - `OracleServiceXE` (ou `OracleServiceORCL`)
  - `OracleOraDB...TNSListener`

- [ ] **Utilisateur SOUMAYA existe**

  - Vérification: `SELECT username FROM dba_users WHERE username='SOUMAYA';`
  - Mot de passe: `esprit18`
  - Privilèges: CONNECT, RESOURCE, CREATE SESSION, CREATE TABLE, etc.

- [ ] **Tables créées dans le schéma SOUMAYA**

  - Vérification: `SELECT table_name FROM user_tables;`
  - Tables attendues: EVENEMENT, EMPLOYES, FOURNISSEUR, ACTIVITES, INSCRIPTION, PLANNING

- [ ] **Driver ODBC Oracle est installé**

  - Vérification: Ouvrir `odbcad32` → Onglet "Pilotes"
  - Chercher: "Oracle in..." ou "Microsoft ODBC for Oracle"

- [ ] **DSN ODBC "SummerClub" est créé**

  - Vérification: `odbcad32` → DSN Système → Chercher "SummerClub"
  - Configuration: Host=localhost, Port=1521, SID=xe

- [ ] **Test de connexion ODBC réussit**

  - Dans la config du DSN → "Tester la connexion"
  - User=soumaya, Pass=esprit18

- [ ] **Module Qt SQL est disponible**
  - Vérification dans le .pro: `QT += sql`
  - Les DLL Qt SQL sont présentes

---

## 🔧 Problèmes spécifiques et solutions

### "Le pilote Oracle ODBC n'apparaît pas dans la liste"

**Solution:**

1. Télécharger et installer Oracle Instant Client:
   - https://www.oracle.com/database/technologies/instant-client/downloads.html
   - Choisir "Instant Client for Windows"
   - Télécharger "ODBC Package"
2. Extraire dans `C:\oracle\instantclient_21_X`

3. Ajouter au PATH:

   - Win + Pause → Paramètres système avancés
   - Variables d'environnement
   - Variable système PATH → Modifier
   - Ajouter: `C:\oracle\instantclient_21_X`

4. Redémarrer l'ordinateur

### "TNS: could not resolve service name"

Vérifier le fichier `tnsnames.ora` ou créer le DSN avec les paramètres manuels (host, port, sid).

### "ORA-12560: TNS:protocol adapter error"

Le service Oracle n'est pas démarré. Voir Étape 3.

### "Listener refused connection"

Le Listener Oracle n'est pas démarré:

```cmd
lsnrctl start
```

---

## 📞 Support supplémentaire

Si le problème persiste après toutes ces étapes:

1. **Consulter les logs Oracle**

   - `C:\app\oracle\diag\rdbms\xe\xe\trace\alert_xe.log`

2. **Consulter les logs Qt**

   - La sortie console (Output) dans Qt Creator
   - Les messages `qDebug()` ajoutés dans le code

3. **Vérifier la version d'Oracle**

   ```sql
   SELECT * FROM v$version;
   ```

4. **Tester avec un autre client**
   - SQL Developer
   - SQL\*Plus
   - DBeaver

Si la connexion fonctionne avec ces clients mais pas avec Qt, le problème vient du DSN ODBC ou du driver Qt SQL.

---

## 📚 Fichiers de référence

- `create_user_soumaya.sql` - Script de création de l'utilisateur
- `create_summerclub.sql` - Script de création des tables
- `INSTRUCTIONS_CREATION_USER.md` - Guide détaillé de création utilisateur
- `connection.cpp` - Fichier de connexion Qt (maintenant avec messages d'erreur détaillés)

---

**Bonne chance!** 🍀

Une fois la connexion établie, vous pourrez utiliser le CRUD complet des événements qui a été implémenté.
