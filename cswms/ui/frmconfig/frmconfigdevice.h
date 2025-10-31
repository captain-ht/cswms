#ifndef FRMCONFIGDEVICE_H
#define FRMCONFIGDEVICE_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigDevice;
}

class frmConfigDevice : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigDevice(QWidget *parent = 0);
    ~frmConfigDevice();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigDevice *ui;
    QSqlTableModel *model;
    QStringList columnNames;
    QList<int> columnWidths;

    DbDelegate *d_cbox_portName;
    DbDelegate *d_cbox_deviceType;
    QList<DbDelegate *> delegates;

private slots:
    void initForm();
    void initData();
    void changeStyle();
    void deviceTypeChanged();
    void portNameChanged();
    void typeValueChanged(const QString &value);
    void numberValueChanged(const QString &value);

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
    void deviceNameChanged();
};

#endif // FRMCONFIGDEVICE_H
