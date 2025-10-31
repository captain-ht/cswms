#ifndef FRMCONFIGLINK_H
#define FRMCONFIGLINK_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigLink;
}

class frmConfigLink : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigLink(QWidget *parent = 0);
    ~frmConfigLink();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigLink *ui;
    QSqlTableModel *model;
    QStringList columnNames;
    QList<int> columnWidths;

    DbDelegate *d_cbox_positionID;
    QList<DbDelegate *> delegates;

private slots:
    void initForm();
    void initData();
    void changeStyle();
    void positionIDChanged();

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
};

#endif // FRMCONFIGLINK_H
