#ifndef FRMVIEWPLOT_H
#define FRMVIEWPLOT_H

#include <QWidget>

class DbPage;
class QPrinter;

namespace Ui {
class frmViewPlot;
}

class frmViewPlot : public QWidget
{
    Q_OBJECT

public:
    explicit frmViewPlot(QWidget *parent = 0);
    ~frmViewPlot();

private:
    Ui::frmViewPlot *ui;

    QString whereSql;
    QList<QString> columnNames;
    QList<int> columnWidths;
    DbPage *dbPage;

    int maxCount;               //曲线点数量
    double currentValue;        //当前值
    QVector<double> keys;       //横坐标索引
    QVector<QString> labs;      //横坐标字符串
    QVector<double> values;     //数据集合

private slots:
    void initForm();
    void initPlot();
    void initRange();
    void initColor();
    void loadPlot();
    void clearData();
    void initData();
    void loadData();

private slots:
    void receiveValue(const QString &positionID, float value);
    void receiveValue(const QString &portName, quint8 addr, const QList<float> &values);

private slots:
    void on_btnSelect_clicked();
    void on_btnPrint_clicked();
    void on_cboxType_currentIndexChanged(int);
    void on_cboxPortName_currentIndexChanged(int);
    void on_cboxDeviceName_currentIndexChanged(int);
    void on_cboxNodeName_currentIndexChanged(int);
    void on_ckMove_stateChanged(int arg1);
};

#endif // FRMVIEWPLOT_H
