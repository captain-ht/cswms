#ifndef FRMLOGOUT_H
#define FRMLOGOUT_H

#include <QDialog>

namespace Ui {
class frmLogout;
}

class frmLogout : public QDialog
{
    Q_OBJECT

public:
    static frmLogout *Instance();
    explicit frmLogout(QWidget *parent = 0);
    ~frmLogout();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmLogout *ui;
    static QScopedPointer<frmLogout> self;

public:
    //设置顶部横幅背景图片
    void setBanner(const QString &image);

private slots:
    void initStyle();   //初始化无边框窗体
    void initForm();    //初始化窗体数据
    void initIcon();    //初始化按钮图标

private slots:
    void on_btnLogout_clicked();
};

#endif // FRMLOGOUT_H
