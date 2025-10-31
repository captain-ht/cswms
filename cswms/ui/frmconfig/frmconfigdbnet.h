#ifndef FRMCONFIGDBNET_H
#define FRMCONFIGDBNET_H

#include <QWidget>

namespace Ui {
class frmConfigDbNet;
}

class frmConfigDbNet : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigDbNet(QWidget *parent = 0);
    ~frmConfigDbNet();

private:
    Ui::frmConfigDbNet *ui;

    bool autoSetInfo;       //自动填入信息默认假
    bool btnInRight;        //按钮在右侧默认假
    QString connName;       //连接名称默认testdb
    QString connFlag;       //连接标识默认为数据库名称

private slots:
    void initForm();        //初始化界面数据
    void initConfig();      //加载数据库配置
    void saveConfig();      //保存数据库配置

private slots:
    void on_btnConnect_clicked();
    void on_btnInit_clicked();
    void on_cboxDbType_activated(int);
};

#endif // FRMCONFIGDBNET_H
