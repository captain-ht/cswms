#ifndef FRMTOOL_H
#define FRMTOOL_H

#include <QWidget>
#include <QSpinBox>
#include <QTcpServer>
#include <QSqlTableModel>
#include "qextserialport.h"

namespace Ui {
class frmTool;
}

class frmTool : public QWidget
{
    Q_OBJECT

public:
    explicit frmTool(QWidget *parent = 0);
    ~frmTool();

private:
    Ui::frmTool *ui;
    bool isComOk;
    QextSerialPort *com;
    bool isTcpOk;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QTimer *timerRead;
    QList<QSpinBox *> spinBox;

    QSqlTableModel *model;
    QStringList columnNames;
    QList<int> columnWidths;
    QTimer *timerData;

private slots:
    void initForm();
    void initData();
    void loadData();
    void initConfig();
    void saveConfig();
    void newConnection();
    void disConnected();
    void readDataTcp();
    void readDataCom();
    void checkData(const QByteArray &data, bool isCom = true);
    void append(int type, const QString &data, bool clear = false);

private slots:
    void on_btnOpen_clicked();
    void on_btnListen_clicked();
    void on_btnClearData_clicked();
    void on_btnShowData_clicked();

    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnDelete_clicked();
    void on_btnReturn_clicked();
    void on_btnClear_clicked();
    void on_ckAutoData_stateChanged(int arg1);
};

#endif // FRMTOOL_H
