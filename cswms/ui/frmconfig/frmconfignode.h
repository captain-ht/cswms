#ifndef FRMCONFIGNODE_H
#define FRMCONFIGNODE_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigNode;
}

class frmConfigNode : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigNode(QWidget *parent = 0);
    ~frmConfigNode();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigNode *ui;
    QSqlTableModel *model;
    QStringList columnNames;
    QList<int> columnWidths;

    DbDelegate *d_cbox_deviceName;
    DbDelegate *d_cbox_permissionType;
    DbDelegate *d_cbox_registerArea;
    DbDelegate *d_cbox_dataType;
    DbDelegate *d_cbox_nodeType;
    DbDelegate *d_cbox_nodeClass;
    DbDelegate *d_cbox_nodeSign;
    DbDelegate *d_cbox_nodeImage;
    QList<DbDelegate *> delegates;

private slots:
    void initForm();
    void initData();
    void changeStyle();
    void deviceNameChanged();
    void permissionTypeChanged();
    void registerAreaChanged();
    void dataTypeChanged();
    void nodeTypeChanged();
    void nodeClassChanged();
    void nodeSignChanged();    
    void nodeImageChanged();
    void soundChanged(const QString &soundName);

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
    void positionIDChanged();
};

#endif // FRMCONFIGNODE_H
