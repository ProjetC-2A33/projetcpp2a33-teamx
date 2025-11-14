#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "employer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    employer emp;
};
#endif // MAINWINDOW_H
