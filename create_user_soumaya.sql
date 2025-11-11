-- =====================================================
-- Script de création de l'utilisateur SOUMAYA
-- Base de données Oracle
-- =====================================================

-- IMPORTANT: Ce script doit être exécuté par un utilisateur ayant les privilèges DBA
-- (par exemple SYSTEM ou SYS en tant que SYSDBA)

-- =====================================================
-- ÉTAPE 1: Créer l'utilisateur SOUMAYA
-- =====================================================

-- Supprimer l'utilisateur s'il existe déjà (optionnel, décommenter si besoin)
-- DROP USER soumaya CASCADE;

-- Créer l'utilisateur avec mot de passe
CREATE USER soumaya IDENTIFIED BY esprit18
DEFAULT TABLESPACE USERS
TEMPORARY TABLESPACE TEMP
QUOTA UNLIMITED ON USERS;

-- =====================================================
-- ÉTAPE 2: Accorder les privilèges de connexion
-- =====================================================

-- Privilège de connexion à la base
GRANT CONNECT TO soumaya;

-- Privilège de création de session
GRANT CREATE SESSION TO soumaya;

-- =====================================================
-- ÉTAPE 3: Accorder les privilèges de création d'objets
-- =====================================================

-- Privilèges pour créer des objets dans son schéma
GRANT CREATE TABLE TO soumaya;
GRANT CREATE VIEW TO soumaya;
GRANT CREATE SEQUENCE TO soumaya;
GRANT CREATE TRIGGER TO soumaya;
GRANT CREATE PROCEDURE TO soumaya;
GRANT CREATE SYNONYM TO soumaya;

-- =====================================================
-- ÉTAPE 4: Privilèges pour manipuler les données
-- =====================================================

-- Ces privilèges sont automatiquement accordés sur les objets créés par l'utilisateur
-- mais on peut les spécifier explicitement si nécessaire
GRANT SELECT ANY TABLE TO soumaya;
GRANT INSERT ANY TABLE TO soumaya;
GRANT UPDATE ANY TABLE TO soumaya;
GRANT DELETE ANY TABLE TO soumaya;

-- Note: Les privilèges "ANY TABLE" donnent accès à toutes les tables
-- Si vous voulez restreindre, créez d'abord les tables puis accordez des privilèges spécifiques

-- =====================================================
-- ÉTAPE 5: Privilèges supplémentaires (optionnels)
-- =====================================================

-- Pour déboguer et analyser
GRANT CREATE ANY INDEX TO soumaya;
GRANT ALTER ANY TABLE TO soumaya;
GRANT DROP ANY TABLE TO soumaya;

-- =====================================================
-- ÉTAPE 6: Vérification
-- =====================================================

-- Pour vérifier que l'utilisateur a été créé
SELECT username, account_status, default_tablespace, created 
FROM dba_users 
WHERE username = 'SOUMAYA';

-- Pour vérifier les privilèges accordés
SELECT * FROM dba_sys_privs WHERE grantee = 'SOUMAYA';

-- =====================================================
-- FIN DU SCRIPT
-- =====================================================

-- Après avoir exécuté ce script, vous pouvez vous connecter avec:
-- Username: soumaya
-- Password: esprit18

COMMIT;
