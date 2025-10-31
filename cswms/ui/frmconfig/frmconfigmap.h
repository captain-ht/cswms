#ifndef FRMCONFIGMAP_H
#define FRMCONFIGMAP_H

#include <QWidget>

namespace Ui {
class frmConfigMap;
}

class frmConfigMap : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigMap(QWidget *parent = 0);
    ~frmConfigMap();

private:
    Ui::frmConfigMap *ui;    

private slots:
    void initForm();

private slots:
    void on_listWidget_pressed();
    void on_btnInput_clicked();
    void on_btnDelete_clicked();

signals:
    void changeMap();
};

#endif // FRMCONFIGMAP_H
