#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots pour la gestion des événements
    void on_ajout_event_clicked();
    void on_modifier_event_clicked();
    void on_supprimer_event_clicked();
    void on_table_event_clicked(const QModelIndex &index);
    void on_anuler_clicked();
    void on_recherche_event_clicked();  // Nouveau slot pour la recherche
    void afficherEvenements();
    void rechercherEvenement();
    void supprimerEvenement();

    void on_certificat_15_clicked();
    void on_certificat_10_clicked();    // Slot pour trier les événements
    void on_pdf_clicked();               // Slot pour exporter le tableau en PDF
    void on_certificat_11_clicked();     // Slot pour afficher les statistiques par mois

    void on_certificat_9_clicked();
    
    // Slots pour la gestion des plannings
    void on_pushButton_145_clicked();    // Slot pour ajouter un planning
    void on_pushButton_151_clicked();    // Slot pour modifier un planning
    void on_pushButton_152_clicked();    // Slot pour annuler (clear fields)
    void on_pushButton_153_clicked();    // Slot pour supprimer un planning
    void on_tableWidget_3_clicked(const QModelIndex &index);  // Slot pour sélectionner un planning
    void afficherPlannings();             // Méthode pour afficher tous les plannings

private:
    Ui::MainWindow *ui;
    class QSpinBox *salleSpinBox;    // Widget pour saisir le numéro de salle

    QWidget* targetWidgetForBase(const QString &base);
    void setupConnections();
    void setupEventManagement();
    void setupPlanningManagement();  // Nouvelle méthode pour configurer la gestion du planning
    void clearEventFields();
    void clearPlanningFields();      // Nouvelle méthode pour vider les champs du planning
    void loadEventData(int row);
    void loadPlanningData(int row);  // Nouvelle méthode pour charger les données du planning
    void chargerEmployesDansCombo();  // Nouvelle méthode pour remplir le ComboBox
};

#endif // MAINWINDOW_H
