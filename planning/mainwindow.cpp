#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include "planning.h"





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConnections();
    ui->tableplaning->setModel(p.afficher());//afichage
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



void MainWindow::on_ajouter_clicked()
{
    int salle = ui->salle->text().toInt();
    QDate date = QDate::fromString(ui->date->text(), "yyyy-MM-dd");
    QTime heureDebut = QTime::fromString(ui->heured->text(), "HH:mm");
    QTime heureFin = QTime::fromString(ui->heuref->text(), "HH:mm");
    QString statut = ui->statut->text();
    QString typeActivite = ui->activite->text();

    Planning p(salle, date, heureDebut, heureFin, statut, typeActivite);

    bool test=p.ajouter();
    if (test)
    {

        ui->tableplaning->setModel(p.afficher());//afichage


        QMessageBox::information(nullptr, QObject::tr("add"),
                                 QObject::tr(" successful.\n"
                                             "Click Cancel to exit."), QMessageBox::Cancel);
        ui->salle->clear();
        ui->date->clear();
        ui->heured->clear();
        ui->heuref->clear();
        ui->statut->clear();
        ui->activite->clear();



    }
    else{

        QMessageBox::critical(nullptr, QObject::tr("add"),
                              QObject::tr(" failed.\n"
                                          "Click Cancel to exit."), QMessageBox::Cancel);


    }

}


void MainWindow::on_modifier_clicked()
{
    int salle = ui->salle->text().toInt();
    QDate date = QDate::fromString(ui->date->text(), "yyyy-MM-dd");
    QTime heureDebut = QTime::fromString(ui->heured->text(), "HH:mm");
    QTime heureFin = QTime::fromString(ui->heuref->text(), "HH:mm");
    QString statut = ui->statut->text();
    QString typeActivite = ui->activite->text();

    Planning p(salle, date, heureDebut, heureFin, statut, typeActivite);
    bool test=p.modifier();
    ui->tableplaning->setModel(p.afficher());
    if(test){

        QMessageBox::information(nullptr, QObject::tr("update"),
                                 QObject::tr(" successful.\n"
                                             "Click Cancel to exit."), QMessageBox::Cancel);
        ui->salle->clear();
        ui->date->clear();
        ui->heured->clear();
        ui->heuref->clear();
        ui->statut->clear();
        ui->activite->clear();





    }else{
        QMessageBox::critical(nullptr, QObject::tr("update"),
                              QObject::tr(" failed.\n"
                                          "Click Cancel to exit."), QMessageBox::Cancel);



    }

}




void MainWindow::on_supprimer_clicked()
{

    QModelIndex index = ui->tableplaning->currentIndex();

    if (!index.isValid()) {
        QMessageBox::warning(this, "Suppression", "Veuillez sélectionner un planning à supprimer !");
        return;
    }

    int row = index.row();
    QModelIndex idIndex = ui->tableplaning->model()->index(row, 0); // colonne 0 = ID_PLANNING
    int id = ui->tableplaning->model()->data(idIndex).toInt();

    Planning p;
    bool test = p.supprimer(id);

    if (test) {
        ui->tableplaning->setModel(p.afficher());

        // Réinitialiser les champs
        ui->salle->clear();
        ui->date->clear();
        ui->heured->clear();
        ui->heuref->clear();
        ui->statut->clear();
        ui->activite->clear();

        QMessageBox::information(this, "Suppression", "Planning supprimé avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du planning !");
    }

  }


void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    if (!index.isValid())
        return; // Vérifie que la ligne cliquée est valide

    int row = index.row();
    QModelIndex in = index.sibling(row, 0); // Colonne 0 = ID_PLANNING
    QString val = ui->tableplaning->model()->data(in).toString();

    QSqlQuery qry;
    qry.prepare("SELECT SALLE, DT, HEURE_DEBUT, HEURE_FIN, STATUT, TYPE_ACTIVITE "
                "FROM PLANNING WHERE ID_PLANNING = :id");
    qry.bindValue(":id", val); // Sécurise contre l’injection SQL

    if (qry.exec() && qry.next())
    {
        ui->salle->setText(qry.value("SALLE").toString());
        ui->date->setDate(QDate::fromString(qry.value("DT").toString(), "yyyy-MM-dd"));
        ui->heured->setTime(QTime::fromString(qry.value("HEURE_DEBUT").toString(), "HH:mm:ss"));
        ui->heuref->setTime(QTime::fromString(qry.value("HEURE_FIN").toString(), "HH:mm:ss"));
        ui->statut->setText(qry.value("STATUT").toString());
        ui->activite->setText(qry.value("TYPE_ACTIVITE").toString());
    }

    else
    {
        qDebug() << "Erreur SQL :" << qry.lastError().text();
    }

}

