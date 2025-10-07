#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QWidget* MainWindow::targetWidgetForBase(const QString &base)
{
    // mappe le nom "de base" du bouton (GF, GI, GE, ...) vers la page correspondante
    if (base == "GF")  return ui->page;      // GF => page principale
    if (base == "GI")  return ui->page_2;    // GI => page_2
    if (base == "GE")  return ui->page_3;    // GE => page_3
    if (base == "GA")  return ui->page_4;    // GA => page_4
    if (base == "GE2") return ui->page_5;    // GE2 => page_5
    if (base == "GP")  return ui->page_6;    // GP => page_6

    return nullptr;
}

void MainWindow::setupConnections()
{
    // noms de base des boutons présents sur chaque page
    const QStringList bases = { "GF", "GI", "GE", "GA", "GE2", "GP" };

    // pour chaque page 1..6 on cherche les boutons nommés exactement:
    // page principale : "GF", "GI", ...
    // page 2 : "GF_2", "GI_2", ...
    // page 3 : "GF_3", ...
    for (int pageIndex = 1; pageIndex <= 6; ++pageIndex) {
        for (const QString &base : bases) {
            QString objectName = base;
            if (pageIndex > 1) objectName += "_" + QString::number(pageIndex);

            // recherche le QPushButton par son objectName dans l'arbre d'objets de la fenêtre
            QPushButton *btn = this->findChild<QPushButton*>(objectName);
            if (!btn) {
                // bouton non trouvé — passe au suivant
                continue;
            }

            // récupère la page cible à partir du nom de base (GF, GI, ...)
            QWidget *target = targetWidgetForBase(base);
            if (!target) continue;

            // connecte le bouton à la page cible (capture target par valeur)
            connect(btn, &QPushButton::clicked, this, [this, target]() {
                ui->stackedWidget->setCurrentWidget(target);
            });
        }
    }
}
