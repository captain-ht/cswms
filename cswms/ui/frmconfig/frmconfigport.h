#ifndef FRMCONFIGPORT_H
#define FRMCONFIGPORT_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigPort;
}

class frmConfigPort : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigPort(QWidget *parent = 0);
    ~frmConfigPort();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigPort *ui;
    QSqlTableModel *model;
    QStringList columnNames;
    QList<int> columnWidths;

    QList<DbDelegate *> delegates;

private slots:
    void initForm();
    void initData();
    void changeStyle();

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
    void portNameChanged();
};

#endif // FRMCONFIGPORT_H
