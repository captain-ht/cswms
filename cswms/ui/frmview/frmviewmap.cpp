#include "frmviewmap.h"
#include "ui_frmviewmap.h"

frmViewMap::frmViewMap(QWidget *parent) : QWidget(parent), ui(new Ui::frmViewMap)
{
    ui->setupUi(this);
}

frmViewMap::~frmViewMap()
{
    delete ui;
}

QLabel *frmViewMap::getLabel()
{
    return ui->labMap;
}
