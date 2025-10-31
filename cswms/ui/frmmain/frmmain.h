#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <QObject>
#include <QList>
#include <QStringList>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>

class QAbstractButton;

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
protected:
    void closeEvent(QCloseEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmMain *ui;

    //导航按钮+图标集合
    QList<int> icons;
    QList<QAbstractButton *> btns;

private slots:
    void initForm();
    void initText();
    void initNav();
    void initIcon();
    void buttonClicked();
//    void timeout();

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

signals:
    void setIndex(int index);
};

#endif // FRMMAIN_H
