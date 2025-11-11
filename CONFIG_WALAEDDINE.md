# Configuration pour DSN "Source_Projet2A" et utilisateur "walaeddine"

## 📋 Résumé de votre configuration

D'après votre code :

- **Nom du DSN ODBC** : `Source_Projet2A`
- **Utilisateur Oracle** : `walaeddine`
- **Mot de passe** : `esprit18`

## 🚀 Étapes de configuration

### 1️⃣ Créer le DSN ODBC "Source_Projet2A"

1. **Ouvrir l'administrateur ODBC**

   ```
   Win + R → odbcad32 → Entrée
   ```

2. **Onglet "DSN système"** → Bouton **"Ajouter"**

3. Sélectionner le pilote Oracle → **"Terminer"**

4. **Configurer le DSN** :

   - **Data Source Name** : `Source_Projet2A` ⚠️ Exactement ce nom!
   - **Description** : `Base SummerClub - Projet 2A`
   - **TNS Service Name** : Sélectionner `XE` (ou votre service)

   OU manuellement :

   - **Server** : `localhost`
   - **Port** : `1521`
   - **SID** : `xe`

5. **Tester la connexion** :
   - User: `walaeddine`
   - Password: `esprit18`

### 2️⃣ Créer l'utilisateur Oracle "walaeddine"

#### Via SQL Developer ou SQL\*Plus (en tant que SYSTEM/SYS)

```sql
-- 1. Créer l'utilisateur
CREATE USER walaeddine IDENTIFIED BY esprit18
DEFAULT TABLESPACE USERS
TEMPORARY TABLESPACE TEMP
QUOTA UNLIMITED ON USERS;

-- 2. Accorder les privilèges
GRANT CONNECT, RESOURCE TO walaeddine;
GRANT CREATE SESSION TO walaeddine;
GRANT CREATE TABLE TO walaeddine;
GRANT CREATE VIEW TO walaeddine;
GRANT CREATE SEQUENCE TO walaeddine;
GRANT CREATE TRIGGER TO walaeddine;
GRANT CREATE PROCEDURE TO walaeddine;

-- 3. Vérifier
SELECT username, account_status FROM dba_users WHERE username = 'WALAEDDINE';
```

### 3️⃣ Créer les tables dans le schéma walaeddine

Après avoir créé l'utilisateur, connectez-vous avec `walaeddine/esprit18` et exécutez :

```sql
-- Adapter le script create_summerclub.sql
@c:\Users\Raouf\Desktop\c++\create_summerclub.sql
```

## 🧪 Test rapide

Pour vérifier que tout fonctionne :

### Test 1: Oracle via SQL\*Plus

```cmd
sqlplus walaeddine/esprit18@localhost:1521/xe
```

Si ça marche → Oracle OK

### Test 2: DSN ODBC

Dans `odbcad32` :

1. Sélectionner `Source_Projet2A`
2. Cliquer "Configurer"
3. "Tester la connexion"
4. User: `walaeddine`, Pass: `esprit18`

Si succès → DSN OK

### Test 3: Application Qt

Maintenant recompilez et exécutez :

```cmd
cd c:\Users\Raouf\Desktop\c++
qmake
mingw32-make clean
mingw32-make
```

**Regardez la sortie dans la console "Output" de Qt Creator !**

Les nouveaux messages `qDebug()` afficheront :

```
=== Démarrage de l'application SummerClub ===
Tentative de connexion à la base de données...
✓ Connexion réussie à la base de données!
Affichage du message de succès...
Création de la fenêtre principale...
Fenêtre affichée, application prête.
```

OU en cas d'erreur :

```
✗ ERREUR DE CONNEXION:
   Message: [Oracle] ORA-01017: invalid username/password; logon denied
   Code: 1017
   Type: 2
```

## 🔧 Si le problème persiste

Le crash avec "exit code -1" est causé par l'échec de connexion. Avec les nouveaux messages, vous verrez exactement quelle erreur Oracle retourne.

### Problème courant : "db" n'est pas membre de la classe

Dans `connection.h`, vérifiez que vous avez :

```cpp
#include <QSqlDatabase>

class Connection
{
public:
    Connection();
    bool createconnect();

    QSqlDatabase db;  // ← Important! Membre de la classe
};
```

Si `db` n'est pas déclaré comme membre, changez dans `connection.cpp` :

```cpp
db = QSqlDatabase::addDatabase("QODBC");  // Utilise le membre
```

au lieu de :

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");  // Variable locale (mauvais)
```

## 📊 Checklist complète

- [ ] Service Oracle démarré (OracleServiceXE)
- [ ] Listener Oracle démarré (TNSListener)
- [ ] Utilisateur `walaeddine` créé dans Oracle
- [ ] Mot de passe correct : `esprit18`
- [ ] Tables créées dans le schéma WALAEDDINE
- [ ] DSN ODBC `Source_Projet2A` créé et testé
- [ ] Pilote ODBC Oracle installé
- [ ] Application recompilée après modifications
- [ ] Sortie console (Output) activée dans Qt Creator

## 🎯 Prochaines étapes

1. **Recompiler** l'application (Build → Rebuild All)
2. **Exécuter en mode Debug** pour voir les `qDebug()` dans la console
3. **Noter l'erreur exacte** affichée dans Output
4. **Résoudre** selon le message d'erreur

Les messages améliorés vous guideront précisément vers la solution !

## 📞 Messages d'erreur courants

| Message                  | Cause              | Solution                    |
| ------------------------ | ------------------ | --------------------------- |
| ORA-12154                | DSN introuvable    | Créer DSN `Source_Projet2A` |
| ORA-01017                | User/pass invalide | Créer user `walaeddine`     |
| ORA-12541                | Service arrêté     | Démarrer OracleServiceXE    |
| QODBC3 Unable to connect | DSN mal configuré  | Vérifier config DSN         |

---

**Note importante** : L'application crash AVANT même d'afficher la fenêtre car elle teste la connexion dans `main()`. C'est pour ça que vous voyez "exit code -1" immédiatement. Avec les nouveaux logs, vous verrez le problème exact dans la console Output de Qt Creator!
