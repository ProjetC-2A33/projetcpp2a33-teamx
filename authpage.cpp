#include "authpage.h"
#include "ui_authpage.h"
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

authpage::authpage(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::authpage)
{
    ui->setupUi(this);
}

authpage::~authpage()
{
    delete ui;
}

void authpage::on_conn_clicked()
{
    int id = ui->id->text().toInt();
    QString mdp = ui->mdp->text();
    bool test = e.login(id, mdp);

    if (test) {
        MainWindow *M = new MainWindow();
        M->show();
        this->close();
    } else {
        QMessageBox::critical(this, tr("Erreur"),
                              tr("Échec de la connexion"),
                              QMessageBox::Ok);
    }
}


void authpage::on_certificat_6_clicked()
{
    // --- Fenêtre popup ---
    QDialog dialog(this);
    dialog.setWindowTitle("Entrer l'ID");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // Champ texte pour l'ID
    QLineEdit *idInput = new QLineEdit(&dialog);
    idInput->setPlaceholderText("Entrer l'ID");
    layout->addWidget(idInput);

    // Bouton valider
    QPushButton *btn = new QPushButton("Valider", &dialog);
    layout->addWidget(btn);

    // Affichage du résultat
    QLabel *resultLabel = new QLabel("", &dialog);
    layout->addWidget(resultLabel);

    // --- Lorsque le bouton est cliqué ---
    QObject::connect(btn, &QPushButton::clicked, [&]() {

        int id = idInput->text().toInt();

        // *** Lecture du hint dans ta base ***
        QSqlQuery query;
        query.prepare("SELECT indice_mdp FROM employer WHERE idemp = :id");
        query.bindValue(":id", id);

        if (!query.exec()) {
            resultLabel->setText("Erreur SQL !");
            return;
        }

        if (query.next()) {
            QString hint = query.value(0).toString();
            resultLabel->setText("Hint : " + hint);
        } else {
            resultLabel->setText("ID introuvable !");
        }
    });

    dialog.exec();
}


