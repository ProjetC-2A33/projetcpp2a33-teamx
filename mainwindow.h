#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget* targetWidgetForBase(const QString &base); // renvoie la page cible pour un bouton de base
    void setupConnections(); // configure toutes les connections automatiquement
};

#endif // MAINWINDOW_H
