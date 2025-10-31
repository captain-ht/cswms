#include "frmviewplot.h"
#include "ui_frmviewplot.h"
#include "quihelper.h"
#include "dbpage.h"
#include "dbquery.h"
#include "deviceserver.h"
#include "dbreceive.h"

frmViewPlot::frmViewPlot(QWidget *parent) : QWidget(parent), ui(new Ui::frmViewPlot)
{
    ui->setupUi(this);
    this->initPlot();
    this->initRange();
    this->initColor();
    this->initForm();
    this->initData();
    //this->loadData();
}

frmViewPlot::~frmViewPlot()
{
    delete ui;
}

void frmViewPlot::initForm()
{
    ui->frameRight->setProperty("flag", "navbtn");
    ui->frameBottom->setProperty("flag", "navlab");

    ui->dateStart->setDate(QDate::currentDate());
    ui->dateEnd->setDate(QDate::currentDate().addDays(1));
    ui->dateStart->calendarWidget()->setLocale(QLocale::Chinese);
    ui->dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight);

    bool ok = false;
    ui->tableView->setVisible(ok);
    ui->frameBottom->setEnabled(ok);
    ui->cboxPortName->addItems(DbData::PortInfo_PortName);

    connect(DbReceive::Instance(), SIGNAL(receiveValue(QString, float)),
            this, SLOT(receiveValue(QString, float)));
    connect(DeviceServer::Instance(), SIGNAL(receiveValue(QString, quint8, QList<float>)),
            this, SLOT(receiveValue(QString, quint8, QList<float>)));

    ui->ckMove->setChecked(true);
    connect(AppEvent::Instance(), SIGNAL(changeColor()), this, SLOT(initColor()));
}

void frmViewPlot::initPlot()
{
    //不同分辨率数量不一样
    maxCount = 20;
    if (QUIHelper::deskWidth() >= 1920) {
        maxCount = 30;
    }

    this->clearData();
    ui->customPlot->addGraph(1);
    ui->customPlot->initItem();

    //设置结构体数据
    LineData data;
    data.index = 0;
    data.key = keys;
    data.value = values;
    data.autoScale = ui->ckAuto->isChecked();
    ui->customPlot->setDataLine(data);
    ui->customPlot->setLabX(keys, labs);
    ui->customPlot->replot();
}

void frmViewPlot::initRange()
{
    //不同的曲线类型不同的范围值
    QString type = ui->cboxType->currentText();
    int count = keys.count();
    if (type == "实时曲线") {
        ui->customPlot->setRangeX(-0.5, count - 0.5);
        ui->customPlot->setRangeY(0, 110, 25);
        ui->gridLayout->setVerticalSpacing(0);
    } else if (type == "历史曲线") {
        ui->customPlot->setRangeX(-0.5, count - 0.5);
        ui->customPlot->setRangeY(0, 1010, 25);
        ui->gridLayout->setVerticalSpacing(7);
    }

    ui->customPlot->replot();
}

void frmViewPlot::initColor()
{
    ui->customPlot->setBgColor(AppConfig::ColorPlotBg);
    ui->customPlot->setTextColor(AppConfig::ColorPlotText);
    ui->customPlot->setLineColor(AppConfig::ColorPlotLine);
    ui->customPlot->replot();
}

void frmViewPlot::loadPlot()
{
    //没超过则后面追加
    //超过数量则移除第一个数据,增加一个数据,保证每次集合数据一致
    int count = keys.count();
    if (count < maxCount) {
        keys << count;
        labs << TIME;
        values << currentValue;
    } else {
        labs.remove(0, 1);
        values.remove(0, 1);
        labs << TIME;
        values << currentValue;
    }

    //设置结构体数据
    LineData data;
    data.index = 0;
    data.key = keys;
    data.value = values;
    data.autoScale = ui->ckAuto->isChecked();
    ui->customPlot->setDataLine(data);
    ui->customPlot->setLabX(keys, labs);
    ui->customPlot->replot();
}

void frmViewPlot::clearData()
{
    keys.clear();
    labs.clear();
    values.clear();
}

void frmViewPlot::initData()
{
    whereSql = "where 1=1";
    columnNames << "编号" << "位号" << "控制器名称" << "变量名称" << "数值" << "单位" << "保存时间";
    columnWidths << 100 << 120 << 145 << 145 << 80 << 80 << 150;

    //实例化翻页类
    dbPage = new DbPage(this);
    //关联查询记录总数
    connect(DbData::DbLocal, SIGNAL(receiveCount(QString, int, int)), dbPage, SLOT(receiveCount(QString, int, int)));

    dbPage->setAllCenter(true);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);
    dbPage->setRecordsPerpage(maxCount);
    dbPage->setDbType(AppConfig::LocalDbType);
    dbPage->setTableName("NodeLog");
    dbPage->setOrderSql(QString("LogID %1").arg(AppConfig::AlarmLogOrder));
    dbPage->setControl(ui->tableView, ui->labPageTotal, ui->labPageCurrent, ui->labResultTotal, ui->labRecordsPerpage,
                       ui->labSelectTime, 0, ui->btnFirst, ui->btnPrevious, ui->btnNext, ui->btnLast);

    //绑定按钮切换载入数据
    connect(ui->btnFirst, SIGNAL(clicked()), this, SLOT(loadData()));
    connect(ui->btnPrevious, SIGNAL(clicked()), this, SLOT(loadData()));
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(loadData()));
    connect(ui->btnLast, SIGNAL(clicked()), this, SLOT(loadData()));
}

void frmViewPlot::loadData()
{
    this->clearData();

    //加载当前表格中的数据
    QAbstractItemModel *model = ui->tableView->model();
    int count = model->rowCount();
    for (int i = 0; i < count; i++) {
        QModelIndex modelIndex = model->index(i, 4);
        QModelIndex labIndex = model->index(i, 6);
        double value = model->data(modelIndex).toDouble();
        QString strDate = model->data(labIndex).toString().right(8);

        keys << i;
        labs << strDate;
        values << value;
        if (keys.count() == maxCount) {
            break;
        }
    }

    //设置结构体数据
    LineData data;
    data.index = 0;
    data.name = "数值";
    data.key = keys;
    data.value = values;
    data.autoScale = ui->ckAuto->isChecked();
    ui->customPlot->setDataLine(data);
    ui->customPlot->replot();
}

void frmViewPlot::receiveValue(const QString &positionID, float value)
{
    if (ui->ckPause->isChecked()) {
        return;
    }

    //必须是实时曲线才需要显示
    if (ui->cboxType->currentText() != "实时曲线") {
        return;
    }

    //找到当前位号对应的索引,取出对应索引位置的值
    QString id = ui->cboxNodeName->itemData(ui->cboxNodeName->currentIndex()).toString();
    if (id != positionID) {
        return;
    }

    currentValue = value;
    this->loadPlot();
}

void frmViewPlot::receiveValue(const QString &portName, quint8 addr, const QList<float> &values)
{
    if (ui->ckPause->isChecked()) {
        return;
    }

    //必须是实时曲线才需要显示
    if (ui->cboxType->currentText() != "实时曲线") {
        return;
    }

    //过滤端口
    if (ui->cboxPortName->currentText() != portName) {
        return;
    }

    //过滤设备
    if (ui->cboxDeviceName->itemData(ui->cboxDeviceName->currentIndex()).toInt() != addr) {
        return;
    }

    //找到当前位号对应的索引,取出对应索引位置的值
    QString positionID = ui->cboxNodeName->itemData(ui->cboxNodeName->currentIndex()).toString();
    //找到当前索引位置的设备地址对应变量的最小寄存器地址
    //如果读取的起始寄存器地址是5则回来的数据位第一个是寄存器地址5的数据,后面连续
    quint16 nodeMinAddr = DbQuery::getNodeMinAddr(portName, addr);
    int index = DbData::NodeInfo_PositionID.indexOf(positionID);
    int startIndex = DbData::NodeInfo_NodeAddr.at(index) - nodeMinAddr - 1;
    //有时候可能出现总共添加了8个变量但是真实读到4个变量的情况
    if (startIndex >= values.count()) {
        return;
    }

    currentValue = values.at(startIndex);
    this->loadPlot();
}

void frmViewPlot::on_btnSelect_clicked()
{
    if (ui->cboxType->currentText() == "实时曲线") {
        return;
    }

    QDateTime dateStart = ui->dateStart->dateTime();
    QDateTime dateEnd = ui->dateEnd->dateTime();
    if (dateStart > dateEnd) {
        QUIHelper::showMessageBoxError("开始时间不能大于结束时间!", 3);
        return;
    }

    //构建SQL语句
    QString sql = "where 1=1";
    if (AppConfig::LocalDbType.toUpper() == "SQLITE") {
        sql += " and datetime(SaveTime)>='" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "'";
        sql += " and datetime(SaveTime)<='" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "'";
    } else if (AppConfig::LocalDbType.toUpper() == "MYSQL") {
        sql += " and unix_timestamp(SaveTime)>=unix_timestamp('" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "')";
        sql += " and unix_timestamp(SaveTime)<=unix_timestamp('" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "')";
    }

    sql += " and DeviceName='" + ui->cboxDeviceName->currentText() + "'";
    sql += " and NodeName='" + ui->cboxNodeName->currentText() + "'";

    //绑定数据到表格
    whereSql = sql;
    dbPage->setWhereSql(whereSql);
    dbPage->select();
    DbData::DbLocal->selectCount("NodeLog", "LogID", whereSql);

    loadData();
}

void frmViewPlot::on_btnPrint_clicked()
{
    QString fileName = QUIHelper::appPath() + "/temp.png";
    ui->customPlot->getPlot()->savePng(fileName);
    QUIHelper::openFile(fileName, "保存曲线截图");
}

void frmViewPlot::on_cboxType_currentIndexChanged(int)
{
    if (!isVisible()) {
        return;
    }

    bool ok = (ui->cboxType->currentText() == "历史曲线");
    ui->tableView->setVisible(ok);
    ui->frameBottom->setEnabled(ok);
    this->clearData();
    this->initRange();

    if (ok) {
        on_btnSelect_clicked();
    }
}

void frmViewPlot::on_cboxPortName_currentIndexChanged(int)
{
    ui->cboxDeviceName->clear();
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_PortName.at(i) == ui->cboxPortName->currentText()) {
            QString deviceName = DbData::DeviceInfo_DeviceName.at(i);
            quint8 deviceAddr = DbData::DeviceInfo_DeviceAddr.at(i);
            ui->cboxDeviceName->addItem(deviceName, deviceAddr);
        }
    }
}

void frmViewPlot::on_cboxDeviceName_currentIndexChanged(int)
{
    ui->cboxNodeName->clear();
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        if (DbData::NodeInfo_DeviceName.at(i) == ui->cboxDeviceName->currentText()) {
            ui->cboxNodeName->addItem(DbData::NodeInfo_NodeName.at(i), DbData::NodeInfo_PositionID.at(i));
        }
    }
}

void frmViewPlot::on_cboxNodeName_currentIndexChanged(int)
{
    this->clearData();
    ui->customPlot->replot();
}

void frmViewPlot::on_ckMove_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->customPlot->setInteractions(0);
    } else {
        ui->customPlot->setInteractions(3);
    }
}
