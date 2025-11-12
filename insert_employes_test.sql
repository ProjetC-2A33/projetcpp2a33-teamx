-- Script pour insérer des employés de test dans la table EMPLOYES
-- Exécuter ce script pour avoir des responsables à sélectionner dans le ComboBox

-- Vérifier si des employés existent déjà
-- SELECT COUNT(*) FROM EMPLOYES;

-- Insérer quelques employés de test
INSERT INTO EMPLOYES (NOM, PRENOM, POSTE, CONTRAT, SALAIRE) 
VALUES ('Dupont', 'Jean', 'Responsable', 'CDI', 2500);

INSERT INTO EMPLOYES (NOM, PRENOM, POSTE, CONTRAT, SALAIRE) 
VALUES ('Martin', 'Sophie', 'Coordinatrice', 'CDI', 2300);

INSERT INTO EMPLOYES (NOM, PRENOM, POSTE, CONTRAT, SALAIRE) 
VALUES ('Bernard', 'Pierre', 'Animateur', 'CDD', 1800);

INSERT INTO EMPLOYES (NOM, PRENOM, POSTE, CONTRAT, SALAIRE) 
VALUES ('Dubois', 'Marie', 'Responsable Activités', 'CDI', 2400);

INSERT INTO EMPLOYES (NOM, PRENOM, POSTE, CONTRAT, SALAIRE) 
VALUES ('Thomas', 'Laurent', 'Chef de Projet', 'CDI', 2800);

-- Valider les insertions
COMMIT;

-- Vérifier les employés insérés
SELECT ID_EMPLOYE, NOM, PRENOM, POSTE FROM EMPLOYES ORDER BY NOM, PRENOM;
