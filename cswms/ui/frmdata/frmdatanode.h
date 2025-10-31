#ifndef FRMDATANODE_H
#define FRMDATANODE_H

#include <QWidget>

class DbPage;

namespace Ui {
class frmDataNode;
}

class frmDataNode : public QWidget
{
    Q_OBJECT

public:
    explicit frmDataNode(QWidget *parent = 0);
    ~frmDataNode();

private:
    Ui::frmDataNode *ui;
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

    void on_cboxDeviceName_currentIndexChanged(int);
    void on_cboxNodeName_currentIndexChanged(int index);
};

#endif // FRMDATANODE_H
