#ifndef AUTHPAGE_H
#define AUTHPAGE_H

#include <QDialog>
#include "mainwindow.h"
#include "employer.h"

namespace Ui {
class authpage;
}

class authpage : public QDialog
{
    Q_OBJECT

public:
    explicit authpage(QWidget *parent = nullptr);
    ~authpage();

private slots:
    void on_conn_clicked();

    void on_certificat_6_clicked();

private:
    Ui::authpage *ui;
    MainWindow w;
    employer e;
};

#endif // AUTHPAGE_H
