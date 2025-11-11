#include "mainwindow.h"
#include <QApplication>
#include "connection.h"
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Gestion des traductions (avant la connexion)
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "samed_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Connexion à la base de données
    Connection c;
    bool test = c.createconnect();

    if(test) {
        QMessageBox::information(nullptr, QObject::tr("Connexion réussie"),
                                 QObject::tr("Connexion à la base de données établie avec succès."),
                                 QMessageBox::Ok);
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Erreur de connexion"),
                              QObject::tr("Impossible de se connecter à la base de données.\n"
                                        "Vérifiez que le service Oracle est démarré."),
                              QMessageBox::Ok);
        return -1;
    }

    // Créer et afficher la fenêtre principale
    MainWindow w;
    w.show();

    return a.exec();
}
