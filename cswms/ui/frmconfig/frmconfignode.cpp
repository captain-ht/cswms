#include "frmconfignode.h"
#include "ui_frmconfignode.h"
#include "quihelper.h"
#include "devicehelper.h"
#include "appkey.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "datahelper.h"

frmConfigNode::frmConfigNode(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigNode)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->changeStyle();
}

frmConfigNode::~frmConfigNode()
{
    delete ui;
}

void frmConfigNode::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigNode::initForm()
{
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, false, true);
    ui->widgetTop->setProperty("flag", "navbtn");
    if (QUIHelper::deskWidth() < 1440) {
        ui->labTip->setText("提示 → 改动后需重启应用");
    } else {
        ui->labTip->setText("提示 → 必须和现场变量信息一致,改动后需重启应用");
    }
}

void frmConfigNode::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //指定表名
    model->setTable("NodeInfo");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();

    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);
    ui->tableView->setProperty("model", true);

    //初始化列名和列宽
    columnNames = AppData::NodeInfoColumns.split("|");
    columnWidths << 40 << 85 << 110 << 85 << 85 << 70 << 90 << 90 << 70 << 80 << 60 << 60 << 50 << 50
                 << 50 << 40 << 40 << 35 << 60 << 70 << 45 << 45 << 50 << 50 << 45 << 45 << 45 << 45 << 45;

    //特殊分辨率重新设置列宽
    if (QUIHelper::deskWidth() < 1440) {
        columnWidths[1] = 75;
        columnWidths[2] = 100;
        columnWidths[3] = 75;
        columnWidths[4] = 50;
    }

    //挨个设置列名和列宽
    for (int i = 0; i < columnNames.count(); i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //隐藏坐标XY列
    ui->tableView->setColumnHidden(27, true);
    ui->tableView->setColumnHidden(28, true);

    //编号委托
    QStringList nodeID;
    for (int i = 1; i <= 255; i++) {
        nodeID.append(QString::number(i));
    }

    DbDelegate *d_cbox_nodeID = new DbDelegate(this);
    d_cbox_nodeID->setDelegateType("QComboBox");
    d_cbox_nodeID->setDelegateValue(nodeID);
    //ui->tableView->setItemDelegateForColumn(0, d_cbox_nodeID);

    //控制器名称委托
    d_cbox_deviceName = new DbDelegate(this);
    d_cbox_deviceName->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(2, d_cbox_deviceName);
    deviceNameChanged();

    //读写类型委托
    d_cbox_permissionType = new DbDelegate(this);
    d_cbox_permissionType->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(5, d_cbox_permissionType);
    permissionTypeChanged();

    //数据类型委托
    d_cbox_dataType = new DbDelegate(this);
    d_cbox_dataType->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(6, d_cbox_dataType);
    dataTypeChanged();

    //寄存器区委托
    d_cbox_registerArea = new DbDelegate(this);
    d_cbox_registerArea->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(7, d_cbox_registerArea);
    registerAreaChanged();

    //变量型号委托
    d_cbox_nodeType = new DbDelegate(this);
    d_cbox_nodeType->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(9, d_cbox_nodeType);
    nodeTypeChanged();

    //变量种类委托
    d_cbox_nodeClass = new DbDelegate(this);
    d_cbox_nodeClass->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(10, d_cbox_nodeClass);
    nodeClassChanged();

    //变量单位委托
    d_cbox_nodeSign = new DbDelegate(this);
    d_cbox_nodeSign->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(11, d_cbox_nodeSign);
    nodeSignChanged();

    //启用禁用委托
    DbDelegate *d_ckbox_nodeEnable = new DbDelegate(this);
    d_ckbox_nodeEnable->setDelegateColumn(17);
    d_ckbox_nodeEnable->setDelegateType("QCheckBox");
    d_ckbox_nodeEnable->setCheckBoxText("启用", "禁用");
    ui->tableView->setItemDelegateForColumn(17, d_ckbox_nodeEnable);

    //报警声音委托
    QStringList nodeSound;
    nodeSound.append("无");
    nodeSound.append(AppData::SoundNames);
    DbDelegate *d_cbox_nodeSound = new DbDelegate(this);
    d_cbox_nodeSound->setDelegateType("QComboBox");
    d_cbox_nodeSound->setDelegateValue(nodeSound);
    connect(d_cbox_nodeSound, SIGNAL(valueChanged(QString)), this, SLOT(soundChanged(QString)));
    ui->tableView->setItemDelegateForColumn(18, d_cbox_nodeSound);

    //背景地图委托
    d_cbox_nodeImage = new DbDelegate(this);
    d_cbox_nodeImage->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(19, d_cbox_nodeImage);
    nodeImageChanged();

    //小数点委托
    QStringList dotCount;
    dotCount << "0" << "1" << "2" << "3";
    DbDelegate *d_cbox_dotCount = new DbDelegate(this);
    d_cbox_dotCount->setDelegateType("QComboBox");
    d_cbox_dotCount->setDelegateValue(dotCount);
    ui->tableView->setItemDelegateForColumn(21, d_cbox_dotCount);

    //启用禁用委托
    DbDelegate *d_ckbox_alarmEnable = new DbDelegate(this);
    d_ckbox_alarmEnable->setDelegateColumn(22);
    d_ckbox_alarmEnable->setDelegateType("QCheckBox");
    d_ckbox_alarmEnable->setCheckBoxText("启用", "禁用");
    ui->tableView->setItemDelegateForColumn(22, d_ckbox_alarmEnable);

    //报警类型委托
    QStringList alarmType;
    alarmType << "HH" << "HL" << "LL";
    DbDelegate *d_cbox_alarmType = new DbDelegate(this);
    d_cbox_alarmType->setDelegateType("QComboBox");
    d_cbox_alarmType->setDelegateValue(alarmType);
    ui->tableView->setItemDelegateForColumn(24, d_cbox_alarmType);

    //用来切换样式自动改变颜色
    delegates << d_cbox_nodeID;
    delegates << d_cbox_deviceName;
    delegates << d_cbox_permissionType;
    delegates << d_cbox_registerArea;
    delegates << d_cbox_dataType;
    delegates << d_cbox_nodeType;
    delegates << d_cbox_nodeClass;
    delegates << d_cbox_nodeSign;
    delegates << d_ckbox_nodeEnable;
    delegates << d_cbox_nodeSound;
    delegates << d_cbox_nodeImage;
    delegates << d_cbox_dotCount;
    delegates << d_ckbox_alarmEnable;
    delegates << d_cbox_alarmType;
}

void frmConfigNode::changeStyle()
{
    foreach (DbDelegate *delegate, delegates) {
        delegate->setTextColor(QUIConfig::TextColor);
        delegate->setSelectBgColor(QUIConfig::NormalColorStart);
    }
}

void frmConfigNode::deviceNameChanged()
{
    d_cbox_deviceName->setDelegateValue(DbData::DeviceInfo_DeviceName);
}

void frmConfigNode::permissionTypeChanged()
{
    QList<QString> list;
    list.append("只读");
    list.append("只写");
    list.append("读写");
    d_cbox_permissionType->setDelegateValue(list);
}

void frmConfigNode::registerAreaChanged()
{
    //后面可改成根据设备驱动获取支持的读写区域
    QList<QString> list;
    list.append("DO线圈");
    list.append("DI离散输入寄存器");
    list.append("AO保持寄存器");
    list.append("AI输入寄存器");
    d_cbox_registerArea->setDelegateValue(list);
}

void frmConfigNode::dataTypeChanged()
{
    //后面可改成根据设备驱动获取支持的数据类型
    QList<QString> list;
    list.append("Bit1开关量");
    list.append("Char8位有符号数");
    list.append("Byte8位无符号数");
    list.append("Short16位有符号数");
    list.append("Word16位无符号数");
    list.append("ASCII2个字符");
    list.append("Long32位有符号数");
    list.append("Dword32位无符号数");
    list.append("Float单精度浮点数");
    list.append("String字符串");
    list.append("Double双精度浮点数");
    list.append("BCD");
    list.append("LongLong64位有符号数");
    list.append("DwordDword64位无符号数");
    d_cbox_dataType->setDelegateValue(list);
}

void frmConfigNode::nodeTypeChanged()
{
    d_cbox_nodeType->setDelegateValue(DbData::TypeInfo_NodeType);
}

void frmConfigNode::nodeClassChanged()
{
    d_cbox_nodeClass->setDelegateValue(DbData::TypeInfo_NodeClass);
}

void frmConfigNode::nodeSignChanged()
{
    d_cbox_nodeSign->setDelegateValue(DbData::TypeInfo_NodeSign);
}

void frmConfigNode::nodeImageChanged()
{
    QStringList imageNames;
    imageNames.append(" -- 无 -- ");
    imageNames.append(AppData::MapNames);
    d_cbox_nodeImage->setDelegateValue(imageNames);
}

void frmConfigNode::soundChanged(const QString &soundName)
{
    DeviceHelper::playSound(soundName);
}

void frmConfigNode::on_btnAdd_clicked()
{
    //对密钥文件中的数量限制进行过滤
    int count = model->rowCount();
    if (!AppKey::Instance()->checkCount(count)) {
        return;
    }

    model->insertRow(count);
    int nodeID = model->index(count - 1, 0).data().toInt() + 1;
    QString positionID = model->index(count - 1, 1).data().toString();
    QString deviceName = model->index(count - 1, 2).data().toString();
    QString nodeName = model->index(count - 1, 3).data().toString();
    QString nodeDescription = model->index(count - 1, 4).data().toString();
    QString permissionType = model->index(count - 1, 5).data().toString();
    QString dataType = model->index(count - 1, 6).data().toString();
    QString registerArea = model->index(count - 1, 7).data().toString();
    int nodeAddr = model->index(count - 1, 8).data().toInt() + 1;
    QString nodeType = model->index(count - 1, 9).data().toString();
    QString nodeClass = model->index(count - 1, 10).data().toString();
    QString nodeSign = model->index(count - 1, 11).data().toString();
    float nodeUpper = model->index(count - 1, 12).data().toFloat();
    float nodeLimit = model->index(count - 1, 13).data().toFloat();
    float nodeMax = model->index(count - 1, 14).data().toFloat();
    float nodeMin = model->index(count - 1, 15).data().toFloat();
    float nodeRange = model->index(count - 1, 16).data().toFloat();
    QString nodeEnable = model->index(count - 1, 17).data().toString();
    QString nodeSound = model->index(count - 1, 18).data().toString();
    QString nodeImage = model->index(count - 1, 19).data().toString();
    int saveInterval = model->index(count - 1, 20).data().toInt();
    int dotCount = model->index(count - 1, 21).data().toInt();
    QString alarmEnable = model->index(count - 1, 22).data().toString();
    int alarmDelay = model->index(count - 1, 23).data().toInt();
    QString alarmType = model->index(count - 1, 24).data().toString();
    float alarmH = model->index(count - 1, 25).data().toFloat();
    float alarmL = model->index(count - 1, 26).data().toFloat();
    int nodeX = model->index(count - 1, 27).data().toInt();
    int nodeY = model->index(count - 1, 28).data().toInt();

    //位号末尾数字的话自动递增
    int id = positionID.right(3).toInt();
    if (id > 0) {
        positionID = QString("%1%2").arg(positionID.left(positionID.length() - 3)).arg(id + 1, 3, 10, QChar('0'));
    }

    //位置递增
    nodeX += AppData::DeviceWidth + 5;
    if (nodeX > AppData::MapWidth - 100) {
        nodeX = 5;
        nodeY += AppData::DeviceHeight + 5;
    }

    //自动填充末尾序号递增
    if (nodeName.startsWith("变量-")) {
        nodeName = QString("变量-%1").arg(nodeAddr);
    } else if (nodeName.startsWith("设备-")) {
        nodeName = QString("设备-%1").arg(nodeAddr);
    } else if (nodeName.startsWith("设备#")) {
        nodeName = QString("设备#%1").arg(nodeAddr);
    } else if (nodeName.startsWith("设备")) {
        nodeName = QString("设备%1").arg(nodeAddr);
    }

    //当前为第一行时候的默认值
    if (count == 0) {
        nodeID = 1;
        positionID = AppData::DefaultPositionID;
        deviceName = DbData::DeviceInfo_Count > 0 ? DbData::DeviceInfo_DeviceName.first() : "1#控制器";
        nodeName = "变量-1";
        permissionType = "只读";
        dataType = "Float单精度浮点数";
        registerArea = "AI输入寄存器";
        nodeAddr = 1;
        nodeType = "FT-2104";
        nodeClass = "温度计";
        nodeSign = "℃";
        nodeUpper = 100;
        nodeLimit = 0;
        nodeMax = 100;
        nodeMin = 0;
        nodeRange = 0;
        nodeEnable = "启用";
        nodeSound = "11.wav";
        nodeImage = AppData::MapNames.count() > 0 ? AppData::MapNames.first() : " -- 无 -- ";
        saveInterval = 60;
        dotCount = 0;
        alarmDelay = 0;
        alarmType = "HH";
        nodeX = 5;
        nodeY = 5;

        if (AppConfig::WorkMode == 2) {
            nodeType = "WF-GM-DT";
            nodeClass = "辐射";
            nodeSign = "ugy/h";
            nodeUpper = 2.5;
            nodeLimit = 1000;
            nodeMax = 100000;
        }
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), nodeID);
    model->setData(model->index(count, 1), positionID);
    model->setData(model->index(count, 2), deviceName);
    model->setData(model->index(count, 3), nodeName);
    model->setData(model->index(count, 4), nodeDescription);
    model->setData(model->index(count, 5), permissionType);
    model->setData(model->index(count, 6), dataType);
    model->setData(model->index(count, 7), registerArea);
    model->setData(model->index(count, 8), nodeAddr);
    model->setData(model->index(count, 9), nodeType);
    model->setData(model->index(count, 10), nodeClass);
    model->setData(model->index(count, 11), nodeSign);
    model->setData(model->index(count, 12), nodeUpper);
    model->setData(model->index(count, 13), nodeLimit);
    model->setData(model->index(count, 14), nodeMax);
    model->setData(model->index(count, 15), nodeMin);
    model->setData(model->index(count, 16), nodeRange);
    model->setData(model->index(count, 17), nodeEnable);
    model->setData(model->index(count, 18), nodeSound);
    model->setData(model->index(count, 19), nodeImage);
    model->setData(model->index(count, 20), saveInterval);
    model->setData(model->index(count, 21), dotCount);
    model->setData(model->index(count, 22), alarmEnable);
    model->setData(model->index(count, 23), alarmDelay);
    model->setData(model->index(count, 24), alarmType);
    model->setData(model->index(count, 25), alarmH);
    model->setData(model->index(count, 26), alarmL);
    model->setData(model->index(count, 27), nodeX);
    model->setData(model->index(count, 28), nodeY);



    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigNode::on_btnSave_clicked()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadNodeInfo();

        //先更新联动表的位号
        emit positionIDChanged();
    } else {
        model->database().rollback();
        QUIHelper::showMessageBoxError("保存信息失败,信息不能为空,请重新填写!");
    }
}

void frmConfigNode::on_btnDelete_clicked()
{
    if (ui->tableView->currentIndex().row() < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的变量!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该变量吗?") == QMessageBox::Yes) {
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        model->submitAll();

        //主动加载所有数据,不然获取到的行数<=256
        while (model->canFetchMore()) {
            model->fetchMore();
        }

        int count = model->rowCount();
        ui->tableView->setCurrentIndex(model->index(count - 1, 0));
    }
}

void frmConfigNode::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigNode::on_btnClear_clicked()
{
    int count = model->rowCount();
    if (count <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有变量信息吗?") == QMessageBox::Yes) {
        DbQuery::clearNodeInfo();
        model->select();
    }
}

void frmConfigNode::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    QString flag = "pdf";
    if (type == 2) {
        flag = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("nodeinfo_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    QString columns = "NodeID,PositionID,DeviceName,NodeName,NodeAddr,NodeType,NodeClass,NodeSign,NodeUpper,NodeLimit,"
                      "NodeMax,NodeMin,NodeRange,NodeEnable,SaveInterval,DotCount,AlarmDelay";
    QString where = "order by NodeID asc";
    QStringList content = DataCsv::getContent("NodeInfo", columns, where, "", ";");

    QList<QString> columnNames;
    QList<int> columnWidths;
    int count = this->columnNames.count() - 8;
    for (int i = 0; i < count; i++) {
        columnNames << this->columnNames.at(i);
    }

    columnNames << "存储" << "小数点" << "报警延时";
    columnWidths << 40 << 90 << 120 << 90 << 50 << 90 << 60 << 60
                 << 45 << 45 << 50 << 45 << 45 << 45 << 45 << 45 << 60;

    QString name = "变量信息";
    DataContent dataContent;

    //pdf和xls需要设置文件名
    if (type == 1 || type == 2) {
        dataContent.fileName = fileName;
    }
    //xls需要设置表名
    if (type == 2) {
        dataContent.sheetName = name;
    }

    dataContent.title = name;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    dataContent.landscape = true;
    DataHelper::DataOut(dataContent);

    //插入日志记录+询问打开文件
    QString msg = QString("导出%1").arg(name);
    if (type == 0) {
        msg = QString("打印%1").arg(name);
    }
    DbQuery::addUserLog(msg);
    QUIHelper::openFile(fileName, msg);
}

void frmConfigNode::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigNode::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigNode::on_btnInput_clicked()
{
    AppFun::inputData(model, columnNames, "NodeInfo", "变量");
    on_btnSave_clicked();
}

void frmConfigNode::on_btnOutput_clicked()
{
    AppFun::outputData("NodeID asc", columnNames, "NodeInfo", "变量");
}

