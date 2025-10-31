#ifndef FRMCONFIGUSER_H
#define FRMCONFIGUSER_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigUser;
}

class frmConfigUser : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigUser(QWidget *parent = 0);
    ~frmConfigUser();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigUser *ui;    

    //数据库表模型+列名+列宽
    QSqlTableModel *model;
    QList<QString> columnNames;
    QList<int> columnWidths;

    //自定义委托集合
    QList<DbDelegate *> delegates;

    //对应权限委托 宽度+类型+值
    bool isLoad;
    QList<int> delegateWidth;
    QList<QString> delegateType;
    QList<QStringList> delegateValue;

public:
    void setDelegate(const QList<int> &delegateWidth,
                     const QList<QString> &delegateType,
                     const QList<QStringList> &delegateValue);

private slots:
    void initForm();    //初始化界面数据
    void initIcon();    //初始化按钮图标
    void initData();    //初始化数据

private slots:
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnDelete_clicked();
    void on_btnReturn_clicked();
    void on_btnClear_clicked();

    void on_btnInput_clicked();
    void on_btnOutput_clicked();
    void dataout(int type);
    void on_btnPrint_clicked();
    void on_btnXls_clicked();
};

#endif // FRMCONFIGUSER_H
