#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}

void MainWindow::on_search_2_clicked()
{
    QString idStr = ui->lineEdit_idemp->text().trimmed();
    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QString typeposte = ui->lineEdit_typp->text().trimmed();
    QString typecontrat = ui->typc->text().trimmed();
    QString salaireStr = ui->lineEdit_slr->text().trimmed();

    if (idStr.isEmpty() || nom.isEmpty() || prenom.isEmpty() ||
        typeposte.isEmpty() || typecontrat.isEmpty() || salaireStr.isEmpty())
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

    employer emp(idemp, nom, prenom, typeposte, typecontrat, salaire);
    if (emp.Ajouter())
    {
        ui->tableView->setModel(emp.afficher());
        QMessageBox::information(this, tr("Succès"),
                                 tr("Employé ajouté avec succès."), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(this, tr("Erreur"),
                              tr("Échec de l’ajout. Vérifiez les données ou l'ID existant."), QMessageBox::Cancel);
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

    if (nom.isEmpty() || prenom.isEmpty() || typeposte.isEmpty() ||
        typecontrat.isEmpty() || salaireStr.isEmpty())
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
                              tr("Échec de la modification."), QMessageBox::Cancel);
    }
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

    ui->lineEdit_idemp->setText(QString::number(idemp));
    ui->lineEdit_nom->setText(nom);
    ui->lineEdit_prenom->setText(prenom);
    ui->lineEdit_typp->setText(typeposte);
    ui->typc->setText(typecontrat);
    ui->lineEdit_slr->setText(QString::number(salaire, 'f', 2));
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

