#include "frmconfigscada.h"
#include "ui_frmconfigscada.h"
#include "quihelper.h"
#include "selectwidget.h"
#include "devicemap.h"

#ifdef scada
#include "qtchinesename.h"
#include "qtobjectcontroller.h"
#endif

frmConfigScada::frmConfigScada(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigScada)
{
    ui->setupUi(this);
    this->initForm();
    this->initTable();
    QTimer::singleShot(100, this, SLOT(initPlugin()));
}

frmConfigScada::~frmConfigScada()
{
    delete ui;
}

bool frmConfigScada::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->centralwidget) {
        if (event->type() == QEvent::Drop) {
            //根据最后按下的控件位置生成控件
            QDropEvent *dropEvent = (QDropEvent *)event;
            int row = listNames.indexOf(pressedItem);
            if (row >= 0) {
                QPoint point = dropEvent->pos();
                point.setX(point.x() - ui->centralwidget->x());
                newWidget(row, point);
            }
        } else if (event->type() == QEvent::DragEnter) {
            //必须要设置这个event->accept();不然没法用拖曳,默认是禁用图标
            QDragEnterEvent *dragEvent = (QDragEnterEvent *)event;
            if(dragEvent->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
                dragEvent->setDropAction(Qt::CopyAction);
                dragEvent->accept();
            } else {
                dragEvent->ignore();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmConfigScada::initForm()
{    
#ifdef scada
    //初始化中英属性对照表,注释掉的话则全部是原生的控件的英文属性
    QtChineseName::loadFile(":/image/chinesename_property.txt");
    QtChineseName::loadFile(":/image/chinesename_control.txt");
    propertyView = new QtObjectController;
    propertyView->setObjectName("propertyView");
    ui->tab2->layout()->addWidget(propertyView);
#endif

    ui->frameXml->setProperty("flag", "navbtn");
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setFixedWidth(AppData::RightWidth);

    xmlName = qApp->applicationDirPath() + "/quc.xml";
    pressedItem = "";
    widgetIndex = -1;

    //允许拖曳接收
    ui->listWidget->setDragEnabled(true);
    ui->centralwidget->setAcceptDrops(true);
    ui->centralwidget->installEventFilter(this);

    //设置背景图片
    ui->centralwidget->setPixmap(DeviceMap::Instance()->getMapPix(AppData::CurrentImage));
}

void frmConfigScada::initTable()
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "属性" << "值";
    columnWidths << 103 << 60;

    int columnCount = columnNames.count();
    ui->tableWidget->setColumnCount(columnCount);
    for (int i = 0; i < columnCount; i++) {
        ui->tableWidget->setColumnWidth(i, columnWidths.at(i));
    }

    ui->tableWidget->setHorizontalHeaderLabels(columnNames);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    QUIHelper::initTableView(ui->tableWidget, AppData::RowHeight);
}

void frmConfigScada::initPlugin()
{
    //载入默认的插件,debug则加载带d结尾的文件
    QString appPath = qApp->applicationDirPath();
#ifdef QT_NO_DEBUG
    QString flag = "";
#else
    QString flag = "d";
#endif

#if defined(Q_OS_WIN)
    QString fileName = QString("%1/quc%2.dll").arg(appPath).arg(flag);
#elif defined(Q_OS_UNIX)
    QString fileName = QString("%1/libquc%2.so").arg(appPath).arg(flag);
#elif defined(Q_OS_MAC)
    QString fileName = QString("%1/libquc%2.dylib").arg(appPath).arg(flag);
#endif

    //载入默认的自定义控件插件
    loadPlugin(fileName);
    //载入默认的控件xml数据
    openFile(appPath + "/quc.xml");
    //自动滚动条滚到指定位置
    qApp->processEvents();
    ui->listWidget->verticalScrollBar()->setValue(0);
}

void frmConfigScada::loadPlugin(const QString &fileName)
{
    openPlugin(fileName);
}

void frmConfigScada::openPlugin(const QString &fileName)
{
#ifdef scada
    //先清空原有的控件
    qDeleteAll(listWidgets);
    listWidgets.clear();
    listNames.clear();
    ui->listWidget->clear();

    //加载自定义控件插件集合信息,包括获得类名+图标
    QPluginLoader loader(fileName);
    if (loader.load()) {
        QObject *plugin = loader.instance();

        //获取插件容器,然后逐个遍历容器找出单个插件
        QDesignerCustomWidgetCollectionInterface *interfaces = qobject_cast<QDesignerCustomWidgetCollectionInterface *>(plugin);
        if (interfaces)  {
            listWidgets = interfaces->customWidgets();
            int count = listWidgets.count();
            for (int i = 0; i < count; i++) {
                //取出图标和类名
                QIcon icon = listWidgets.at(i)->icon();
                QString className = listWidgets.at(i)->name();
                QListWidgetItem *item = new QListWidgetItem(ui->listWidget);

                //转换成对应的控件的中文名称
                item->setText(QtChineseName::getControlName(className));
                //真实类名设置到自定义用户数据
                item->setData(Qt::UserRole, className);
                //设置图标
                item->setIcon(QIcon(icon));
                //添加到队列方便查找
                listNames << className;
            }
        }

        //获取所有插件的类名
        const QObjectList objList = plugin->children();
        foreach (QObject *obj, objList) {
            QString className = obj->metaObject()->className();
            //qDebug() << className;
        }
    }
#endif
}

void frmConfigScada::openFile(const QString &fileName)
{
#ifdef scada
    //如果控件列表没有则不用继续
    if (ui->listWidget->count() == 0) {
        return;
    }

    //打开文件
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    //将文件填充到dom容器
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }

    file.close();

    listSelect.clear();
    listUserProperty.clear();
    xmlName = fileName;

    //先清空原有控件
    QList<QWidget *> widgets = ui->centralwidget->findChildren<QWidget *>();
    qDeleteAll(widgets);
    widgets.clear();

    //先判断根元素是否正确
    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() == "canvas") {
        QDomNode node = docElem.firstChild();
        QDomElement element = node.toElement();
        while(!node.isNull()) {
            //控件名称
            QString name = element.tagName();
            //取出当前控件在控件列表中的索引,如果不存在则意味着配置文件中的该控件不存在了
            int index = listNames.indexOf(name);
            if (index < 0) {
                continue;
            }

            //存储控件的坐标位置和宽度高度
            int x, y, width, height;
            //存储自定义控件属性
            QList<QPair<QString, QVariant> > propertys;
            //存储控件自定义属性
            QStringList userProperty;

            //节点名称不为空才继续
            if (!name.isEmpty()) {
                //遍历节点的属性名称和属性值
                QDomNamedNodeMap attrs = element.attributes();
                for (int i = 0; i < attrs.count(); i++) {
                    QDomNode node = attrs.item(i);
                    QString nodeName = node.nodeName();
                    QString nodeValue = node.nodeValue();
                    //qDebug() << name << nodeName << nodeValue;

                    //优先取出坐标+宽高属性,这几个属性不能通过设置弱属性实现
                    if (nodeName == "x") {
                        x = nodeValue.toInt();
                    } else if (nodeName == "y") {
                        y = nodeValue.toInt();
                    } else if (nodeName == "width") {
                        width = nodeValue.toInt();
                    } else if (nodeName == "height") {
                        height = nodeValue.toInt();
                    } else if (nodeName.startsWith("user-")) {
                        //取出user-开头的自定义属性
                        nodeName = nodeName.split("-").last();
                        userProperty << QString("%1|%2").arg(nodeName).arg(nodeValue);
                    } else {
                        QVariant value = QVariant(nodeValue);
                        //为了兼容Qt4,需要将颜色值的rgba分别取出来,因为Qt4不支持16进制字符串带透明度
                        //#6422a3a9 这种格式依次为 argb 带了透明度的才需要特殊处理
                        if (nodeValue.startsWith("#") && nodeValue.length() == 9) {
                            bool ok;
                            int alpha = nodeValue.mid(1, 2).toInt(&ok, 16);
                            int red = nodeValue.mid(3, 2).toInt(&ok, 16);
                            int green = nodeValue.mid(5, 2).toInt(&ok, 16);
                            int blue = nodeValue.mid(7, 2).toInt(&ok, 16);
                            value = QColor(red, green, blue, alpha);
                        }

                        propertys.append(qMakePair(nodeName, value));
                    }
                }
            }

            //qDebug() << name << x << y << width << height;

            //根据不同的控件类型实例化控件
            int countWidget = listWidgets.count();
            int countProperty = propertys.count();
            for (int i = 0; i < countWidget; i++) {
                QString className = listWidgets.at(i)->name();
                if (name == className) {
                    //生成对应的控件
                    QWidget *widget = createWidget(i);
                    //逐个设置自定义控件的属性
                    for (int j = 0; j < countProperty; j++) {
                        QPair<QString, QVariant> property = propertys.at(j);
                        QString name = property.first;
                        QVariant value = property.second;
                        widget->setProperty(name.toStdString().c_str(), value);
                    }

                    //设置控件坐标及宽高
                    widget->setGeometry(x, y, width, height);
                    //实例化选中窗体跟随控件一起
                    newSelect(widget, userProperty);
                    break;
                }
            }

            //移动到下一个节点
            node = node.nextSibling();
            element = node.toElement();
        }
    }
#endif
}

void frmConfigScada::saveFile(const QString &fileName)
{
    //如果控件列表没有则不用继续
    if (ui->listWidget->count() == 0) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        return;
    }

    //以流的形式输出文件
    QTextStream stream(&file);

    //构建xml数据
    QStringList list;

    //添加固定头部数据
    list << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    //添加canvas主变量,保存宽高和背景图片,还可以自行添加其他属性
    list << QString("<canvas width=\"%1\" height=\"%2\" image=\"%3\">")
            .arg(ui->centralwidget->width()).arg(ui->centralwidget->height()).arg("bg.jpg");

    //从容器中找到所有控件,根据控件的类名保存该类的所有属性
    QList<QWidget *> widgets = ui->centralwidget->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        const QMetaObject *metaObject = widget->metaObject();
        QString className = metaObject->className();

        //如果当前控件的父类不是主窗体则无需导出,有些控件有子控件无需导出
        if (widget->parent() != ui->centralwidget || className == "SelectWidget") {
            continue;
        }

        //逐个存储自定义控件属性
        //metaObject->propertyOffset()表示当前控件的属性开始索引,0开始的是父类的属性
        QStringList values;
        int index = metaObject->propertyOffset();
        int count = metaObject->propertyCount();
        for (int i = index; i < count; i++) {
            QMetaProperty property = metaObject->property(i);
            QString nodeName = property.name();
            QVariant variant = property.read(widget);
            QString typeName = variant.typeName();
            QString nodeValue = variant.toString();

            //如果是颜色值则取出透明度一起,颜色值toString在Qt4中默认不转透明度
            if (typeName == "QColor") {
                QColor color = variant.value<QColor>();
                if (color.alpha() < 255) {
                    //Qt4不支持HexArgb格式的字符串,需要挨个取出来拼接
                    //nodeValue = color.name(QColor::HexArgb);
                    QString alpha = QString("%1").arg(color.alpha(), 2, 16, QChar('0'));
                    QString red = QString("%1").arg(color.red(), 2, 16, QChar('0'));
                    QString green = QString("%1").arg(color.green(), 2, 16, QChar('0'));
                    QString blue = QString("%1").arg(color.blue(), 2, 16, QChar('0'));
                    nodeValue = QString("#%1%2%3%4").arg(alpha).arg(red).arg(green).arg(blue);
                }
            }

            //枚举值要特殊处理,需要以字符串形式写入,不然存储到配置文件数据为int
            if (property.isEnumType()) {
                QMetaEnum enumValue = property.enumerator();
                nodeValue = enumValue.valueToKey(nodeValue.toInt());
            }

            values << QString("%1=\"%2\"").arg(nodeName).arg(nodeValue);
            //qDebug() << nodeName << nodeValue << variant;
        }

        //找到当前控件对应的索引
        index = -1;
        count = listSelect.count();
        for (int i = 0; i < count; i++) {
            if (listSelect.at(i)->getWidget() == widget) {
                index = i;
                break;
            }
        }

        //可以用下面方法列出所有的用户属性,然后取值,本程序已经用 listUserProperty 存储了
        //qDebug() << widget->dynamicPropertyNames();

        //逐个存储控件的用户属性
        QStringList userProperty = listUserProperty.at(index);
        count = userProperty.count();
        for (int i = 0; i < count; i++) {
            QStringList list = userProperty.at(i).split("|");
            values << QString("user-%1=\"%2\"").arg(list.at(0)).arg(list.at(1));
        }

        //逐个添加界面上的控件的属性
        QString geometry = QString("x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\"").arg(widget->x()).arg(widget->y()).arg(widget->width()).arg(widget->height());
        QString str = QString("\t<%1 %2 %3/>").arg(className).arg(geometry).arg(values.join(" "));
        list << str;
    }

    //添加固定尾部数据
    list << "</canvas>";

    //写入文件
    QString data = list.join("\n");
    stream << data;
    file.close();
}

QWidget *frmConfigScada::createWidget(int row)
{
    //生成对应的控件,单独提取出来一个函数是为了方便后期在这里增加一些处理
    QWidget *widget;
#ifdef scada
    widget = listWidgets.at(row)->createWidget(ui->centralwidget);
#endif
    return widget;
}

void frmConfigScada::newSelect(QWidget *widget, const QStringList &property)
{
    //实例化选中窗体跟随控件一起
    SelectWidget *select = new SelectWidget(ui->centralwidget);
    connect(select, SIGNAL(widgetPressed(QWidget *)), this, SLOT(widgetPressed(QWidget *)));
    connect(select, SIGNAL(widgetDelete(QWidget *)), this, SLOT(widgetDelete(QWidget *)));
    select->setWidget(widget);
    listSelect << select;
    listUserProperty << property;

    //将用户属性绑定到对应控件
    widgetProperty(listSelect.count() - 1);
}

void frmConfigScada::newWidget(int row, const QPoint &point)
{
    //列表按照同样的索引生成的,所以这里直接对该行的索引就行
    QWidget *widget = createWidget(row);
    widget->move(point);
    widget->resize(widget->sizeHint());

    //实例化选中窗体跟随控件一起
    newSelect(widget, QStringList());

    //立即执行获取焦点以及设置属性
    widgetPressed(widget);
}

void frmConfigScada::clearFocus()
{
    //将原有焦点窗体全部设置成无焦点
    foreach (SelectWidget *w, listSelect) {
        w->setDrawPoint(false);
    }
}

void frmConfigScada::widgetPressed(QWidget *widget)
{
    //清空所有控件的焦点
    clearFocus();

    //设置当前按下的控件有焦点
    //这里后面增加按下自动移到最前面显示
    int widgetIndex = -1;
    for (int i = 0; i < listSelect.count(); i++) {
        SelectWidget *w = listSelect.at(i);
        if (w->getWidget() == widget) {
            w->setDrawPoint(true);
            widgetIndex = i;
            break;
        }
    }

    //设置自动加载该控件的所有属性
#ifdef scada
    propertyView->setObject(widget);
#endif
    //加载用户属性存入表格
    newProperty(widgetIndex);
}

void frmConfigScada::widgetDelete(QWidget *widget)
{
    //移除对应的控件
    for (int i = 0; i < listSelect.count(); i++) {
        if (listSelect.at(i)->getWidget() == widget) {
            listSelect.removeAt(i);
            listUserProperty.removeAt(i);
            break;
        }
    }

    //清空右侧的用户属性
    on_btnClear_clicked();
}

void frmConfigScada::newProperty(int widgetIndex)
{
    //加载控件的自定义属性到表格,在xml数据为user-开头的
    this->widgetIndex = widgetIndex;
    QStringList userProperty = listUserProperty.at(widgetIndex);
    int rowCount = userProperty.count();
    ui->tableWidget->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++) {
        QStringList list = userProperty.at(i).split("|");
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(list.at(0)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(list.at(1)));
    }
}

void frmConfigScada::widgetProperty(int widgetIndex)
{
    //将自定义属性设置到控件中,这步可以考虑是否需要
    QStringList userProperty = listUserProperty.at(widgetIndex);
    QWidget *widget = listSelect.at(widgetIndex)->getWidget();
    foreach (QString property, userProperty) {
        QStringList list = property.split("|");
        QString name = list.at(0);
        QVariant value = list.at(1);
        widget->setProperty(name.toStdString().c_str(), value);
    }
}

void frmConfigScada::on_btnInput_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开控件配置信息", qApp->applicationDirPath(), "xml文件(*.xml)");
    if (!fileName.isEmpty()) {
        openFile(fileName);
    }
}

void frmConfigScada::on_btnOutput_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存控件配置信息", qApp->applicationDirPath() + "/quc.xml", "xml文件(*.xml)");
    if (!fileName.isEmpty()) {
        //如果拓展名确实则自动加上,在linux需要手动填写拓展名
        if (!fileName.endsWith("xml")) {
            fileName = fileName + ".xml";
        }

        saveFile(fileName);
    }
}

void frmConfigScada::on_listWidget_itemDoubleClicked()
{
    //根据不同的类名实例化不同的控件
    int row = ui->listWidget->currentRow();
    if (row >= 0) {
        newWidget(row, QPoint(10, 10));
    }
}

void frmConfigScada::on_listWidget_itemPressed(QListWidgetItem *item)
{
    pressedItem = item->data(Qt::UserRole).toString();
}

void frmConfigScada::on_btnAdd_clicked()
{
    int count = ui->tableWidget->rowCount() + 1;
    ui->tableWidget->setRowCount(count);
}

void frmConfigScada::on_btnDelete_clicked()
{
    int row = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(row);
}

void frmConfigScada::on_btnClear_clicked()
{
    ui->tableWidget->setRowCount(0);
}

void frmConfigScada::on_btnSave_clicked()
{
    //挨个遍历行列,检查属性名称或者值为空的,严格要求不能为空
    QStringList userProperty;
    int count = ui->tableWidget->rowCount();
    for (int i = 0; i < count; i++) {
        QTableWidgetItem *itemName = ui->tableWidget->item(i, 0);
        QTableWidgetItem *itemValue = ui->tableWidget->item(i, 1);
        if (itemName == 0 || itemValue == 0) {
            QMessageBox::critical(this, "错误", "用户属性中的属性名和值不能为空!");
            return;
        }

        QString name = itemName->text().trimmed();
        QString value = itemValue->text().trimmed();
        if (name.isEmpty() || value.isEmpty()) {
            QMessageBox::critical(this, "错误", "用户属性中的属性名和值不能为空!");
            return;
        }

        userProperty << QString("%1|%2").arg(name).arg(value);
    }

    //更新对应的用户属性列表+重新赋值+立即存储
    if (widgetIndex >= 0) {
        listUserProperty[widgetIndex] = userProperty;
        widgetProperty(widgetIndex);
    }

    saveFile(xmlName);
}

