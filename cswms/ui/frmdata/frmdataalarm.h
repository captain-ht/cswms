#ifndef FRMDATAALARM_H
#define FRMDATAALARM_H

#include <QWidget>
#include <QModelIndex>

class DbPage;

namespace Ui {
class frmDataAlarm;
}

class frmDataAlarm : public QWidget
{
    Q_OBJECT

public:
    explicit frmDataAlarm(QWidget *parent = 0);
    ~frmDataAlarm();

private:
    Ui::frmDataAlarm *ui;
    QString whereSql;
    QList<QString> columnNames;
    QList<int> columnWidths;
    DbPage *dbPage;

private slots:
    void initForm();
    void initData();
    void doAction();

private slots:
    void on_btnSelect_clicked();
    void dataout(int type);
    void on_btnXls_clicked();
    void on_btnPrint_clicked();
    void on_btnPdf_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    void on_cboxDeviceName_currentIndexChanged(int);
    void on_cboxNodeName_currentIndexChanged(int index);
    void on_tableView_doubleClicked(const QModelIndex &index);
};

#endif // FRMDATAALARM_H
