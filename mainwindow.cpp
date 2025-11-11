#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "evenement.h"
#include "planning.h"
#include <QPushButton>
#include <QSpinBox>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
// Pour export PDF
#include <QPrinter>
#include <QFileDialog>
#include <QTextDocument>
#include <QDateTime>
#include <QPageSize>
#include <QPageLayout>
#include <QPrinter>
#include <QPainter>
#include <QPageLayout>
#include <QPageSize>
#include <QMarginsF>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConnections();
    setupEventManagement();
    setupPlanningManagement();
    
    // Charger les employés dans le ComboBox
    chargerEmployesDansCombo();
    
    // Charger les événements au démarrage
    afficherEvenements();
    
    // Charger les plannings au démarrage
    afficherPlannings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QWidget* MainWindow::targetWidgetForBase(const QString &base)
{
    if (base == "GF")  return ui->page;
    if (base == "GI")  return ui->page_2;
    if (base == "GE")  return ui->page_3;
    if (base == "GA")  return ui->page_4;
    if (base == "GE2") return ui->page_5;
    if (base == "GP")  return ui->page_6;
    return nullptr;
}

void MainWindow::setupConnections()
{
    const QStringList bases = { "GF", "GI", "GE", "GA", "GE2", "GP" };

    for (int pageIndex = 1; pageIndex <= 6; ++pageIndex) {
        for (const QString &base : bases) {
            QString objectName = base;
            if (pageIndex > 1) objectName += "_" + QString::number(pageIndex);

            QPushButton *btn = this->findChild<QPushButton*>(objectName);
            if (!btn) continue;

            QWidget *target = targetWidgetForBase(base);
            if (!target) continue;

            connect(btn, &QPushButton::clicked, this, [this, target]() {
                if (ui->stackedWidget)
                    ui->stackedWidget->setCurrentWidget(target);
            });
        }
    }
}

void MainWindow::setupEventManagement()
{
    // Connecter les boutons de la gestion des événements
    connect(ui->ajout_event, &QPushButton::clicked, this, &MainWindow::on_ajout_event_clicked);
    connect(ui->modifier_event, &QPushButton::clicked, this, &MainWindow::on_modifier_event_clicked);
    connect(ui->supprimer_event, &QPushButton::clicked, this, &MainWindow::on_supprimer_event_clicked);
    connect(ui->anuler, &QPushButton::clicked, this, &MainWindow::on_anuler_clicked);
    connect(ui->recherche_event, &QPushButton::clicked, this, &MainWindow::on_recherche_event_clicked);
    connect(ui->certificat_15, &QPushButton::clicked, this, &MainWindow::on_certificat_15_clicked);
    connect(ui->certificat_10, &QPushButton::clicked, this, &MainWindow::on_certificat_10_clicked);
    connect(ui->pdf, &QPushButton::clicked, this, &MainWindow::on_pdf_clicked);
    connect(ui->stat, &QPushButton::clicked, this, &MainWindow::on_certificat_11_clicked);
    connect(ui->table_event, &QTableWidget::clicked, this, &MainWindow::on_table_event_clicked);
    
    // Configurer le tableau pour qu'il soit sélectionnable par ligne
    ui->table_event->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_event->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_event->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// Ajouter un événement
void MainWindow::on_ajout_event_clicked()
{
    // Récupérer les données des champs
    QString lieux = ui->lieux_event->text().trimmed();
    QDate date = ui->date_event->date();
    QString type = ui->type_event->text().trimmed();
    
    // Récupérer le responsable depuis le ComboBox
    QString responsable = ui->comboBox_responsable->currentText();
    int idEmploye = ui->comboBox_responsable->currentData().toInt();
    
    int capacite = ui->capacite->text().toInt();
    
    // Déterminer la confidentialité
    QString confidentialite;
    if (ui->confidentialit_prive->isChecked()) {
        confidentialite = "Privee";
    } else if (ui->confidentialit_public->isChecked()) {
        confidentialite = "Public";
    }
    
    // Validation des champs
    if (lieux.isEmpty() || type.isEmpty() || confidentialite.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir tous les champs obligatoires.");
        return;
    }
    
    if (idEmploye == 0) {
        QMessageBox::warning(this, "Responsable requis", "Veuillez sélectionner un responsable.");
        return;
    }
    
    if (capacite <= 0) {
        QMessageBox::warning(this, "Capacité invalide", "La capacité doit être supérieure à 0.");
        return;
    }
    
    // Créer l'événement et l'ajouter
    Evenement ev(0, lieux, date, type, confidentialite, responsable, capacite, idEmploye);
    
    if (ev.ajouter()) {
        QMessageBox::information(this, "Succès", "Événement ajouté avec succès!");
        clearEventFields();
        afficherEvenements();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'événement.");
    }
}

// Modifier un événement
void MainWindow::on_modifier_event_clicked()
{
    // Vérifier qu'un ID est renseigné
    QString idText = ui->id_event->text().trimmed();
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un événement à modifier.");
        return;
    }
    
    int id = idText.toInt();
    QString lieux = ui->lieux_event->text().trimmed();
    QDate date = ui->date_event->date();
    QString type = ui->type_event->text().trimmed();
    
    // Récupérer le responsable depuis le ComboBox
    QString responsable = ui->comboBox_responsable->currentText();
    int idEmploye = ui->comboBox_responsable->currentData().toInt();
    
    int capacite = ui->capacite->text().toInt();
    
    QString confidentialite;
    if (ui->confidentialit_prive->isChecked()) {
        confidentialite = "Privee";
    } else if (ui->confidentialit_public->isChecked()) {
        confidentialite = "Public";
    }
    
    // Validation
    if (lieux.isEmpty() || type.isEmpty() || confidentialite.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir tous les champs obligatoires.");
        return;
    }
    
    if (idEmploye == 0) {
        QMessageBox::warning(this, "Responsable requis", "Veuillez sélectionner un responsable.");
        return;
    }
    
    if (capacite <= 0) {
        QMessageBox::warning(this, "Capacité invalide", "La capacité doit être supérieure à 0.");
        return;
    }
    
    // Créer l'événement et le modifier
    Evenement ev(id, lieux, date, type, confidentialite, responsable, capacite, idEmploye);
    
    if (ev.modifier()) {
        QMessageBox::information(this, "Succès", "Événement modifié avec succès!");
        clearEventFields();
        afficherEvenements();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification de l'événement.");
    }
}

// Charger les données d'un événement sélectionné
void MainWindow::on_table_event_clicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    int row = index.row();
    loadEventData(row);
}

// Annuler/Effacer les champs
void MainWindow::on_anuler_clicked()
{
    clearEventFields();
}

// Afficher tous les événements dans le tableau
void MainWindow::afficherEvenements()
{
    Evenement ev;
    QSqlQueryModel* model = ev.afficher();
    
    // Utiliser le modèle directement si possible ou copier les données
    ui->table_event->setRowCount(model->rowCount());
    ui->table_event->setColumnCount(model->columnCount());
    
    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(model->data(model->index(i, j)).toString());
            ui->table_event->setItem(i, j, item);
        }
    }
    
    // Ajuster les colonnes
    ui->table_event->resizeColumnsToContents();
    
    delete model;
}

// Rechercher un événement (peut être connecté à un champ de recherche)
void MainWindow::rechercherEvenement()
{
    // Cette fonction peut être connectée à un QLineEdit pour la recherche
    // Pour l'instant on laisse un exemple basique
}

// Supprimer un événement
void MainWindow::supprimerEvenement()
{
    QString idText = ui->id_event_2->text().trimmed();
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un événement à supprimer.");
        return;
    }
    
    int id = idText.toInt();
    
    // Demander confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation", "Voulez-vous vraiment supprimer cet événement?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        Evenement ev;
        if (ev.supprimer(id)) {
            QMessageBox::information(this, "Succès", "Événement supprimé avec succès!");
            clearEventFields();
            afficherEvenements();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression de l'événement.");
        }
    }
}

// Effacer les champs du formulaire
void MainWindow::clearEventFields()
{
    ui->id_event->clear();
    ui->lieux_event->clear();
    ui->date_event->setDate(QDate::currentDate());
    ui->type_event->clear();
    ui->comboBox_responsable->setCurrentIndex(0);  // Réinitialiser au premier élément (vide)
    ui->capacite->clear();
    ui->confidentialit_prive->setChecked(false);
    ui->confidentialit_public->setChecked(false);
}

// Charger les données d'un événement dans le formulaire
void MainWindow::loadEventData(int row)
{
    if (row < 0 || row >= ui->table_event->rowCount()) return;
    
    // Charger les données de la ligne sélectionnée
    ui->id_event->setText(ui->table_event->item(row, 0)->text());
    ui->lieux_event->setText(ui->table_event->item(row, 1)->text());
    
    // Parser et définir la date
    QString dateStr = ui->table_event->item(row, 2)->text();
    QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
    if (!date.isValid()) {
        date = QDate::fromString(dateStr, "dd/MM/yyyy");
    }
    ui->date_event->setDate(date.isValid() ? date : QDate::currentDate());
    
    ui->type_event->setText(ui->table_event->item(row, 3)->text());
    
    // Confidentialité
    QString conf = ui->table_event->item(row, 4)->text();
    if (conf.contains("Priv", Qt::CaseInsensitive)) {
        ui->confidentialit_prive->setChecked(true);
    } else {
        ui->confidentialit_public->setChecked(true);
    }
    
    // Sélectionner le responsable dans le ComboBox
    QString responsable = ui->table_event->item(row, 5)->text();
    int index = ui->comboBox_responsable->findText(responsable);
    if (index >= 0) {
        ui->comboBox_responsable->setCurrentIndex(index);
    }
    
    ui->capacite->setText(ui->table_event->item(row, 6)->text());
}

void MainWindow::on_supprimer_event_clicked()
{
    // Vérifier qu'un ID est renseigné
    QString idText = ui->id_event->text().trimmed();
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un événement à supprimer.");
        return;
    }
    
    int id = idText.toInt();
    
    // Demander confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation", 
        "Voulez-vous vraiment supprimer cet événement?\n\nCette action est irréversible.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        Evenement ev;
        if (ev.supprimer(id)) {
            QMessageBox::information(this, "Succès", "Événement supprimé avec succès!");
            clearEventFields();
            afficherEvenements();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression de l'événement.");
        }
    }
}

// Rechercher un événement par ID
void MainWindow::on_recherche_event_clicked()
{
    // Récupérer l'ID depuis le champ de recherche
    QString idText = ui->id_event_3->text().trimmed();
    
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "ID requis", "Veuillez entrer un ID d'événement à rechercher.");
        return;
    }
    
    bool ok;
    int id = idText.toInt(&ok);
    
    if (!ok || id <= 0) {
        QMessageBox::warning(this, "ID invalide", "L'ID doit être un nombre entier positif.");
        return;
    }
    
    // Rechercher l'événement dans la base de données
    QSqlQuery query;
    query.prepare("SELECT ID_EVENT, LIEUX, DATE_EVENT, TYPE_EV, CONFIDENTIALITE, RESPONSABLE, CAPACITE "
                  "FROM EVENEMENT WHERE ID_EVENT = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la recherche : " + query.lastError().text());
        return;
    }
    
    if (query.next()) {
        // Événement trouvé - remplir le tableau avec cet événement uniquement
        ui->table_event->setRowCount(1);
        ui->table_event->setColumnCount(7);
        
        for (int j = 0; j < 7; ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(query.value(j).toString());
            ui->table_event->setItem(0, j, item);
        }
        
        // Ajuster les colonnes
        ui->table_event->resizeColumnsToContents();
        
        QMessageBox::information(this, "Trouvé", "Événement ID " + QString::number(id) + " trouvé !");
        
        // Charger automatiquement les données dans le formulaire
        loadEventData(0);
    } else {
        // Aucun événement trouvé
        QMessageBox::information(this, "Non trouvé", "Aucun événement avec l'ID " + QString::number(id) + " n'a été trouvé.");
        ui->table_event->setRowCount(0);
    }
}

// Charger les employés dans le ComboBox responsable
void MainWindow::chargerEmployesDansCombo()
{
    // Vider le ComboBox
    ui->comboBox_responsable->clear();
    
    // Ajouter un élément vide par défaut
    ui->comboBox_responsable->addItem("-- Sélectionner un responsable --", 0);
    
    // Requête pour récupérer les employés
    QSqlQuery query;
    query.prepare("SELECT ID_EMPLOYE, NOM, PRENOM FROM EMPLOYES ORDER BY NOM, PRENOM");
    
    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString nom = query.value(1).toString();
            QString prenom = query.value(2).toString();
            QString nomComplet = nom + " " + prenom;
            
            // Ajouter l'item avec le nom complet visible et l'ID stocké
            ui->comboBox_responsable->addItem(nomComplet, id);
        }
    } else {
        qDebug() << "Erreur chargement employés:" << query.lastError().text();
        QMessageBox::warning(this, "Erreur", "Impossible de charger la liste des employés.");
    }
}

void MainWindow::on_certificat_15_clicked()
{
    ui->id_event_3->clear();
    afficherEvenements();
    // Message supprimé pour une action silencieuse
}

// Trier les événements selon le critère sélectionné dans comboBox_6
void MainWindow::on_certificat_10_clicked()
{

}

// Exporter le tableau en PDF
void MainWindow::on_pdf_clicked()
{
    // Demander le fichier de sortie
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer en PDF"), QString(), tr("PDF Files (*.pdf)"));
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";

    // Obtenir la date et l'heure actuelles
    QString dateTime = QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm");

    // Construire un HTML stylé à partir du QTableWidget
    QString html;
    html += "<html><head><meta charset=\"utf-8\">";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 20px; }";
    html += "h1 { color: #1E3A8A; text-align: center; border-bottom: 3px solid #3B82F6; padding-bottom: 10px; }";
    html += "h3 { color: #64748B; text-align: center; margin-top: 5px; font-weight: normal; }";
    html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    html += "th { background: linear-gradient(to bottom, #3B82F6, #2563EB); color: white; padding: 12px; text-align: left; font-weight: bold; }";
    html += "td { padding: 10px; border-bottom: 1px solid #E5E7EB; }";
    html += "tr:nth-child(even) { background-color: #F9FAFB; }";
    html += "tr:nth-child(odd) { background-color: #FFFFFF; }";
    html += "tr:hover { background-color: #EFF6FF; }";
    html += ".footer { margin-top: 30px; text-align: center; color: #9CA3AF; font-size: 10px; border-top: 1px solid #E5E7EB; padding-top: 10px; }";
    html += ".stats { background-color: #DBEAFE; padding: 10px; border-radius: 5px; margin-bottom: 15px; text-align: center; color: #1E40AF; }";
    html += "</style>";
    html += "</head><body>";

    // En-tête du document
    html += "<h1>📋 Liste des Événements</h1>";
    html += QString("<h3>Généré le %1</h3>").arg(dateTime);

    // Statistiques
    int totalEvents = ui->table_event->rowCount();
    html += QString("<div class='stats'><strong>Total:</strong> %1 événement(s)</div>").arg(totalEvents);

    // Tableau
    html += "<table>";

    // En-têtes
    html += "<thead><tr>";
    int cols = ui->table_event->columnCount();
    for (int c = 0; c < cols; ++c) {
        QTableWidgetItem *h = ui->table_event->horizontalHeaderItem(c);
        QString htext = h ? h->text() : QString("Col%1").arg(c+1);
        html += QString("<th>%1</th>").arg(htext.toHtmlEscaped());
    }
    html += "</tr></thead>";

    // Lignes
    html += "<tbody>";
    int rows = ui->table_event->rowCount();
    for (int r = 0; r < rows; ++r) {
        html += "<tr>";
        for (int c = 0; c < cols; ++c) {
            QTableWidgetItem *it = ui->table_event->item(r, c);
            QString txt = it ? it->text() : QString();
            html += QString("<td>%1</td>").arg(txt.toHtmlEscaped());
        }
        html += "</tr>";
    }
    html += "</tbody></table>";

    // Pied de page
    html += "<div class='footer'>";
    html += "Document généré automatiquement par l'application de gestion des événements<br>";
    html += QString("© %1 - SummerClub Management System").arg(QDate::currentDate().year());
    html += "</div>";
    html += "</body></html>";

    // ✅ Impression PDF (Qt 6)
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    // ✅ Qt 6 API : utilisation de QPageSize et QPageLayout
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, tr("Export PDF"),
                             tr("Export terminé avec succès !\n\nFichier : %1\nNombre d'événements : %2")
                                 .arg(fileName).arg(totalEvents));
}

void MainWindow::on_certificat_9_clicked()
{
    QString critere = ui->comboBox_6->currentText();

    QString colonneSQL;
    QString nomColonne;

    // Déterminer la colonne SQL selon le critère
    if (critere == "Date") {
        colonneSQL = "DATE_EVENT";
        nomColonne = "date";
    } else if (critere.contains("lieux", Qt::CaseInsensitive) || critere == "lieux d evenement") {
        colonneSQL = "LIEUX";
        nomColonne = "lieu";
    } else if (critere.contains("ID", Qt::CaseInsensitive)) {
        colonneSQL = "ID_EVENT";
        nomColonne = "ID";
    } else {
        // Par défaut, trier par ID
        colonneSQL = "ID_EVENT";
        nomColonne = "ID";
    }

    // Utiliser la méthode trierPar de la classe Evenement
    Evenement ev;
    QSqlQueryModel* model = ev.trierPar(colonneSQL, "ASC");

    // Vérifier si la requête a réussi
    if (model->lastError().isValid()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors du tri : " + model->lastError().text());
        delete model;
        return;
    }

    // Remplir le tableau avec les données triées
    ui->table_event->setRowCount(model->rowCount());
    ui->table_event->setColumnCount(model->columnCount());

    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(model->data(model->index(i, j)).toString());
            ui->table_event->setItem(i, j, item);
        }
    }

    // Ajuster les colonnes
    ui->table_event->resizeColumnsToContents();

    delete model;

    // Message de confirmation
    QMessageBox::information(this, "Tri effectué",
                           QString("Événements triés par %1 (ordre croissant).").arg(nomColonne));
}

// Afficher les statistiques par mois

void MainWindow::on_certificat_11_clicked()
{
    // Requête SQL pour compter les événements par mois et année
    QSqlQuery query;
    query.prepare("SELECT "
                  "TO_CHAR(DATE_EVENT, 'YYYY') AS ANNEE, "
                  "TO_CHAR(DATE_EVENT, 'MM') AS MOIS, "
                  "TO_CHAR(DATE_EVENT, 'Month', 'NLS_DATE_LANGUAGE=FRENCH') AS NOM_MOIS, "
                  "COUNT(*) AS NOMBRE "
                  "FROM EVENEMENT "
                  "GROUP BY TO_CHAR(DATE_EVENT, 'YYYY'), TO_CHAR(DATE_EVENT, 'MM'), TO_CHAR(DATE_EVENT, 'Month', 'NLS_DATE_LANGUAGE=FRENCH') "
                  "ORDER BY ANNEE DESC, MOIS DESC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur",
                              "Erreur lors de la récupération des statistiques:\n" + query.lastError().text());
        return;
    }

    // Construire un HTML pour afficher les statistiques
    QString html;
    html += "<html><head><meta charset=\"utf-8\">";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 20px; background-color: #F3F4F6; }";
    html += "h1 { color: #1E3A8A; text-align: center; border-bottom: 3px solid #3B82F6; padding-bottom: 10px; }";
    html += "h3 { color: #64748B; text-align: center; margin-top: 5px; font-weight: normal; }";
    html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; background: white; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }";
    html += "th { background: linear-gradient(to bottom, #10B981, #059669); color: white; padding: 15px; text-align: center; font-weight: bold; font-size: 14px; }";
    html += "td { padding: 12px; border-bottom: 1px solid #E5E7EB; text-align: center; }";
    html += "tr:nth-child(even) { background-color: #F9FAFB; }";
    html += "tr:nth-child(odd) { background-color: #FFFFFF; }";
    html += "tr:hover { background-color: #ECFDF5; }";
    html += ".total-row { background: linear-gradient(to right, #DBEAFE, #BFDBFE) !important; font-weight: bold; color: #1E40AF; }";
    html += ".stats-summary { background-color: #DBEAFE; padding: 15px; border-radius: 8px; margin-bottom: 20px; }";
    html += ".stat-box { display: inline-block; margin: 10px 20px; padding: 10px 20px; background: white; border-radius: 5px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    html += ".stat-number { font-size: 28px; font-weight: bold; color: #3B82F6; }";
    html += ".stat-label { font-size: 12px; color: #64748B; text-transform: uppercase; }";
    html += "</style></head><body>";

    // En-tête
    html += "<h1>📊 Statistiques des Événements par Mois</h1>";
    html += QString("<h3>Généré le %1</h3>").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm"));

    int totalEvents = 0;
    int uniqueMonths = 0;
    QList<QPair<QString, int>> statistiques;

    // Calcul du total
    while (query.next()) {
        QString annee = query.value(0).toString();
        QString numMois = query.value(1).toString();
        QString nomMois = query.value(2).toString().trimmed();
        int nombre = query.value(3).toInt();

        totalEvents += nombre;
        uniqueMonths++;
        QString moisAnnee = nomMois + " " + annee;
        statistiques.append(qMakePair(moisAnnee, nombre));
    }

    // Tableau des statistiques
    html += "<table><thead><tr>"
            "<th>Année</th><th>Mois</th><th>Nombre d'Événements</th><th>Pourcentage</th>"
            "</tr></thead><tbody>";

    for (const auto &stat : statistiques) {
        QString moisAnnee = stat.first;
        int nombre = stat.second;
        double pourcentage = (totalEvents > 0) ? (nombre * 100.0 / totalEvents) : 0;

        html += "<tr>";
        html += QString("<td>%1</td>").arg(moisAnnee.section(' ', 1, 1));
        html += QString("<td>%1</td>").arg(moisAnnee.section(' ', 0, 0));
        html += QString("<td><strong>%1</strong></td>").arg(nombre);
        html += QString("<td>%1%</td>").arg(QString::number(pourcentage, 'f', 1));
        html += "</tr>";
    }

    html += QString("<tr class='total-row'><td colspan='2'><strong>TOTAL</strong></td>"
                    "<td><strong>%1</strong></td><td><strong>100%</strong></td></tr>")
                .arg(totalEvents);
    html += "</tbody></table>";

    // Résumé
    html += "<div class='stats-summary' style='text-align: center; margin-top: 30px;'>";
    html += QString("<div class='stat-box'><div class='stat-number'>%1</div><div class='stat-label'>Total Événements</div></div>").arg(totalEvents);
    html += QString("<div class='stat-box'><div class='stat-number'>%1</div><div class='stat-label'>Mois Actifs</div></div>").arg(uniqueMonths);
    double moyenne = (uniqueMonths > 0) ? (totalEvents * 1.0 / uniqueMonths) : 0;
    html += QString("<div class='stat-box'><div class='stat-number'>%1</div><div class='stat-label'>Moyenne/Mois</div></div>")
                .arg(QString::number(moyenne, 'f', 1));
    html += "</div></body></html>";

    // Affichage
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Statistiques par Mois");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(html);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);

    int ret = msgBox.exec();

    // Export PDF si demandé
    if (ret == QMessageBox::Save) {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Enregistrer les statistiques en PDF"),
                                                        "statistiques_evenements.pdf",
                                                        tr("PDF Files (*.pdf)"));

        if (!fileName.isEmpty()) {
            if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
                fileName += ".pdf";

            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            printer.setPageSize(QPageSize(QPageSize::A4));
            printer.setPageOrientation(QPageLayout::Portrait);
            printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

            QTextDocument doc;
            doc.setHtml(html);
            doc.print(&printer);

            QMessageBox::information(this, "Export PDF",
                                     QString("Statistiques exportées avec succès !\n\nFichier : %1").arg(fileName));
        }
    }
}


// ========== GESTION DU PLANNING ==========

void MainWindow::setupPlanningManagement()
{
    // Créer un QSpinBox pour la saisie de la salle et le placer à la même position que lcdNumber
    salleSpinBox = new QSpinBox(ui->lcdNumber->parentWidget());
    salleSpinBox->setGeometry(ui->lcdNumber->geometry());
    salleSpinBox->setMinimum(1);
    salleSpinBox->setMaximum(9999);
    salleSpinBox->setValue(1);
    salleSpinBox->setStyleSheet("QSpinBox { font-family: Arial; font-size: 14px; color: white; background-color: #0A0A2A; border: 2px solid #1E1E5A; border-radius: 5px; padding: 5px; }");
    
    // Synchroniser lcdNumber avec salleSpinBox
    connect(salleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        ui->lcdNumber->display(value);
    });
    
    // Masquer le lcdNumber et afficher le spinBox
    ui->lcdNumber->setVisible(false);
    salleSpinBox->setVisible(true);
    
    // Connecter les boutons de la gestion du planning
    connect(ui->pushButton_145, &QPushButton::clicked, this, &MainWindow::on_pushButton_145_clicked);
    connect(ui->pushButton_151, &QPushButton::clicked, this, &MainWindow::on_pushButton_151_clicked);
    connect(ui->pushButton_152, &QPushButton::clicked, this, &MainWindow::on_pushButton_152_clicked);
    connect(ui->pushButton_153, &QPushButton::clicked, this, &MainWindow::on_pushButton_153_clicked);
    connect(ui->tableWidget_3, &QTableWidget::clicked, this, &MainWindow::on_tableWidget_3_clicked);
    
    // Configurer le tableau pour qu'il soit sélectionnable par ligne
    ui->tableWidget_3->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_3->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// Ajouter un planning
void MainWindow::on_pushButton_145_clicked()
{
    // Récupérer les données des champs
    int salle = salleSpinBox->value();  // Utiliser salleSpinBox au lieu de lcdNumber
    QDate date = ui->dateTimeEdit_16->date();
    QTime heureDebut = ui->timeEdit_3->time();
    QTime heureFin = ui->timeEdit_2->time();
    QString typeActivite = ui->l2_23->text().trimmed();
    
    // Déterminer le statut
    QString statut;
    if (ui->radioButton_3->isChecked()) {
        statut = "Confirme";
    } else if (ui->radioButton_4->isChecked()) {
        statut = "Non Confirme";
    }
    
    // Validation des champs
    if (salle <= 0) {
        QMessageBox::warning(this, "Salle invalide", "Veuillez saisir un numéro de salle valide.");
        return;
    }
    
    if (typeActivite.isEmpty()) {
        QMessageBox::warning(this, "Type activité requis", "Veuillez saisir le type d'activité.");
        return;
    }
    
    if (statut.isEmpty()) {
        QMessageBox::warning(this, "Statut requis", "Veuillez sélectionner un statut.");
        return;
    }
    
    if (heureDebut >= heureFin) {
        QMessageBox::warning(this, "Horaires invalides", "L'heure de début doit être avant l'heure de fin.");
        return;
    }
    
    // Créer le planning et l'ajouter
    Planning p(salle, date, heureDebut, heureFin, statut, typeActivite);
    
    if (p.ajouter()) {
        QMessageBox::information(this, "Succès", "Planning ajouté avec succès!");
        clearPlanningFields();
        afficherPlannings();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du planning.");
    }
}

// Modifier un planning
void MainWindow::on_pushButton_151_clicked()
{
    // Vérifier qu'un ID est renseigné
    QString idText = ui->lineEdit_46->text().trimmed();
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un planning à modifier.");
        return;
    }
    
    int id = idText.toInt();
    int salle = salleSpinBox->value();  // Utiliser salleSpinBox au lieu de lcdNumber
    QDate date = ui->dateTimeEdit_16->date();
    QTime heureDebut = ui->timeEdit_3->time();
    QTime heureFin = ui->timeEdit_2->time();
    QString typeActivite = ui->l2_23->text().trimmed();
    
    // Déterminer le statut
    QString statut;
    if (ui->radioButton_3->isChecked()) {
        statut = "Confirme";
    } else if (ui->radioButton_4->isChecked()) {
        statut = "Non Confirme";
    }
    
    // Validation
    if (salle <= 0) {
        QMessageBox::warning(this, "Salle invalide", "Veuillez saisir un numéro de salle valide.");
        return;
    }
    
    if (typeActivite.isEmpty()) {
        QMessageBox::warning(this, "Type activité requis", "Veuillez saisir le type d'activité.");
        return;
    }
    
    if (statut.isEmpty()) {
        QMessageBox::warning(this, "Statut requis", "Veuillez sélectionner un statut.");
        return;
    }
    
    if (heureDebut >= heureFin) {
        QMessageBox::warning(this, "Horaires invalides", "L'heure de début doit être avant l'heure de fin.");
        return;
    }
    
    // Créer le planning et le modifier
    Planning p;
    p.setIdPlanning(id);
    p.setSalle(salle);
    p.setDate(date);
    p.setHeureDebut(heureDebut);
    p.setHeureFin(heureFin);
    p.setStatut(statut);
    p.setTypeActivite(typeActivite);
    
    if (p.modifier()) {
        QMessageBox::information(this, "Succès", "Planning modifié avec succès!");
        clearPlanningFields();
        afficherPlannings();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification du planning.");
    }
}

// Annuler - Vider les champs
void MainWindow::on_pushButton_152_clicked()
{
    clearPlanningFields();
}

// Afficher tous les plannings
void MainWindow::afficherPlannings()
{
    Planning p;
    QSqlQueryModel *model = p.afficher();
    
    if (!model) {
        QMessageBox::critical(this, "Erreur", "Impossible de charger les plannings.");
        return;
    }
    
    // Vider le tableau
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->setColumnCount(model->columnCount());
    
    // Définir les en-têtes
    QStringList headers;
    for (int i = 0; i < model->columnCount(); ++i) {
        headers << model->headerData(i, Qt::Horizontal).toString();
    }
    ui->tableWidget_3->setHorizontalHeaderLabels(headers);
    
    // Remplir le tableau
    for (int row = 0; row < model->rowCount(); ++row) {
        ui->tableWidget_3->insertRow(row);
        for (int col = 0; col < model->columnCount(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(model->data(model->index(row, col)).toString());
            ui->tableWidget_3->setItem(row, col, item);
        }
    }
    
    ui->tableWidget_3->resizeColumnsToContents();
    delete model;
}

// Charger les données d'un planning sélectionné
void MainWindow::on_tableWidget_3_clicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    int row = index.row();
    loadPlanningData(row);
}

// Charger les données d'une ligne du tableau dans le formulaire
void MainWindow::loadPlanningData(int row)
{
    // ID Planning (colonne 0)
    QString idPlanning = ui->tableWidget_3->item(row, 0) ? ui->tableWidget_3->item(row, 0)->text() : "";
    ui->lineEdit_46->setText(idPlanning);
    
    // Salle (colonne 1)
    QString salle = ui->tableWidget_3->item(row, 1) ? ui->tableWidget_3->item(row, 1)->text() : "0";
    salleSpinBox->setValue(salle.toInt());  // Utiliser salleSpinBox au lieu de lcdNumber
    ui->lcdNumber->display(salle.toInt());
    
    // Date (colonne 2)
    QString dateStr = ui->tableWidget_3->item(row, 2) ? ui->tableWidget_3->item(row, 2)->text() : "";
    QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
    if (!date.isValid()) {
        date = QDate::fromString(dateStr, "dd/MM/yyyy");
    }
    ui->dateTimeEdit_16->setDate(date);
    
    // Heure début (colonne 3)
    QString heureDebutStr = ui->tableWidget_3->item(row, 3) ? ui->tableWidget_3->item(row, 3)->text() : "";
    QTime heureDebut = QTime::fromString(heureDebutStr, "HH:mm:ss");
    if (!heureDebut.isValid()) {
        heureDebut = QTime::fromString(heureDebutStr, "HH:mm");
    }
    ui->timeEdit_3->setTime(heureDebut);
    
    // Heure fin (colonne 4)
    QString heureFinStr = ui->tableWidget_3->item(row, 4) ? ui->tableWidget_3->item(row, 4)->text() : "";
    QTime heureFin = QTime::fromString(heureFinStr, "HH:mm:ss");
    if (!heureFin.isValid()) {
        heureFin = QTime::fromString(heureFinStr, "HH:mm");
    }
    ui->timeEdit_2->setTime(heureFin);
    
    // Statut (colonne 5)
    QString statut = ui->tableWidget_3->item(row, 5) ? ui->tableWidget_3->item(row, 5)->text() : "";
    if (statut.contains("Confirme", Qt::CaseInsensitive) && !statut.contains("Non", Qt::CaseInsensitive)) {
        ui->radioButton_3->setChecked(true);
    } else {
        ui->radioButton_4->setChecked(true);
    }
    
    // Type activité (colonne 6)
    QString typeActivite = ui->tableWidget_3->item(row, 6) ? ui->tableWidget_3->item(row, 6)->text() : "";
    ui->l2_23->setText(typeActivite);
}

// Vider les champs du formulaire planning
void MainWindow::clearPlanningFields()
{
    ui->lineEdit_46->clear();
    salleSpinBox->setValue(1);          // Utiliser salleSpinBox au lieu de lcdNumber
    ui->lcdNumber->display(1);
    ui->dateTimeEdit_16->setDate(QDate::currentDate());
    ui->timeEdit_3->setTime(QTime(8, 0));
    ui->timeEdit_2->setTime(QTime(9, 0));
    ui->radioButton_3->setChecked(false);
    ui->radioButton_4->setChecked(false);
    ui->l2_23->clear();
}

// Supprimer un planning
void MainWindow::on_pushButton_153_clicked()
{
    // Récupérer l'ID depuis le champ de suppression
    QString idText = ui->lineEdit_50->text().trimmed();
    
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "ID requis", "Veuillez saisir l'ID du planning à supprimer.");
        return;
    }
    
    int id = idText.toInt();
    
    // Demander confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", 
                                  QString("Êtes-vous sûr de vouloir supprimer le planning #%1 ?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        Planning p;
        if (p.supprimer(id)) {
            QMessageBox::information(this, "Succès", "Planning supprimé avec succès!");
            ui->lineEdit_50->clear();
            afficherPlannings();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression du planning.");
        }
    }
}


