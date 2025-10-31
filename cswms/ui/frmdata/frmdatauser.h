#ifndef FRMDATAUSER_H
#define FRMDATAUSER_H

#include <QWidget>

class DbPage;

namespace Ui {
class frmDataUser;
}

class frmDataUser : public QWidget
{
    Q_OBJECT

public:
    explicit frmDataUser(QWidget *parent = 0);
    ~frmDataUser();

private:
    Ui::frmDataUser *ui;
    QString whereSql;
    QList<QString> columnNames;
    QList<int> columnWidths;
    DbPage *dbPage;

private slots:
    void initForm();
    void initData();

private slots:
    void on_btnSelect_clicked();
    void dataout(int type);
    void on_btnXls_clicked();
    void on_btnPrint_clicked();
    void on_btnPdf_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();
};

#endif // FRMDATAUSER_H
