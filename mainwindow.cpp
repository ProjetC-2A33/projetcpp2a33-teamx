#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDebug>

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
