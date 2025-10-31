#ifndef FRMCONFIG_H
#define FRMCONFIG_H

#include <QWidget>

class QAbstractButton;

namespace Ui {
class frmConfig;
}

class frmConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfig(QWidget *parent = 0);
    ~frmConfig();

private:
    Ui::frmConfig *ui;

    //导航按钮+图标集合
    QList<int> icons;
    QList<QAbstractButton *> btns;

private slots:
    void initForm();        //初始化界面数据
    void initWidget();      //初始化子窗体
    void initNav();         //初始化导航按钮
    void initIcon();        //初始化按钮图标
    void buttonClicked();   //按钮单击事件
};

#endif // FRMCONFIG_H
