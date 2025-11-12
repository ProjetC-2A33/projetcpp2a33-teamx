# 🚀 Guide Rapide - Résolution "Database is not open"

## ❌ Problème

L'application affiche: **"Database is not open - Connection failed"**

## ✅ Solution en 3 étapes

---

### 📍 ÉTAPE 1: Créer le DSN ODBC "SummerClub"

1. Appuyer sur **Win + R**
2. Taper `odbcad32` et Entrée
3. Onglet **"DSN système"** → Cliquer **"Ajouter"**
4. Sélectionner **"Oracle in OraClient..."** → **"Terminer"**
5. Remplir:
   ```
   Nom: SummerClub
   TNS Service: XE (ou sélectionner dans la liste)
   ```
   OU manuellement:
   ```
   Serveur: localhost
   Port: 1521
   SID: xe
   ```
6. **"Tester la connexion"**:
   - User: `soumaya`
   - Password: `esprit18`
7. Si erreur "invalid username" → passer à l'ÉTAPE 2
8. Si succès → **"OK"** pour sauvegarder

---

### 👤 ÉTAPE 2: Créer l'utilisateur Oracle "soumaya"

#### Via SQL Developer:

1. Se connecter en **SYSTEM** (ou SYS as SYSDBA)

   ```
   User: system
   Password: (votre mot de passe admin)
   Hostname: localhost
   Port: 1521
   SID: xe
   ```

2. Ouvrir `create_user_soumaya.sql`

3. Appuyer sur **F5** (Exécuter le script)

4. Créer une nouvelle connexion:

   ```
   User: soumaya
   Password: esprit18
   Hostname: localhost
   Port: 1521
   SID: xe
   ```

5. Ouvrir `create_summerclub.sql` et exécuter (F5)

#### Via ligne de commande (CMD):

```cmd
sqlplus system/votre_mdp_admin@localhost:1521/xe

@c:\Users\Raouf\Desktop\c++\create_user_soumaya.sql

exit

sqlplus soumaya/esprit18@localhost:1521/xe

@c:\Users\Raouf\Desktop\c++\create_summerclub.sql

exit
```

---

### 🔧 ÉTAPE 3: Démarrer Oracle

1. **Win + R** → `services.msc` → Entrée

2. Chercher et **démarrer** ces services:

   - `OracleServiceXE`
   - `OracleOraDB...TNSListener`

3. Clic droit → **"Démarrer"**

---

## 🧪 Test final

Après les 3 étapes:

1. **Recompiler l'application**:

   ```cmd
   cd c:\Users\Raouf\Desktop\c++
   qmake
   mingw32-make
   ```

2. **Exécuter**:

   ```cmd
   .\release\Atelier_Connexion.exe
   ```

3. **Résultat attendu**:
   ```
   ✓ "Base de données connectée"
   "Connexion réussie à la base de données SummerClub!"
   ```

---

## 🆘 Si ça ne marche toujours pas

Le nouveau code affichera maintenant un **message d'erreur détaillé** avec:

- Le code d'erreur Oracle exact
- Des instructions précises selon l'erreur

**Erreurs communes:**

| Code      | Signification      | Solution        |
| --------- | ------------------ | --------------- |
| ORA-12154 | DSN introuvable    | Refaire ÉTAPE 1 |
| ORA-01017 | User/pass invalide | Refaire ÉTAPE 2 |
| ORA-12541 | Service arrêté     | Refaire ÉTAPE 3 |

---

## 📂 Fichiers créés

✅ `create_user_soumaya.sql` - Créer l'utilisateur  
✅ `create_summerclub.sql` - Créer les tables  
✅ `INSTRUCTIONS_CREATION_USER.md` - Guide détaillé  
✅ `TROUBLESHOOTING.md` - Résolution problèmes

---

## 💡 Astuce

Pour vérifier rapidement si Oracle fonctionne:

```cmd
sqlplus soumaya/esprit18@localhost:1521/xe
```

Si ça marche → Oracle OK, problème = DSN ODBC  
Si ça ne marche pas → Oracle problème ou user inexistant

---

**Note**: Les modifications du code (`main.cpp` et `connection.cpp`) affichent maintenant des erreurs détaillées pour vous guider!
