# Solution : ORA-28001 - Mot de passe expiré

## 🔍 Diagnostic

Votre application affiche l'erreur :

```
ORA-28001: the password has expired
```

Cela signifie que le mot de passe de l'utilisateur `walaeddine` a expiré dans Oracle. Par défaut, Oracle fait expirer les mots de passe après 180 jours.

## ✅ Solution rapide (3 minutes)

### Étape 1: Ouvrir SQL\*Plus en mode SYSDBA

```cmd
sqlplus / as sysdba
```

### Étape 2: Réinitialiser le mot de passe

```sql
ALTER USER walaeddine IDENTIFIED BY esprit18;
```

### Étape 3: Désactiver l'expiration (recommandé)

```sql
ALTER PROFILE DEFAULT LIMIT PASSWORD_LIFE_TIME UNLIMITED;
```

### Étape 4: Quitter

```sql
exit
```

### Étape 5: Tester

```cmd
sqlplus walaeddine/esprit18@localhost:1521/xe
```

Si vous voyez `Connected to:`, c'est bon ! ✓

## 🎯 Solution complète via script SQL

J'ai créé un script automatique pour vous : **`fix_password_expired.sql`**

### Exécution :

```cmd
sqlplus / as sysdba
@c:\Users\Raouf\Desktop\c++\fix_password_expired.sql
exit
```

## 🚀 Après la correction

1. **Fermez** l'application Qt si elle est ouverte
2. **Relancez** l'application depuis Qt Creator
3. **Résultat attendu** :

```
=== Démarrage de l'application SummerClub ===
Tentative de connexion à la base de données...
✓ Connexion réussie à la base de données!
```

Et une boîte de dialogue avec :

```
✓ Connexion réussie!

DSN: Source_Projet2A
Utilisateur: walaeddine

L'application va démarrer.
```

## 📊 Vérifications supplémentaires

### Vérifier le statut du compte :

```sql
SELECT username, account_status, expiry_date, profile
FROM dba_users
WHERE username = 'WALAEDDINE';
```

**Résultat attendu :**

```
USERNAME      ACCOUNT_STATUS   EXPIRY_DATE   PROFILE
WALAEDDINE    OPEN            (null/future)  DEFAULT
```

### Vérifier les paramètres d'expiration :

```sql
SELECT resource_name, limit
FROM dba_profiles
WHERE profile = 'DEFAULT'
AND resource_type = 'PASSWORD';
```

**Après correction, PASSWORD_LIFE_TIME devrait être UNLIMITED**

## 🔧 Méthodes alternatives

### Via SQL Developer (Interface graphique) :

1. Se connecter en **SYSTEM**
2. Dans l'arborescence : **Autres utilisateurs** → Clic droit sur **WALAEDDINE** → **Modifier l'utilisateur**
3. Onglet **"Mot de passe"** :
   - Nouveau mot de passe : `esprit18`
   - Confirmer : `esprit18`
   - Décocher "Expirer le mot de passe maintenant" si présent
4. Cliquer sur **"Appliquer"**

### Via une commande unique (copier-coller) :

```cmd
echo ALTER USER walaeddine IDENTIFIED BY esprit18; | sqlplus -s / as sysdba
```

## ❓ FAQ

### Pourquoi le mot de passe a-t-il expiré ?

Par défaut, Oracle fait expirer les mots de passe après 180 jours pour des raisons de sécurité.

### Est-ce dangereux de désactiver l'expiration ?

Pour un environnement de développement local, c'est OK. En production, c'est déconseillé.

### Le problème peut-il revenir ?

Non, si vous avez exécuté `ALTER PROFILE DEFAULT LIMIT PASSWORD_LIFE_TIME UNLIMITED;`

### Que faire si je n'ai pas les droits SYSDBA ?

Demandez à l'administrateur système ou utilisez le compte SYSTEM avec son mot de passe.

### Puis-je changer le mot de passe ?

Oui ! Remplacez `esprit18` par ce que vous voulez, puis mettez à jour dans `connection.cpp`.

## 🎓 Pour éviter ce problème à l'avenir

Lors de la création d'un nouvel utilisateur, ajoutez :

```sql
CREATE USER nouvel_user IDENTIFIED BY mot_de_passe;
ALTER PROFILE DEFAULT LIMIT PASSWORD_LIFE_TIME UNLIMITED;
ALTER USER nouvel_user PROFILE DEFAULT;
```

## 📞 Résumé des commandes essentielles

| Action                        | Commande                                                                      |
| ----------------------------- | ----------------------------------------------------------------------------- |
| Réinitialiser le mot de passe | `ALTER USER walaeddine IDENTIFIED BY esprit18;`                               |
| Désactiver l'expiration       | `ALTER PROFILE DEFAULT LIMIT PASSWORD_LIFE_TIME UNLIMITED;`                   |
| Vérifier le statut            | `SELECT username, account_status FROM dba_users WHERE username='WALAEDDINE';` |
| Tester la connexion           | `sqlplus walaeddine/esprit18@localhost:1521/xe`                               |

---

**Une fois corrigé, votre application se connectera sans problème et vous pourrez utiliser le CRUD des événements !** 🎉
