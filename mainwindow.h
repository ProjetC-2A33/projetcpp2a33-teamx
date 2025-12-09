#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDialog>
#include <QVBoxLayout>
#include <QChartView>
#include <QChart>
#include <QPieSeries>
#include <QPieSlice>
#include <QLegend>
#include "employer.h"
#include "arduino.h"
#include <QItemSelection>
#include <QItemSelectionModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLineEdit;
class QDateTimeEdit;
class QPushButton;
class QTableView;
class QSortFilterProxyModel;
class QTableWidget;
class QComboBox;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool isValidName(const QString &s);
    bool isValidSalary(const QString &s);
    bool isValidId(const QString &s);

    void clearForm();

private slots:
    void on_search_2_clicked();

    void on_search_3_clicked();

    void on_search_6_clicked();

    void on_certificat_2_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

    void on_lineEdit_3_textChanged(const QString &text);
    void performSearch();

    void on_certificat_4_clicked();
    void afficherStatistiques();
    void concatRfid();

    //void on_GI_3_clicked();

// 5edmet le5er

private slots:
    void handleAjouter();
    void handleModifier();
    void handleSupprimer();
    void handleSearch(const QString &text);
    void handleSort(int index);
    void handleExportPdf();
    void handleTableWidgetSelectionChanged();
    void handleTableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Ui::MainWindow *ui;
    employer emp;
    QTimer *searchTimer;

    QByteArray data;
    arduino A;
    QString uid;

//5edmet le5er
    // Pointeurs vers widgets (trouvés dynamiquement par objectName)
    QLineEdit *le_id = nullptr;
    QLineEdit *le_nom = nullptr;
    QLineEdit *le_age = nullptr;
    QDateTimeEdit *date_age = nullptr;
    QLineEdit *le_allergie = nullptr;
    QLineEdit *le_parent = nullptr;
    QLineEdit *le_num = nullptr;
    QLineEdit *le_email = nullptr;
    QLineEdit *le_id_supprimer = nullptr;
    QPushButton *btn_ajouter = nullptr;
    QPushButton *btn_modifier = nullptr;
    QPushButton *btn_supprimer = nullptr;
    QTableView *table = nullptr;
    QTableWidget *tableWidget = nullptr; // UI contains a QTableWidget named 'tableWidget'
    QLineEdit *le_search = nullptr;      // rechercher
    QComboBox *combo_sort = nullptr;     // comboBox_9 in UI
    QPushButton *btn_export_pdf = nullptr; // acceuil_26 in UI
    QPushButton *btn_emotion = nullptr;     // emotionsdetections in UI
    QPushButton *btn_voice = nullptr;       // voicechat / voicetotext button in UI

    // Proxy model used when QTableView is present (search + sort)
    QSortFilterProxyModel *proxyModel = nullptr;

    void setupWidgetPointers();
    QObject* findAny(const QStringList& names) const;
    template <typename T> T* findAnyCast(const QStringList& names) const;
    void rafraichirTable();
    void handleEmotionDetection();
    void handleVoiceChat();
    // QR code removed

};
#endif // MAINWINDOW_H
