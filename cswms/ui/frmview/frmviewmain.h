#ifndef FRMVIEWMAIN_H
#define FRMVIEWMAIN_H

#include <QWidget>

namespace Ui {
class frmViewMain;
}

class frmViewMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmViewMain(QWidget *parent = nullptr);
    ~frmViewMain();

private:
    Ui::frmViewMain *ui;

    int TabCounts;    
    QWidget * CurrentTab = nullptr;
    QWidget * CurrentWidget = nullptr;
    //自己电脑合适的尺寸  2021.12.03
    int TabWidth = 1090;
    int TabHeight = 880;
    //
    void RemoveItemsInTabIndex(int Index);
    //
private Q_SLOTS:
    void UpdateUIBySelectedTabIndex(int Index);
    void TabBarDoubleClicked(int index);
};

#endif // FRMVIEWMAIN_H
