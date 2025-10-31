#include "frmconfigtype.h"
#include "ui_frmconfigtype.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "datahelper.h"

frmConfigType::frmConfigType(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigType)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
}

frmConfigType::~frmConfigType()
{
    delete ui;
}

void frmConfigType::initForm()
{
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, false, true, false);
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后会立即应用");
}

void frmConfigType::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //指定表名
    model->setTable("TypeInfo");
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
    columnNames = AppData::TypeInfoColumns.split("|");
    columnWidths << 60 << 180 << 80 << 180 << 130 << 130;

    //挨个设置列名和列宽
    for (int i = 0; i < columnNames.count(); i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }
}

void frmConfigType::on_btnAdd_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    int typeID = model->index(count - 1, 0).data().toInt() + 1;
    QString deviceType = model->index(count - 1, 1).data().toString();
    QString nodeNumber = model->index(count - 1, 2).data().toString();
    QString nodeType = model->index(count - 1, 3).data().toString();
    QString nodeClass = model->index(count - 1, 4).data().toString();
    QString nodeSign = model->index(count - 1, 5).data().toString();

    //当前为第一行时候的默认值
    if (count == 0) {
        typeID = 1;
        deviceType = "FC-1003-8";
        nodeNumber = "8";
        nodeType = "FT-2104";
        nodeClass = "O2";
        nodeSign = "PPM";
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), typeID);
    model->setData(model->index(count, 1), deviceType);
    model->setData(model->index(count, 2), nodeNumber);
    model->setData(model->index(count, 3), nodeType);
    model->setData(model->index(count, 4), nodeClass);
    model->setData(model->index(count, 5), nodeSign);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigType::on_btnSave_clicked()
{
    //过滤判断是否有重复的,只有有一个重复就返回
    int count = model->rowCount();
    QStringList deviceTypes, nodeTypes, nodeClasss, nodeSigns;
    for (int i = 0; i < count; i++) {
        QString deviceType = model->index(i, 1).data().toString();
        if (!deviceType.isEmpty()) {
            if (deviceTypes.contains(deviceType)) {
                QUIHelper::showMessageBoxError("控制器类型不能重复,请重新输入!", 3);
                return;
            } else {
                deviceTypes << deviceType;
            }
        }

        QString nodeType = model->index(i, 3).data().toString();
        if (!nodeType.isEmpty()) {
            if (nodeTypes.contains(nodeType)) {
                QUIHelper::showMessageBoxError("变量类型不能重复,请重新输入!", 3);
                return;
            } else {
                nodeTypes << nodeType;
            }
        }

        QString nodeClass = model->index(i, 4).data().toString();
        if (!nodeClass.isEmpty()) {
            if (nodeClasss.contains(nodeClass)) {
                QUIHelper::showMessageBoxError("气体种类不能重复,请重新输入!", 3);
                return;
            } else {
                nodeClasss << nodeClass;
            }
        }

        QString nodeSign = model->index(i, 5).data().toString();
        if (!nodeSign.isEmpty()) {
            if (nodeSigns.contains(nodeSign)) {
                QUIHelper::showMessageBoxError("气体符号不能重复,请重新输入!", 3);
                return;
            } else {
                nodeSigns << nodeSign;
            }
        }
    }

    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();

        //分别判断那种改动过,改动过的才需要发送对应的信号
        bool deviceTypeChange = false;
        int deviceTypeCount = deviceTypes.count();
        if (DbData::TypeInfo_DeviceType.count() != deviceTypeCount) {
            deviceTypeChange = true;
        } else {
            for (int i = 0; i < deviceTypeCount; i++) {
                if (DbData::TypeInfo_DeviceType.at(i) != deviceTypes.at(i)) {
                    deviceTypeChange = true;
                    break;
                }
            }
        }

        bool nodeTypeChange = false;
        int nodeTypeCount = nodeTypes.count();
        if (DbData::TypeInfo_NodeType.count() != nodeTypeCount) {
            nodeTypeChange = true;
        } else {
            for (int i = 0; i < nodeTypeCount; i++) {
                if (DbData::TypeInfo_NodeType.at(i) != nodeTypes.at(i)) {
                    nodeTypeChange = true;
                    break;
                }
            }
        }

        bool nodeClassChange = false;
        int nodeClassCount = nodeClasss.count();
        if (DbData::TypeInfo_NodeClass.count() != nodeClassCount) {
            nodeClassChange = true;
        } else {
            for (int i = 0; i < nodeClassCount; i++) {
                if (DbData::TypeInfo_NodeClass.at(i) != nodeClasss.at(i)) {
                    nodeClassChange = true;
                    break;
                }
            }
        }

        bool nodeSignChange = false;
        int nodeSignCount = nodeSigns.count();
        if (DbData::TypeInfo_NodeSign.count() != nodeSignCount) {
            nodeSignChange = true;
        } else {
            for (int i = 0; i < nodeSignCount; i++) {
                if (DbData::TypeInfo_NodeSign.at(i) != nodeSigns.at(i)) {
                    nodeSignChange = true;
                    break;
                }
            }
        }

        DbQuery::loadTypeInfo();

        if (deviceTypeChange) {
            emit deviceTypeChanged();
        }

        if (nodeTypeChange) {
            emit nodeTypeChanged();
        }

        if (nodeClassChange) {
            emit nodeClassChanged();
        }

        if (nodeSignChange) {
            emit nodeSignChanged();
        }
    } else {
        model->database().rollback();
        QUIHelper::showMessageBoxError("保存信息失败,信息不能为空,请重新填写!");
    }
}

void frmConfigType::on_btnDelete_clicked()
{
    if (ui->tableView->currentIndex().row() < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的类型!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该类型吗?") == QMessageBox::Yes) {
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        model->submitAll();

        int count = model->rowCount();
        ui->tableView->setCurrentIndex(model->index(count - 1, 0));
    }
}

void frmConfigType::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigType::on_btnClear_clicked()
{
    int count = model->rowCount();
    if (count <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有类型信息吗?") == QMessageBox::Yes) {
        DbQuery::clearTypeInfo();
        model->select();
    }
}

void frmConfigType::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    QString flag = "pdf";
    if (type == 2) {
        flag = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("typeinfo_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    QString columns = "TypeID,DeviceType,NodeNumber,NodeType,NodeClass,NodeSign";
    QString where = "";
    QStringList content = DataCsv::getContent("TypeInfo", columns, where, "", ";");

    QString name = "类型信息";
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
    dataContent.landscape = false;
    DataHelper::DataOut(dataContent);

    //插入日志记录+询问打开文件
    QString msg = QString("导出%1").arg(name);
    if (type == 0) {
        msg = QString("打印%1").arg(name);
    }
    DbQuery::addUserLog(msg);
    QUIHelper::openFile(fileName, msg);
}

void frmConfigType::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigType::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigType::on_btnInput_clicked()
{
    AppFun::inputData(model, columnNames, "TypeInfo", "类型");
    on_btnSave_clicked();
}

void frmConfigType::on_btnOutput_clicked()
{
    AppFun::outputData("TypeID asc", columnNames, "TypeInfo", "类型");
}
