#include "mainwindow.h"
#include <QApplication>
#include "connexion.h"
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Connexion à la base
    Connection c;
    bool test = c.createconnect();

    if(test) {
        QMessageBox::information(nullptr, QObject::tr("Database is open"),
                                 QObject::tr("Connection successful.\nClick Cancel to exit."),
                                 QMessageBox::Cancel);
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Database is not open"),
                              QObject::tr("Connection failed.\nClick Cancel to exit."),
                              QMessageBox::Cancel);
        return -1;
    }

    // Créer et afficher la fenêtre principale
    MainWindow w;
    w.show();

    // Gestion des traductions
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "samed_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    return a.exec();
}
