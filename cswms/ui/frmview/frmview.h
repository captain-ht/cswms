#ifndef FRMVIEW_H
#define FRMVIEW_H

#include <QWidget>
#include <QTreeWidgetItem>

class DeviceButton;

namespace Ui {
class frmView;
}

class frmView : public QWidget
{
    Q_OBJECT

public:
    explicit frmView(QWidget *parent = 0);
    ~frmView();

private:
    Ui::frmView *ui;

private slots:
    void initForm();
    void initWidget();
    void initTree();
    void initControl();
    void initServer();
    void mousePressed(int index);
    void doubleClicked(DeviceButton *btn);

private slots:
    void on_listWidget_pressed();
    void on_treeWidget_itemPressed(QTreeWidgetItem *item, int column);

    void on_btnClearSound_clicked();
    void on_btnClearMsg_clicked();

signals:
    void itemClicked(const QString &text);

public slots:
    void setIndex(int index);
    void receiveAlarm(const QString &deviceName, const QString &positionID, quint8 alarmType);
};

#endif // FRMVIEW_H
