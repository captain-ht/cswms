#include "frmconfiguser.h"
#include "ui_frmconfiguser.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "datahelper.h"

frmConfigUser::frmConfigUser(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigUser)
{
    ui->setupUi(this);
    this->initForm();
}

frmConfigUser::~frmConfigUser()
{
    delete ui;
}

void frmConfigUser::showEvent(QShowEvent *)
{
    //保证只加载一次
    if (!isLoad) {
        isLoad = true;
        this->initData();
        this->initIcon();
    }

    model->select();
}

void frmConfigUser::setDelegate(const QList<int> &delegateWidth, const QList<QString> &delegateType, const QList<QStringList> &delegateValue)
{
    this->delegateWidth = delegateWidth;
    this->delegateType = delegateType;
    this->delegateValue = delegateValue;
}

void frmConfigUser::initForm()
{
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后重启应用");

    //默认委托类型和值
    isLoad = false;
    for (int i = 0; i < AppData::PermissionName.count(); ++i) {
        delegateWidth << 50;
        delegateType << "QCheckBox";
        delegateValue << QStringList();
    }

    //初始化通用的表格属性
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, false, true);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmConfigUser::initIcon()
{
    //挨个设置按钮图标
    AppNav::setIconBtn(ui->btnAdd, 0xf067);
    AppNav::setIconBtn(ui->btnSave, 0xf0c7);
    AppNav::setIconBtn(ui->btnDelete, 0xf00d);
    AppNav::setIconBtn(ui->btnReturn, 0xf122);
    AppNav::setIconBtn(ui->btnClear, 0xf1f8);
    AppNav::setIconBtn(ui->btnInput, 0xf063);
    AppNav::setIconBtn(ui->btnOutput, 0xf062);
    AppNav::setIconBtn(ui->btnPrint, 0xf02f);
    AppNav::setIconBtn(ui->btnXls, 0xf1c3);

    //重新设置委托列的颜色
    foreach (DbDelegate *delegate, delegates) {
        delegate->setTextColor(QUIConfig::TextColor);
        delegate->setSelectBgColor(QUIConfig::NormalColorStart);
    }
}

void frmConfigUser::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //指定表名
    model->setTable("UserInfo");
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
    columnNames << "用户名称" << "用户密码" << "用户类型";
    columnWidths << 100 << 120 << 80;

    columnNames << AppData::PermissionName;
    for (int i = 0; i < AppData::PermissionName.count(); ++i) {
        columnWidths << delegateWidth.at(i);
    }

    columnNames << "备注";
    columnWidths << 60;

    //特殊分辨率重新设置列宽
    int count = columnNames.count();
    if (QUIHelper::deskWidth() >= 1920) {
        for (int i = 0; i < count; i++) {
            columnWidths[i] += 30;
        }
    }

    //挨个设置列名和列宽
    for (int i = 0; i < count; i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //用户密码委托
    DbDelegate *d_txt_userPwd = new DbDelegate(this);
    d_txt_userPwd->setDelegateType("QLineEdit");
    d_txt_userPwd->setDelegatePwd(true);
    d_txt_userPwd->setDelegateColumn(1);
    ui->tableView->setItemDelegateForColumn(1, d_txt_userPwd);

    //用户类型委托
    QStringList userType;
    userType << "操作员" << "管理员";
    DbDelegate *d_cbox_userType = new DbDelegate(this);
    d_cbox_userType->setDelegateType("QComboBox");
    d_cbox_userType->setDelegateValue(userType);
    ui->tableView->setItemDelegateForColumn(2, d_cbox_userType);

    //权限委托
    int index = 3;
    count = AppData::PermissionName.count() + index;
    for (int i = index; i < count; i++) {
        //取出委托类型和值
        QString type = delegateType.at(i - index);
        QStringList value = delegateValue.at(i - index);
        DbDelegate *delegate = new DbDelegate(this);
        delegate->setDelegateType(type);

        if (type == "QCheckBox") {
            delegate->setDelegateColumn(i);
            delegate->setCheckBoxText("启用", "禁用");
        } else if (type == "QComboBox") {
            delegate->setDelegateValue(value);
        }

        ui->tableView->setItemDelegateForColumn(i, delegate);
        delegates << delegate;
    }

    //用来切换样式自动改变颜色
    delegates << d_txt_userPwd;
    delegates << d_cbox_userType;
}

void frmConfigUser::on_btnAdd_clicked()
{
    //插入一行
    int count = model->rowCount();
    model->insertRow(count);

    //规则清晰直接用循环来赋值
    int columns = columnNames.count();
    for (int i = 0; i < columns; ++i) {
        //获取上一行的对应列的数据
        QString data = model->index(count - 1, i).data().toString();
        //设置新增加的行默认值
        model->setData(model->index(count, i), data);
    }

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigUser::on_btnSave_clicked()
{
    //开启数据库事务提交数据
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
    } else {
        //提交失败则回滚事务并打印错误信息
        model->database().rollback();
        qDebug() << TIMEMS << model->database().lastError();
        QUIHelper::showMessageBoxError("保存信息失败,请重新填写!");
    }

    //有些数据库需要主动查询一下不然是空白的比如odbc数据源
    model->select();
}

void frmConfigUser::on_btnDelete_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的用户!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该用户吗? 删除后不能恢复!") == QMessageBox::Yes) {
        QString userName = model->index(row, 0).data().toString();
        if (userName == "admin") {
            QUIHelper::showMessageBoxError("管理员 [admin] 不能被删除!", 3);
            return;
        }

        model->removeRow(row);
        model->submitAll();
        ui->tableView->setCurrentIndex(model->index(model->rowCount() - 1, 0));
    }
}

void frmConfigUser::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigUser::on_btnClear_clicked()
{
    if (model->rowCount() <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有用户信息吗?") == QMessageBox::Yes) {
        DbQuery::clearUserInfo();
        model->select();
    }
}

void frmConfigUser::on_btnInput_clicked()
{
    AppFun::inputData(model, columnNames, "UserInfo", "用户");
    on_btnSave_clicked();
}

void frmConfigUser::on_btnOutput_clicked()
{
    AppFun::outputData("UserName asc", columnNames, "UserInfo", "用户");
}

void frmConfigUser::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    QString flag = "pdf";
    if (type == 2) {
        flag = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("userinfo_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    QString columns = "*";
    QString where = "order by UserName asc";
    QStringList content = DataCsv::getContent("UserInfo", columns, where, "", ";");

    QString name = "用户信息";
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

void frmConfigUser::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigUser::on_btnXls_clicked()
{
    dataout(2);
}

