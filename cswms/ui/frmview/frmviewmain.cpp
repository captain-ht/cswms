#include "frmviewmain.h"
#include "ui_frmviewmain.h"
#include "frmviewcrane.h"
#include "frmbigcart.h"
#include <iostream>
#include "frmsmall.h"
#include "frmlift.h"
#include "frmglobalstate.h"

frmViewMain::frmViewMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmViewMain)
{
    ui->setupUi(this);

    TabCounts = 5;
    CurrentTab = ui->tab_Crane;

    UpdateUIBySelectedTabIndex(0);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this,
            SLOT(UpdateUIBySelectedTabIndex(int)));

    connect(ui->tabWidget, SIGNAL(tabBarDoubleClicked(int)), this,
            SLOT(TabBarDoubleClicked(int)));

}

frmViewMain::~frmViewMain()
{
    if (CurrentWidget != nullptr)
    {
        CurrentWidget->deleteLater();
        CurrentWidget = nullptr;
    }

    delete ui;
}

void frmViewMain::RemoveItemsInTabIndex(int Index)
{
    QWidget * TempTab = nullptr;

    if (Index == 0) TempTab = ui->tab_Crane;
    else if (Index == 1) TempTab = ui->tab_Big;
    else if (Index == 2) TempTab = ui->tab_Small;
    else if (Index == 3) TempTab = ui->tab_Lift;
    //if (Index == 4) TempTab = ui->tab_Crane;

    if (TempTab == nullptr) return;

    QObjectList list = TempTab->children();
    Q_FOREACH(QObject* obj, list)
    {
        obj->deleteLater();
        delete obj;
    }
}

//单击切换每一个子页面
void frmViewMain::UpdateUIBySelectedTabIndex(int Index)
{
    if (CurrentTab == nullptr) return;

    for(int i = 0; i < TabCounts; i++)
    {
        RemoveItemsInTabIndex(i);
    }

    //frmGlobalState Device UI
    if (Index == 0)
    {        
        CurrentTab = ui->tab_Crane;
        CurrentTab->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        frmGlobalState * pForm = new frmGlobalState(CurrentTab);
        pForm->setGeometry(0, 0, TabWidth, TabHeight);
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->show();
        pForm->setFocus();
    }
    //Big Cart Device UI
    else if(Index == 1)
    {
        CurrentTab = ui->tab_Big;
        CurrentTab->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        frmBigCart * pForm = new frmBigCart(CurrentTab);
        pForm->setGeometry(0, 0, TabWidth, TabHeight);
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->show();
        pForm->setFocus();
    }
    //Small Cart Device UI
    else if(Index == 2)
    {
        CurrentTab = ui->tab_Small;
        CurrentTab->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        frmSmall * pForm = new frmSmall(CurrentTab);
        pForm->setGeometry(0, 0, TabWidth, TabHeight);
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->show();
        pForm->setFocus();
    }
    else if(Index == 3)
    {
        CurrentTab = ui->tab_Lift;
        CurrentTab->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        frmLift * pForm = new frmLift(CurrentTab);
        pForm->setGeometry(0, 0, TabWidth, TabHeight);
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->show();
        pForm->setFocus();
    }
    else if(Index == 4)
    {
        CurrentTab = ui->tab_Lift;
        CurrentTab->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        frmLift * pForm = new frmLift(CurrentTab);
        pForm->setGeometry(0, 0, TabWidth, TabHeight);
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->show();
        pForm->setFocus();
    }
}


//双击以打开每一个子页面并最大化显示
void frmViewMain::TabBarDoubleClicked(int Index)
{
    if (Index == 0)
    {
        frmGlobalState * pForm = new frmGlobalState();
        CurrentWidget = pForm;
        pForm->setWindowFlags(pForm->windowFlags()&~Qt::WindowMinMaxButtonsHint);        
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->setAttribute(Qt::WA_ShowModal, true);//属性设置
        pForm->setWindowModality(Qt::ApplicationModal);//设置阻塞类型
        pForm->showMaximized();
        pForm->setFocus();
    }
    //Big Cart Device UI
    else if(Index == 1)
    {
        frmBigCart * pForm = new frmBigCart();
        CurrentWidget = pForm;
        pForm->setWindowFlags(pForm->windowFlags()&~Qt::WindowMinMaxButtonsHint);        
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->setAttribute(Qt::WA_ShowModal, true);//属性设置
        pForm->setWindowModality(Qt::ApplicationModal);//设置阻塞类型
        pForm->showMaximized();
        pForm->UpdateUI();
        pForm->setFocus();
    }
    //Small Cart Device UI
    else if(Index == 2)
    {
        frmSmall * pForm = new frmSmall();
        CurrentWidget = pForm;
        pForm->setWindowFlags(pForm->windowFlags()&~Qt::WindowMinMaxButtonsHint);        
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->setAttribute(Qt::WA_ShowModal, true);//属性设置
        pForm->setWindowModality(Qt::ApplicationModal);//设置阻塞类型
        pForm->showMaximized();
        pForm->setFocus();
    }
    else if(Index == 3)
    {
        frmLift * pForm = new frmLift();
        CurrentWidget = pForm;
        pForm->setWindowFlags(pForm->windowFlags()&~Qt::WindowMinMaxButtonsHint);        
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->setAttribute(Qt::WA_ShowModal, true);//属性设置
        pForm->setWindowModality(Qt::ApplicationModal);//设置阻塞类型
        pForm->showMaximized();
        pForm->setFocus();
    }
    else if(Index == 4)
    {
        frmviewcrane * pForm = new frmviewcrane();
        CurrentWidget = pForm;
        pForm->setWindowFlags(pForm->windowFlags()&~Qt::WindowMinMaxButtonsHint);        
        pForm->setFont(QFont(("Sans Serif"), 12, QFont::Normal));
        pForm->setAttribute(Qt::WA_ShowModal, true);//属性设置
        pForm->setWindowModality(Qt::ApplicationModal);//设置阻塞类型
        pForm->showMaximized();
        pForm->setFocus();
    }
}
