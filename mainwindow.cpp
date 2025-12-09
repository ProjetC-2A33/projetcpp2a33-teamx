#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QColor>
#include <QPalette>
#include <QLabel>
#include <QChartView>
#include <QChart>
#include <QPieSeries>
#include <QPieSlice>
#include <QLegend>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <numeric>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QTableWidget>
#include <QComboBox>
#include <QDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QDateTime>
#include <QLocale>
#include <QPrinter>
#include <QDateTimeEdit>
#include <QFileDialog>
#include <QDir>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include <QDate>
#include <QMessageBox>
#include <QSqlQuery>
#include "enfant.h"
#include "happysad.h"
#include <QVBoxLayout>
#include "voicetotextdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (btn_ajouter) connect(btn_ajouter, &QPushButton::clicked, this, &MainWindow::handleAjouter);
    if (btn_modifier) connect(btn_modifier, &QPushButton::clicked, this, &MainWindow::handleModifier);
    if (btn_supprimer) connect(btn_supprimer, &QPushButton::clicked, this, &MainWindow::handleSupprimer);

    // connect search/sort/export if widgets are present
    if (le_search) connect(le_search, &QLineEdit::textChanged, this, &MainWindow::handleSearch);
    if (combo_sort) connect(combo_sort, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::handleSort);
    if (btn_export_pdf) connect(btn_export_pdf, &QPushButton::clicked, this, &MainWindow::handleExportPdf);
    if (btn_emotion) connect(btn_emotion, &QPushButton::clicked, this, &MainWindow::handleEmotionDetection);
    // wire up voice / voicetotext button (reuse existing qrcode button names if present)
    btn_voice = findAnyCast<QPushButton>({"voicechat","voicetotext","btn_voice","qrcode","QR_CODE","pushButton_qrcode","btn_qrcode"});
    if (btn_voice) connect(btn_voice, &QPushButton::clicked, this, &MainWindow::handleVoiceChat);

    rafraichirTable();

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    employer emp;
    ui->tableView->setModel(emp.afficher());

    QRegularExpression rxName("^[A-Za-zÀ-ÖØ-öø-ÿ\\s]*$");
    ui->lineEdit_nom->setValidator(new QRegularExpressionValidator(rxName, this));
    ui->lineEdit_prenom->setValidator(new QRegularExpressionValidator(rxName, this));

    ui->lineEdit_typp->setValidator(new QRegularExpressionValidator(rxName, this));
    ui->typc->setValidator(new QRegularExpressionValidator(rxName, this));

    ui->lineEdit_slr->setValidator(new QDoubleValidator(0.0, 999999.99, 2, this));

    ui->lineEdit_idemp->setValidator(new QIntValidator(1, 999999, this));
    // lineEdit_3 peut accepter lettres et nombres pour la recherche

    // Configuration de la recherche en temps réel
    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300); // Délai de 300ms avant de lancer la recherche
    connect(searchTimer, &QTimer::timeout, this, &MainWindow::performSearch);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_3_textChanged);

    int ret=A.connect_arduino();

    switch (ret) {
    case 0 :
        qDebug()<<"Arduino is available and connected to : "<<A.getarduino_port_name();
        break;

    case 1 :
        qDebug()<<"Arduino is available but not connected to : "<<A.getarduino_port_name();
        break;
    case -1 :
        qDebug()<<"Arduino is not available ";
        break;
    }

    QObject::connect(A.getserial(),SIGNAL(readyRead()),this,SLOT(concatRfid()));
// 5edmet le5er
}

void MainWindow::concatRfid()
{
    // STATIC BUFFER to accumulate partial data
    static QString buffer = "";
    static QString lastTag = "";  // remember last processed RFID
    static bool processingTag = false; // prevent reentry

    // 1. Read data from Arduino
    QByteArray data = A.read_from_arduino();
    if (data.isEmpty() || processingTag)
        return;

    buffer += QString::fromUtf8(data);

    // Wait until the full tag is received
    if (!buffer.contains("\n") && !buffer.contains("\r"))
        return;

    QString rfid = buffer.trimmed();
    buffer.clear();

    // Ignore duplicate reads
    if (rfid == lastTag)
        return;

    processingTag = true; // mark as processing
    lastTag = rfid;       // store current tag

    qDebug() << "RFID received:" << rfid;

    // 2. Check if it's "Unknown User"
    if (rfid.contains("Unknown", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, "RFID", "You are not an employee.");
    } else {
        // 3. Get name and surname from your function
        employer e;
        QPair<QString, QString> res = e.getNameById(rfid);

        QString nom = res.first;
        QString prenom = res.second;

        // 4. Check result
        if (!nom.isEmpty()) {
            QMessageBox::information(
                this,
                "RFID",
                "Hello " + prenom + " " + nom + "!"
                );
            // Send "on" to Arduino only for valid employee
            A.write_to_arduino("on\n");
        } else {
            QMessageBox::warning(
                this,
                "RFID",
                "You are not an employee."
                );
        }
    }

    // Allow the same card to be processed again after 2 seconds
    QTimer::singleShot(2000, [=]() {
        lastTag = "";
        processingTag = false;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isValidName(const QString &s)
{
    static const QRegularExpression re("^[A-Za-zÀ-ÖØ-öø-ÿ\\s]+$");
    return !s.isEmpty() && re.match(s).hasMatch();
}

bool MainWindow::isValidSalary(const QString &s)
{
    bool ok;
    double val = s.toDouble(&ok);
    return ok && val >= 0;
}

bool MainWindow::isValidId(const QString &s)
{
    bool ok;
    int id = s.toInt(&ok);
    return ok && id > 0;
}

void MainWindow::clearForm()
{
    ui->lineEdit_idemp->clear();
    ui->lineEdit_nom->clear();
    ui->lineEdit_prenom->clear();
    ui->lineEdit_typp->clear();
    ui->typc->clear();
    ui->lineEdit_slr->clear();
    ui->lineEdit_mdp->clear();
    if (ui->comboBox_indice_mdp->count() > 0)
        ui->comboBox_indice_mdp->setCurrentIndex(0);
}

void MainWindow::on_search_2_clicked()
{
    QString idStr = ui->lineEdit_idemp->text().trimmed();
    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QString typeposte = ui->lineEdit_typp->text().trimmed();
    QString typecontrat = ui->typc->text().trimmed();
    QString salaireStr = ui->lineEdit_slr->text().trimmed();
    QString mdp = ui->lineEdit_mdp->text().trimmed();
    bool indiceSelected = ui->comboBox_indice_mdp->currentIndex() > 0;
    QString indiceMdp = indiceSelected ? ui->comboBox_indice_mdp->currentText().trimmed() : QString();

    if (idStr.isEmpty() || nom.isEmpty() || prenom.isEmpty() ||
        typeposte.isEmpty() || typecontrat.isEmpty() || salaireStr.isEmpty() ||
        mdp.isEmpty() || !indiceSelected)
    {
        QMessageBox::warning(this, tr("Champ manquant"),
                             tr("Tous les champs sont obligatoires."));
        return;
    }

    if (!isValidId(idStr))
    {
        QMessageBox::warning(this, tr("ID invalide"),
                             tr("L'ID doit être un nombre entier positif."));
        return;
    }
    int idemp = idStr.toInt();

    if (!isValidName(nom))
    {
        QMessageBox::warning(this, tr("Nom invalide"),
                             tr("Le <b>nom</b> ne doit contenir que des lettres et espaces."));
        return;
    }
    if (!isValidName(prenom))
    {
        QMessageBox::warning(this, tr("Prénom invalide"),
                             tr("Le <b>prénom</b> ne doit contenir que des lettres et espaces."));
        return;
    }

    if (!isValidName(typeposte))
    {
        QMessageBox::warning(this, tr("Type Poste invalide"),
                             tr("Lettres et espaces uniquement."));
        return;
    }
    if (!isValidName(typecontrat))
    {
        QMessageBox::warning(this, tr("Type Contrat invalide"),
                             tr("Lettres et espaces uniquement."));
        return;
    }

    if (!isValidSalary(salaireStr))
    {
        QMessageBox::warning(this, tr("Salaire invalide"),
                             tr("Entrez un salaire positif (ex: 2500.50)."));
        return;
    }
    float salaire = salaireStr.toFloat();

    clearForm();

    employer emp(idemp, nom, prenom, typeposte, typecontrat, salaire, mdp, indiceMdp);
    if (emp.Ajouter())
    {
        ui->tableView->setModel(emp.afficher());
        QMessageBox::information(this, tr("Succès"),
                                 tr("Employé ajouté avec succès."), QMessageBox::Ok);
    }
    else
    {

        QMessageBox::critical(this, tr("Erreur"),
                                   tr("Échec de l’ajout : %1"),
                              QMessageBox::Cancel);
    }
}


void MainWindow::on_search_3_clicked()
{
    QString idStr = ui->lineEdit_idemp->text().trimmed();
    if (idStr.isEmpty())
    {
        QMessageBox::warning(this, tr("Sélection requise"),
                             tr("Veuillez sélectionner un employé dans le tableau."));
        return;
    }

    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QString typeposte = ui->lineEdit_typp->text().trimmed();
    QString typecontrat = ui->typc->text().trimmed();
    QString salaireStr = ui->lineEdit_slr->text().trimmed();
    QString mdp = ui->lineEdit_mdp->text().trimmed();
    bool indiceSelected = ui->comboBox_indice_mdp->currentIndex() > 0;
    QString indiceMdp = indiceSelected ? ui->comboBox_indice_mdp->currentText().trimmed() : QString();

    if (nom.isEmpty() || prenom.isEmpty() || typeposte.isEmpty() ||
        typecontrat.isEmpty() || salaireStr.isEmpty() || mdp.isEmpty() || !indiceSelected)
    {
        QMessageBox::warning(this, tr("Champ manquant"),
                             tr("Tous les champs sont obligatoires."));
        return;
    }

    if (!isValidName(nom) || !isValidName(prenom) ||
        !isValidName(typeposte) || !isValidName(typecontrat))
    {
        QMessageBox::warning(this, tr("Données invalides"),
                             tr("Vérifiez les champs texte (lettres et espaces uniquement)."));
        return;
    }

    if (!isValidSalary(salaireStr))
    {
        QMessageBox::warning(this, tr("Salaire invalide"),
                             tr("Entrez un montant valide."));
        return;
    }

    int idemp = idStr.toInt();
    float salaire = salaireStr.toFloat();

    employer emp;
    emp.setidemp(idemp);
    emp.setnom(nom);
    emp.setprenom(prenom);
    emp.settypeposte(typeposte);
    emp.settypecontrat(typecontrat);
    emp.setsalaire(salaire);
    emp.setmdp(mdp);
    emp.setindice_mdp(indiceMdp);

    if (emp.modifier())
    {
        ui->tableView->setModel(emp.afficher());
        clearForm();
        QMessageBox::information(this, tr("Succès"),
                                 tr("Employé modifié avec succès."), QMessageBox::Ok);
    }
    else
    {

        QMessageBox::critical(this, tr("Erreur"),
                             tr("Échec de la modification : %1"),
                              QMessageBox::Cancel);
    }
}


void MainWindow::on_search_6_clicked()
{
    QString searchTerm = ui->lineEdit_3->text().trimmed();

    if (searchTerm.isEmpty())
    {
        QMessageBox::warning(this, tr("Champ manquant"),
                             tr("Veuillez saisir une lettre ou un numéro à rechercher."));
        return;
    }

    employer emp;
    QSqlQueryModel* model = emp.chercherParLettreOuNumero(searchTerm);

    if (model->rowCount() == 0)
    {
        QMessageBox::information(this, tr("Aucun résultat"),
                                 tr("Aucun employé trouvé pour '%1'.").arg(searchTerm));
    }

    ui->tableView->setModel(model);
}

void MainWindow::on_lineEdit_3_textChanged(const QString &text)
{
    Q_UNUSED(text);
    // Redémarrer le timer à chaque changement de texte
    searchTimer->stop();
    searchTimer->start();
}

void MainWindow::performSearch()
{
    QString searchTerm = ui->lineEdit_3->text().trimmed();

    if (searchTerm.isEmpty())
    {
        // Si le champ est vide, afficher tous les employés
        employer emp;
        ui->tableView->setModel(emp.afficher());
        return;
    }

    employer emp;
    QSqlQueryModel* model = emp.chercherParLettreOuNumero(searchTerm);
    ui->tableView->setModel(model);
}

void MainWindow::on_certificat_4_clicked()
{
    afficherStatistiques();
}

void MainWindow::afficherStatistiques()
{
    employer emp;
    QMap<QString, int> statistiques = emp.getStatistiquesParTypeContrat();

    if (statistiques.isEmpty()) {
        QMessageBox::information(this, tr("Aucune donnée"),
                                 tr("Aucune statistique disponible."));
        return;
    }

    // Calculer le total
    int total = 0;
    for (auto it = statistiques.begin(); it != statistiques.end(); ++it) {
        total += it.value();
    }

    if (total == 0) {
        QMessageBox::information(this, tr("Aucune donnée"),
                                 tr("Aucun employé trouvé."));
        return;
    }

    // Créer la série de données pour le diagramme circulaire
    QPieSeries *series = new QPieSeries();
    
    // Couleurs pour les différentes tranches
    QList<QColor> colors = {
        QColor("#FF6384"), QColor("#36A2EB"), QColor("#FFCE56"),
        QColor("#4BC0C0"), QColor("#9966FF"), QColor("#FF9F40"),
        QColor("#FF6384"), QColor("#C9CBCF")
    };

    int colorIndex = 0;
    for (auto it = statistiques.begin(); it != statistiques.end(); ++it) {
        QString typeContrat = it.key();
        int nombre = it.value();
        double pourcentage = (nombre * 100.0) / total;
        
        QPieSlice *slice = series->append(QString("%1\n%2 (%3%)")
                                           .arg(typeContrat)
                                           .arg(nombre)
                                           .arg(QString::number(pourcentage, 'f', 1)),
                                           nombre);
        
        // Assigner une couleur
        slice->setColor(colors[colorIndex % colors.size()]);
        colorIndex++;
        
        // Rendre la tranche cliquable et avec effet de survol
        slice->setLabelVisible(true);
        slice->setExploded(false);
    }

    // Créer le graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Statistiques des employés par type de contrat"));
    chart->setTitleFont(QFont("Arial", 16, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setFont(QFont("Arial", 10));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Créer la vue du graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(800, 600);

    // Créer la fenêtre de dialogue
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Statistiques par type de contrat"));
    dialog->setMinimumSize(850, 650);
    
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    
    // Ajouter un label avec le total
    QLabel *labelTotal = new QLabel(tr("Total des employés: %1").arg(total), dialog);
    labelTotal->setAlignment(Qt::AlignCenter);
    labelTotal->setFont(QFont("Arial", 12, QFont::Bold));
    layout->addWidget(labelTotal);

    dialog->exec();
    
    // Nettoyer la mémoire
    delete dialog;
}

void MainWindow::on_certificat_2_clicked()
{
    QSqlQuery query;
    if (!query.exec("SELECT idemp, nom, prenom, typeposte, typecontrat, salaire, mdp, indice_mdp "
                    "FROM employer ORDER BY idemp"))
    {
        QMessageBox::critical(this, tr("Erreur base de données"),
                              tr("Impossible de récupérer les données : %1")
                                  .arg(query.lastError().text()));
        return;
    }

    QStringList headers = {
        tr("ID"),
        tr("Nom"),
        tr("Prénom"),
        tr("Type Poste"),
        tr("Type Contrat"),
        tr("Salaire"),
        tr("MDP"),
        tr("Indice MDP")
    };

    QVector<QVector<QString>> data;
    while (query.next())
    {
        QVector<QString> row;
        row.reserve(headers.size());
        row << query.value(0).toString()
            << query.value(1).toString()
            << query.value(2).toString()
            << query.value(3).toString()
            << query.value(4).toString()
            << QString::number(query.value(5).toDouble(), 'f', 2)
            << query.value(6).toString()
            << query.value(7).toString();
        data.append(row);
    }

    if (data.isEmpty())
    {
        QMessageBox::information(this, tr("Aucune donnée"),
                                 tr("La base de données ne contient aucun employé à exporter."));
        return;
    }

    QString defaultPath = QDir::homePath() + "/employes.pdf";
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Exporter en PDF"), defaultPath, tr("Fichiers PDF (*.pdf)"));
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
        fileName += ".pdf";

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300); // Better quality
    writer.setPageMargins(QMarginsF(30, 30, 30, 30), QPageLayout::Millimeter);

    QPainter painter(&writer);
    if (!painter.isActive())
    {
        QMessageBox::critical(this, tr("Erreur"),
                              tr("Impossible de créer le fichier PDF."));
        return;
    }

    QFont titleFont("Arial", 18, QFont::Bold);
    QFont headerFont("Arial", 11, QFont::Bold);
    QFont bodyFont("Arial", 10);
    painter.setFont(bodyFont);

    const int pageWidth = writer.width();
    const int pageHeight = writer.height();
    const int leftMargin = 100;
    const int usableWidth = pageWidth - 2 * leftMargin;
    const int bottomMargin = pageHeight - 150;

    // === Improved column width calculation ===
    QVector<int> colWidths;
    const int minColWidth = 120;     // Minimum width per column (prevents crushing)
    const int idealPadding = 30;     // Total left + right padding inside cell
    const int extraSpacePerCol = 20;

    for (int col = 0; col < headers.size(); ++col)
    {
        int maxWidth = painter.fontMetrics().horizontalAdvance(headers[col] + "  ");

        for (int row = 0; row < data.size(); ++row)
        {
            QString text = data[row][col];
            int w = painter.fontMetrics().horizontalAdvance(text);
            if (w > maxWidth) maxWidth = w;
        }

        // Add padding and enforce minimum
        int desired = maxWidth + idealPadding + extraSpacePerCol;
        colWidths.append(qMax(desired, minColWidth));
    }

    // === Smart scaling only if really needed ===
    int totalDesired = std::accumulate(colWidths.begin(), colWidths.end(), 0);

    if (totalDesired > usableWidth)
    {
        // Instead of crushing everything, allow horizontal overflow → we'll wrap text later if needed
        // Or reduce proportionally but keep minimum
        double scale = static_cast<double>(usableWidth) / totalDesired;
        scale = qMax(scale, 0.6); // Never scale below 60% (prevents unreadable text)
        for (int i = 0; i < colWidths.size(); ++i)
        {
            colWidths[i] = qMax(static_cast<int>(colWidths[i] * scale), minColWidth);
        }
    }

    // === Title & Date ===
    painter.setFont(titleFont);
    painter.drawText(QRect(0, 100, pageWidth, 100), Qt::AlignCenter, tr("Liste des employés"));

    painter.setFont(bodyFont);
    QString dateStr = tr("Date d'export : %1")
                          .arg(QDateTime::currentDateTime().toString("dddd dd MMMM yyyy à hh:mm"));
    painter.drawText(QRect(0, 180, pageWidth, 50), Qt::AlignCenter, dateStr);

    painter.drawLine(leftMargin, 250, pageWidth - leftMargin, 250);

    // === Header drawing lambda ===
    int y = 300;
    auto drawHeader = [&]() {
        painter.setFont(headerFont);
        painter.setPen(QPen(Qt::black, 2));
        int x = leftMargin;
        const int headerHeight = 90;  // Taller header

        for (int col = 0; col < headers.size(); ++col)
        {
            QRect rect(x, y, colWidths[col], headerHeight);
            painter.fillRect(rect, QColor(230, 240, 255)); // Light blue background
            painter.drawRect(rect);
            painter.drawText(rect.adjusted(15, 0, -15, 0),
                             Qt::AlignVCenter | Qt::AlignLeft,
                             headers[col]);
            x += colWidths[col];
        }
        y += headerHeight + 20; // Space after header
        painter.setFont(bodyFont);
        painter.setPen(QPen(Qt::black, 1));
    };

    drawHeader();

    // === Body rows with better height and text wrapping fallback ===
    const int baseRowHeight = 80;
    painter.setPen(QPen(Qt::gray, 0.5));

    for (int row = 0; row < data.size(); ++row)
    {
        int rowHeight = baseRowHeight;

        // Optional: calculate required height per row if text is long
        // For now, fixed but generous height
        if (y + rowHeight > bottomMargin)
        {
            writer.newPage();
            painter.drawText(QRect(0, 100, pageWidth, 100), Qt::AlignCenter, tr("Liste des employés (suite)"));
            painter.drawLine(leftMargin, 250, pageWidth - leftMargin, 250);
            y = 300;
            drawHeader();
        }

        if (row % 2 == 1)
        {
            painter.save();
            painter.fillRect(QRect(leftMargin, y, usableWidth + 50, rowHeight),
                             QColor(248, 250, 252));
            painter.restore();
        }

        int x = leftMargin;

        for (int col = 0; col < headers.size(); ++col)
        {
            QString text = data[row][col];
            QRect cellRect(x, y, colWidths[col], rowHeight);

            painter.drawRect(cellRect);

            // Multi-line text with padding
            QRect textRect = cellRect.adjusted(15, 10, -15, -10);
            painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap, text);

            x += colWidths[col];
        }

        y += rowHeight + 10; // spacing between rows
    }

    painter.end();

    QMessageBox::information(this, tr("Succès"),
                             tr("PDF exporté avec succès !\n%1")
                                 .arg(QDir::toNativeSeparators(fileName)));
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    int row = index.row();
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    if (!model) return;

    int idemp = model->data(model->index(row, 0)).toInt();
    QString nom = model->data(model->index(row, 1)).toString();
    QString prenom = model->data(model->index(row, 2)).toString();
    QString typeposte = model->data(model->index(row, 3)).toString();
    QString typecontrat = model->data(model->index(row, 4)).toString();
    float salaire = model->data(model->index(row, 5)).toFloat();
    QString mdp = model->columnCount() > 6 ? model->data(model->index(row, 6)).toString() : QString();
    QString indiceMdp = model->columnCount() > 7 ? model->data(model->index(row, 7)).toString() : QString();

    ui->lineEdit_idemp->setText(QString::number(idemp));
    ui->lineEdit_nom->setText(nom);
    ui->lineEdit_prenom->setText(prenom);
    ui->lineEdit_typp->setText(typeposte);
    ui->typc->setText(typecontrat);
    ui->lineEdit_slr->setText(QString::number(salaire, 'f', 2));
    ui->lineEdit_mdp->setText(mdp);
    if (!indiceMdp.isEmpty())
    {
        int idx = ui->comboBox_indice_mdp->findText(indiceMdp);
        if (idx == -1)
        {
            ui->comboBox_indice_mdp->addItem(indiceMdp);
            idx = ui->comboBox_indice_mdp->count() - 1;
        }
        ui->comboBox_indice_mdp->setCurrentIndex(idx);
    }
    else if (ui->comboBox_indice_mdp->count() > 0)
    {
        ui->comboBox_indice_mdp->setCurrentIndex(0);
    }
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    if (!model) return;

    int idemp = model->data(model->index(row, 0)).toInt();
    if (idemp <= 0)
    {
        QMessageBox::warning(this, tr("Erreur"), tr("ID invalide."), QMessageBox::Ok);
        return;
    }

    auto reply = QMessageBox::question(
        this, tr("Confirmer la suppression"),
        tr("Supprimer l'employé <b>ID: %1</b> ?<br>Cette action est <u>irréversible</u>.").arg(idemp),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        employer emp;
        if (emp.Supprime(idemp))
        {
            ui->tableView->setModel(emp.afficher());
            clearForm();
            QMessageBox::information(this, tr("Supprimé"),
                                     tr("Employé supprimé avec succès."), QMessageBox::Ok);
        }
        else
        {
            QMessageBox::critical(this, tr("Échec"),
                                  tr("Impossible de supprimer l'employé."), QMessageBox::Cancel);
        }
    }
}


void MainWindow::on_pushButton_clicked()
{
    employer emp;
    ui->tableView->setModel(emp.afficher());
}


void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    employer emp;
    QSqlQueryModel* model = nullptr;

    switch (index)
    {
    case 1:
        model = emp.trierParSalaire(true);
        break;
    case 2:
        model = emp.trierParSalaire(false);
        break;
    case 3:
        model = emp.trierParNom(true);
        break;
    case 4:
        model = emp.trierParNom(false);
        break;
    case 5:
        model = emp.trierParTypeContrat(true);
        break;
    case 6:
        model = emp.trierParTypeContrat(false);
        break;
    default:
        model = emp.afficher();
        break;
    }

    ui->tableView->setModel(model);
}

//#include"test.h"
//void MainWindow::on_GI_3_clicked()
//{
//    test t;
//    t.exec();
//}


// 5dmet le5er

QObject* MainWindow::findAny(const QStringList& names) const
{
    for (const QString& n : names) {
        if (QObject* o = this->findChild<QObject*>(n)) return o;
    }
    return nullptr;
}

template <typename T>
T* MainWindow::findAnyCast(const QStringList& names) const
{
    if (QObject* o = findAny(names)) return qobject_cast<T*>(o);
    return nullptr;
}

void MainWindow::setupWidgetPointers()
{
    le_id     = findAnyCast<QLineEdit>({"lineEdit_id","le_id","idLineEdit","lineEdit_ID"});
    le_nom    = findAnyCast<QLineEdit>({"lineEdit_nom","le_nom","nomLineEdit","lineEditName"});
    le_age    = findAnyCast<QLineEdit>({"lineEdit_age","le_age","ageLineEdit"});
    date_age  = findAnyCast<QDateTimeEdit>({"lineEdit_age","dateEdit_age","dateAge"});
    le_allergie = findAnyCast<QLineEdit>({"lineEdit_allergie","le_allergie","allergieLineEdit"});
    le_parent = findAnyCast<QLineEdit>({"lineEdit_parent","le_parent","parentLineEdit"});
    le_num    = findAnyCast<QLineEdit>({"lineEdit_num","le_num","numLineEdit","lineEditPhone"});
    le_email  = findAnyCast<QLineEdit>({"lineEdit_email","le_email","emailLineEdit"});
    le_id_supprimer = findAnyCast<QLineEdit>({"lineEdit_id_supprimer","le_id_supprimer","lineEdit_id_2","lineEdit_id_2"});

    btn_ajouter = findAnyCast<QPushButton>({"btn_ajouter","pushButton_ajouter","pushButton","Ajouter"});
    btn_modifier = findAnyCast<QPushButton>({"btn_modifier","pushButton_modifier"});
    btn_supprimer = findAnyCast<QPushButton>({"btn_supprimer","pushButton_supprimer","pushButton_121","supprimer"});

    table = findAnyCast<QTableView>({"tableView","tv_enfants"});
    tableWidget = findAnyCast<QTableWidget>({"tableWidget","tableWidget_7","tableWidget_3","tableWidget_4","tableWidget_2"});

    le_search = findAnyCast<QLineEdit>({"search_bar","lineEdit_search","lineEdit"});
    combo_sort = findAnyCast<QComboBox>({"comboBox_9","comboBox_4","combo_sort"});
    btn_export_pdf = findAnyCast<QPushButton>({"acceuil_26","pushButton_export_pdf","exportPdfButton"});
    btn_emotion = findAnyCast<QPushButton>({"emotionsdetections","emotiondetection","emotionDetection","emotionsDetection"});
    btn_voice = findAnyCast<QPushButton>({"voicechat","voicetotext","btn_voice","qrcode","QR_CODE","pushButton_qrcode","btn_qrcode"});
}

void MainWindow::rafraichirTable()
{
    Enfant e;

    // If a QTableView is present, use a proxy model with multi-column filtering
    if (table) {
        QSqlQueryModel *model = e.afficher();
        // Create a proxy model subclass that matches across all columns
        class MultiColumnFilterProxyModel : public QSortFilterProxyModel {
        public:
            explicit MultiColumnFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
        protected:
            bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
            {
                if (filterRegularExpression().pattern().isEmpty()) return true;
                for (int c = 0; c < sourceModel()->columnCount(); ++c) {
                    QModelIndex idx = sourceModel()->index(source_row, c, source_parent);
                    if (idx.isValid() && idx.data().toString().contains(filterRegularExpression())) return true;
                }
                return false;
            }
        };

        // (re)create proxyModel if needed
        if (!proxyModel) proxyModel = new MultiColumnFilterProxyModel(this);
        proxyModel->setSourceModel(model);
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        // set the proxy on the view
        table->setModel(proxyModel);
        table->setStyleSheet("QTableView { color: black; } QHeaderView::section { color: black; }");

        // connect selection on table view to populate ID fields
        if (table->selectionModel())
            connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::handleTableViewSelectionChanged);
    }

    // If a QTableWidget is present, fill its rows from the DB
    if (tableWidget) {
        // make sure items appear black
        tableWidget->setStyleSheet("QTableWidget { background-color: rgb(255, 255, 255); color: black; } QTableWidget::item { color: black; } QHeaderView::section { color: black; }");
        QSqlQuery q;
        q.exec("SELECT id_enfant, nom_prenom, age, allergie_remarque, '' AS extra, nom_prenom_parents, email FROM genfant");

        tableWidget->setRowCount(0);
        int row = 0;
        while (q.next()) {
            tableWidget->insertRow(row);
            for (int col = 0; col < 7; ++col) {
                QString text = q.value(col).toString();
                QTableWidgetItem *it = new QTableWidgetItem(text);
                it->setForeground(Qt::black);
                tableWidget->setItem(row, col, it);
            }
            ++row;
        }
        tableWidget->resizeColumnsToContents();

        // connect selection on widget to populate ID fields
        connect(tableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::handleTableWidgetSelectionChanged);
    }
}

void MainWindow::handleAjouter()
{
    if (!le_id || !le_nom) { QMessageBox::warning(this, tr("Interface"),
                             tr("Champs introuvables dans l'interface.")); return; }

    int id = le_id->text().toInt();
    QString nom = le_nom->text();
    int age = 0;
    if (date_age) age = QDate::currentDate().year() - date_age->date().year();
    // fallback to reading plain text if date widget not found
    if (le_age && le_age->text().size()) age = le_age->text().toInt();
    QString allergie = le_allergie ? le_allergie->text() : QString();
    QString parent = le_parent ? le_parent->text() : QString();
    int num = le_num ? le_num->text().toInt() : 0;
    QString email = le_email ? le_email->text() : QString();

    Enfant e(id, nom, age, allergie, parent, num, email);
    if (e.ajouter()) {
        QMessageBox::information(this, tr("Succès"), tr("Enfant ajouté avec succès."));
        rafraichirTable();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec lors de l'ajout."));
    }
}

void MainWindow::handleModifier()
{
    if (!le_id) { QMessageBox::warning(this, tr("Interface"),
                             tr("Champ ID introuvable.")); return; }

    int id = le_id->text().toInt();
    QString nom = le_nom ? le_nom->text() : QString();
    int age = 0;
    if (date_age) age = QDate::currentDate().year() - date_age->date().year();
    if (le_age && le_age->text().size()) age = le_age->text().toInt();
    QString allergie = le_allergie ? le_allergie->text() : QString();
    QString parent = le_parent ? le_parent->text() : QString();
    int num = le_num ? le_num->text().toInt() : 0;
    QString email = le_email ? le_email->text() : QString();

    Enfant e(id, nom, age, allergie, parent, num, email);

    if (e.modifier(id)) {
        QMessageBox::information(this, tr("Succès"), tr("Mise à jour réussie."));
        rafraichirTable();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de la modification."));
    }
}

void MainWindow::handleSupprimer()
{
    int id = 0;
    if (le_id_supprimer && !le_id_supprimer->text().isEmpty())
        id = le_id_supprimer->text().toInt();
    else if (le_id)
        id = le_id->text().toInt();

    Enfant e;
    if (e.supprimer(id)) {
        QMessageBox::information(this, tr("Succès"), tr("Suppression effectuée."));
        rafraichirTable();
    } else {
        QMessageBox::warning(this, tr("Erreur"), tr("Suppression échouée."));
    }
}

void MainWindow::handleSearch(const QString &text)
{
    if (tableWidget) {
        for (int r = 0; r < tableWidget->rowCount(); ++r) {
            bool match = false;
            for (int c = 0; c < tableWidget->columnCount(); ++c) {
                QTableWidgetItem *it = tableWidget->item(r, c);
                if (!it) continue;
                if (it->text().contains(text, Qt::CaseInsensitive)) { match = true; break; }
            }
            tableWidget->setRowHidden(r, !match);
        }
    }

    // For QTableView with proxy model, set the proxy filter
    if (table && proxyModel) {
        QRegularExpression re(text, QRegularExpression::CaseInsensitiveOption);
        proxyModel->setFilterRegularExpression(re);
    }
}

void MainWindow::handleSort(int index)
{
    Q_UNUSED(index);
    if (!combo_sort) return;
    // first try to sort QTableView (via proxy), otherwise fallback to tableWidget
    if (table && proxyModel) {
        QString choice = combo_sort->currentText().toLower();
        int column = 0;
        if (choice.contains("age")) column = 2;
        else if (choice.contains("nom")) column = 1;
        else if (choice.contains("id")) column = 0;
        proxyModel->sort(column, Qt::AscendingOrder);
        return;
    }

    if (!tableWidget) return;
    QString choice = combo_sort->currentText().toLower();
    int column = 0;
    if (choice.contains("age")) column = 2;
    else if (choice.contains("nom")) column = 1;
    else if (choice.contains("id")) column = 0;

    tableWidget->setSortingEnabled(true);
    tableWidget->sortItems(column, Qt::AscendingOrder);
}

void MainWindow::handleExportPdf()
{
    if (!tableWidget && !table) {
        QMessageBox::warning(this, tr("Export PDF"), tr("Aucun tableau trouvé pour exporter."));
        return;
    }

    // ask the user for a file path to save
    QString defaultPath = QDir::currentPath() + QDir::separator() + "enfants_export.pdf";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer en PDF"), defaultPath, tr("PDF Files (*.pdf)"));
    if (fileName.isEmpty()) return; // user canceled
    // ensure extension
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";
    // Build HTML table to render cleanly in PDF
    QString html;
    html += "<html><head><meta charset='utf-8'/>";
    html += "<style>body{font-family: Arial, Helvetica, sans-serif; font-size:12px; color: #000;} ";
    html += "table{border-collapse: collapse; width:100%;} th, td{border:1px solid #444; padding:6px; text-align:left;} th{background:#f0f0f0;}</style></head><body>";
    html += QString("<h2>Export liste enfants - %1</h2>").arg(QLocale::system().toString(QDateTime::currentDateTime(), QLocale::ShortFormat));

    html += "<table>";

    // Headers
    if (tableWidget) {
        html += "<tr>";
        for (int c = 0; c < tableWidget->columnCount(); ++c) {
            QString h = tableWidget->horizontalHeaderItem(c) ? tableWidget->horizontalHeaderItem(c)->text() : QString();
            html += QString("<th>%1</th>").arg(h.toHtmlEscaped());
        }
        html += "</tr>";

        // Rows
        for (int r = 0; r < tableWidget->rowCount(); ++r) {
            if (tableWidget->isRowHidden(r)) continue;
            html += "<tr>";
            for (int c = 0; c < tableWidget->columnCount(); ++c) {
                QString text = tableWidget->item(r, c) ? tableWidget->item(r, c)->text() : QString();
                html += QString("<td>%1</td>").arg(text.toHtmlEscaped());
            }
            html += "</tr>";
        }
    } else if (table && table->model()) {
        QAbstractItemModel *m = table->model();
        html += "<tr>";
        for (int c = 0; c < m->columnCount(); ++c) {
            QString h = m->headerData(c, Qt::Horizontal).toString();
            html += QString("<th>%1</th>").arg(h.toHtmlEscaped());
        }
        html += "</tr>";

        for (int r = 0; r < m->rowCount(); ++r) {
            html += "<tr>";
            for (int c = 0; c < m->columnCount(); ++c) {
                QModelIndex idx = m->index(r, c);
                QString text = idx.isValid() ? idx.data().toString() : QString();
                html += QString("<td>%1</td>").arg(text.toHtmlEscaped());
            }
            html += "</tr>";
        }
    }

    html += "</table></body></html>";

    // Render HTML to PDF using QTextDocument + QPrinter
    QTextDocument doc;
    doc.setHtml(html);

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setOutputFileName(fileName);
    // Small margins
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    doc.print(&printer);

    QMessageBox::information(this, tr("Export PDF"), tr("Export terminé: %1").arg(fileName));
}

void MainWindow::handleTableWidgetSelectionChanged()
{
    if (!tableWidget) return;
    QList<QTableWidgetItem*> sel = tableWidget->selectedItems();
    if (sel.isEmpty()) return;
    // selectedItems returns items row-major; get id from first selected row col 0
    QTableWidgetItem *first = sel.first();
    int row = first->row();
    QTableWidgetItem *idItem = tableWidget->item(row, 0);
    if (!idItem) return;
    QString id = idItem->text();
    if (le_id) le_id->setText(id);
    if (le_id_supprimer) le_id_supprimer->setText(id);
}

void MainWindow::handleTableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (!table) return;
    QModelIndexList indexes = selected.indexes();
    if (indexes.isEmpty()) return;
    // get the first selected index and read column 0 id
    QModelIndex idx = indexes.first();
    QModelIndex idIndex = table->model()->index(idx.row(), 0);
    QString id = idIndex.data().toString();
    if (le_id) le_id->setText(id);
    if (le_id_supprimer) le_id_supprimer->setText(id);
}

void MainWindow::handleEmotionDetection()
{
    // Create a dialog and put the HappySad widget inside it
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Emotion Detection"));
    dlg.setModal(true);
    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    HappySad *hs = new HappySad(&dlg);
    layout->addWidget(hs);

    // Optional: give a fixed minimum size so camera UI has room
    dlg.resize(900, 600);
    dlg.exec();
}

void MainWindow::handleVoiceChat()
{
    // Show the voice-to-text dialog which will stream audio and show interim/final transcriptions
    VoiceToTextDialog dlg(this);
    dlg.exec();
}


