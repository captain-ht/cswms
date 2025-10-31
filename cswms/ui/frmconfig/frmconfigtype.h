#ifndef FRMCONFIGTYPE_H
#define FRMCONFIGTYPE_H

#include <QWidget>

class QSqlTableModel;

namespace Ui {
class frmConfigType;
}

class frmConfigType : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigType(QWidget *parent = 0);
    ~frmConfigType();

private:
    Ui::frmConfigType *ui;
    QSqlTableModel *model;
    QStringList columnNames;
    QList<int> columnWidths;

private slots:
    void initForm();
    void initData();

private slots:
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnDelete_clicked();
    void on_btnReturn_clicked();
    void on_btnClear_clicked();

    void dataout(int type);
    void on_btnXls_clicked();
    void on_btnPrint_clicked();
    void on_btnInput_clicked();
    void on_btnOutput_clicked();

signals:
    void deviceTypeChanged();
    void nodeTypeChanged();
    void nodeClassChanged();
    void nodeSignChanged();
};

#endif // FRMCONFIGTYPE_H
