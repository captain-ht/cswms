#include "frmviewdata.h"
#include "ui_frmviewdata.h"

frmViewData::frmViewData(QWidget *parent) : QWidget(parent), ui(new Ui::frmViewData)
{
    ui->setupUi(this);
}

frmViewData::~frmViewData()
{
    delete ui;
}

QTableWidget *frmViewData::getTable()
{
    return ui->tableWidget;
}
