-- =====================================================
-- Script de réinitialisation du mot de passe expiré
-- Pour l'utilisateur WALAEDDINE
-- =====================================================

-- IMPORTANT: Exécuter ce script en tant que SYSTEM ou SYS (as SYSDBA)

-- Afficher le statut actuel de l'utilisateur
SELECT username, account_status, expiry_date, profile 
FROM dba_users 
WHERE username = 'WALAEDDINE';

-- Réinitialiser le mot de passe
ALTER USER walaeddine IDENTIFIED BY esprit18;

-- Empêcher l'expiration future du mot de passe
-- Option 1: Au niveau utilisateur (recommandé pour un utilisateur spécifique)
-- ALTER USER walaeddine PASSWORD EXPIRE NEVER;  -- Pas disponible sur toutes les versions

-- Option 2: Modifier le profil DEFAULT (recommandé)
ALTER PROFILE DEFAULT LIMIT PASSWORD_LIFE_TIME UNLIMITED;

-- Option 3: Créer un profil personnalisé sans expiration
-- CREATE PROFILE no_expiry_profile LIMIT 
--     PASSWORD_LIFE_TIME UNLIMITED
--     PASSWORD_REUSE_TIME UNLIMITED
--     PASSWORD_REUSE_MAX UNLIMITED;
-- ALTER USER walaeddine PROFILE no_expiry_profile;

-- Déverrouiller le compte si nécessaire
ALTER USER walaeddine ACCOUNT UNLOCK;

-- Vérifier que le problème est résolu
SELECT username, account_status, expiry_date, profile 
FROM dba_users 
WHERE username = 'WALAEDDINE';

-- Afficher les paramètres du profil
SELECT * FROM dba_profiles WHERE profile = 'DEFAULT' AND resource_type = 'PASSWORD';

COMMIT;

-- =====================================================
-- Instructions d'exécution
-- =====================================================
-- 
-- Méthode 1: SQL Developer
-- 1. Se connecter en tant que SYSTEM
-- 2. Ouvrir ce fichier
-- 3. Appuyer sur F5 (Exécuter le script)
--
-- Méthode 2: SQL*Plus
-- 1. Ouvrir CMD
-- 2. Taper: sqlplus system/votre_mdp@localhost:1521/xe
-- 3. Taper: @c:\Users\Raouf\Desktop\c++\fix_password_expired.sql
--
-- Méthode 3: SYSDBA (le plus simple)
-- 1. Ouvrir CMD en Administrateur
-- 2. Taper: sqlplus / as sysdba
-- 3. Taper: @c:\Users\Raouf\Desktop\c++\fix_password_expired.sql
--
-- =====================================================
-- Test de connexion après correction
-- =====================================================
-- 
-- Tester en ligne de commande:
-- sqlplus walaeddine/esprit18@localhost:1521/xe
--
-- Si la connexion réussit, le problème est résolu!
-- Vous pouvez maintenant relancer votre application Qt.
