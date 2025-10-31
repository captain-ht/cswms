#include "dataxls.h"
#include "datahelper.h"

DataXls *DataXls::dataXls = 0;
void DataXls::saveXls(const DataContent &dataContent)
{
    //没有实例化过则先实例化
    if (dataXls == 0) {
        dataXls = new DataXls;
    }

    //没有内容数据则不用处理
    if (dataContent.content.count() == 0) {
        return;
    }

    dataXls->init();
    dataXls->setDataContent(dataContent);
    dataXls->open();
    dataXls->takeContent();
    dataXls->close();
}

void DataXls::saveXls(const QString &fileName, const QString &sheetName, const QString &title,
                      const QList<QString> &columnNames, const QList<int> &columnWidths,
                      const QStringList &content)
{
    DataContent dataContent;
    dataContent.fileName = fileName;
    dataContent.sheetName = sheetName;
    dataContent.title = title;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    DataXls::saveXls(dataContent);
}

DataXls::DataXls(QObject *parent) : QThread(parent)
{
    this->init();
}

DataXls::~DataXls()
{
    this->stop();
    this->wait();
}

void DataXls::run()
{
    time.restart();
    while (!stopped) {
        takeContent();
        //可以自行调整下面这个间隔以便保证完整输出
        //发现有些电脑需要延时久一点
        msleep(10);this->deleteLater();
    }

    stopped = false;
}

QString DataXls::getUseTime()
{
    return QString::number((double)time.elapsed() / 1000, 'f', 3);
}

void DataXls::appendHead()
{
    //固定头部信息
    stream << "<?xml version=\"1.0\"?>\n";
    stream << "<?mso-application progid=\"Xls.Sheet\"?>\n";
    stream << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    stream << " xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
    stream << " xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
    stream << " xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    stream << " xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n";
}

void DataXls::appendDocument()
{
    //文档信息
    stream << " <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n";
    stream << "  <LastAuthor></LastAuthor>\n";
    stream << QString("  <Created>%1</Created>\n").arg(DATETIME);
    stream << "  <Version>11.6360</Version>\n";
    stream << " </DocumentProperties>\n";

    stream << " <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n";
    stream << "  <AllowPNG/>\n";
    stream << " </OfficeDocumentSettings>\n";

    stream << " <XlsWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n";
    stream << "  <WindowHeight>9795</WindowHeight>\n";
    stream << "  <WindowWidth>21435</WindowWidth>\n";
    stream << "  <WindowTopX>120</WindowTopX>\n";
    stream << "  <WindowTopY>75</WindowTopY>\n";
    stream << "  <WindowTopY>75</WindowTopY>\n";
    stream << "  <ProtectWindows>False</ProtectWindows>\n";
    stream << " </XlsWorkbook>\n";
}

void DataXls::appendBorder()
{
    //如果边框宽度为0则不显示边框
    int borderWidth = dataContent.borderWidth;
    if (borderWidth > 0) {
        stream << "  <Borders>\n";
        stream << QString("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << QString("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << QString("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << QString("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << "  </Borders>\n";
    }
}

void DataXls::appendStyle()
{
    //样式表
    stream << " <Styles>\n";
    stream << "  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n";
    stream << "   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n";
    stream << "   <Borders/>\n";
    stream << "   <Font ss:FontName=\"Microsoft Yahei\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#000000\"/>\n";
    stream << "   <Interior/>\n";
    stream << "   <NumberFormat/>\n";
    stream << "   <Protection/>\n";
    stream << "  </Style>\n";

    //标题样式--居中大号加粗字体
    stream << "  <Style ss:ID=\"s_title\">\n";
    stream << "   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n";
    appendBorder();
    stream << "   <Font ss:FontName=\"Microsoft Yahei\" x:CharSet=\"134\" ss:Size=\"15\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n";
    stream << "  </Style>\n";

    //标题样式--左对齐普通字体
    stream << "  <Style ss:ID=\"s_subtitle\">\n";
    stream << "   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Center\"/>\n";
    appendBorder();
    stream << "   <Font ss:FontName=\"Microsoft Yahei\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#000000\" ss:Bold=\"0\"/>\n";
    stream << "  </Style>\n";

    //正文样式--默认颜色
    stream << "  <Style ss:ID=\"s_normal\">\n";
    stream << "   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n";
    appendBorder();
    stream << " </Style>\n";

    //正文样式--文字红色
    stream << "  <Style ss:ID=\"s_red\">\n";
    appendBorder();
    stream << QString("  <Font ss:FontName=\"Microsoft Yahei\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"%1\" ss:Bold=\"0\"/>\n").arg(dataContent.checkColor);
    stream << " </Style>\n";

    //生成多个带背景色的样式
    if (dataContent.randomColor) {
        QStringList colorName;
        colorName << "#FF99CC" << "#FFCC99" << "#FF8080" << "#8DB4E2" << "#EBF1DE" << "#CC99FF" << "#99CC00" << "#99CCFF" << "#FFFF00" << "#FF6600";
        for (int i = 0; i < 10; i++) {
            stream << QString("  <Style ss:ID=\"s_color_%1\">\n").arg(i);
            stream << "   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n";
            appendBorder();
            stream << QString("   <Interior ss:Color=\"%1\" ss:Pattern=\"Solid\"/>\n").arg(colorName.at(i));
            stream << " </Style>\n";
        }
    }

    stream << " </Styles>\n";
}

void DataXls::appendTable()
{
    //工作表名称
    stream << QString(" <Worksheet ss:Name=\"%1\">\n").arg(dataContent.sheetName);

    //表格开始
    stream << QString("  <Table ss:ExpandedColumnCount=\"%1\" x:FullColumns=\"1\"\n").arg(columnCount);
    stream << "   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n";

    //设置字段宽度
    for (int i = 0; i < columnCount; i++) {
        stream << QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(dataContent.columnWidths.at(i));
    }

    //表格标题,如果没有则不输出
    if (!dataContent.title.isEmpty()) {
        stream << "   <Row ss:AutoFitHeight=\"0\" ss:Height=\"22\">\n";
        stream << QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_title\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(dataContent.title);
        stream << "   </Row>";
    }

    //表格副标题,如果没有则不输出
    if (!dataContent.subTitle.isEmpty()) {
        stream << "   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n";
        stream << QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(dataContent.subTitle);
        stream << "   </Row>";
    }
}

void DataXls::appendColumn()
{
    //逐个添加字段名称
    if (columnCount > 0) {
        stream << "   <Row ss:AutoFitHeight=\"0\">\n";
        for (int i = 0; i < columnCount; i++) {
            stream << QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n").arg(dataContent.columnNames.at(i));
        }
        stream << "   </Row>\n";
    }
}

void DataXls::appendContent(const QStringList &listContent)
{
    //逐个添加数据
    int rowCount = listContent.count();
    for (int i = 0; i < rowCount; i++) {
        stream << "   <Row ss:AutoFitHeight=\"0\">\n";
        QString temp = listContent.at(i);
        QStringList value = temp.split(dataContent.separator);

        if (dataContent.randomColor) {
            //如果启用了随机背景颜色则随机取背景颜色样式
            QString style = "s_normal";
            static int styleIndex = 0;

            //如果没有指定列随机背景颜色则对整行
            for (int j = 0; j < columnCount; j++) {
                if (dataContent.colorColumn.count() == 0) {
                    style = QString("s_color_%1").arg(styleIndex);
                } else {
                    if (dataContent.colorColumn.contains(j)) {
                        style = QString("s_color_%1").arg(styleIndex);
                    } else {
                        style = "s_normal";
                    }
                }

                stream << QString("    <Cell ss:StyleID=\"%1\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(style).arg(value.at(j));
            }

            styleIndex++;
            if (styleIndex > 9) {
                styleIndex = 0;
            }
        } else {
            //如果启用了过滤数据,则将符合条件的数据突出颜色显示
            bool exist = false;
            if (dataContent.checkColumn >= 0) {
                exist = DataHelper::CheckColumn(value.at(dataContent.checkColumn), dataContent.checkType, dataContent.checkValue);
            }

            //用这行采用默认样式,文件体积可以减少一部分
            //QString strCell = QString("    <Cell><Data ss:Type=\"String\">");
            QString strCell = QString("    <Cell ss:StyleID=\"%1\"><Data ss:Type=\"String\">").arg(exist ? "s_red" : "s_normal");
            for (int j = 0; j < columnCount; j++) {
                stream << QString("%1%2</Data></Cell>\n").arg(strCell).arg(value.at(j));
            }
        }

        stream << "   </Row>\n";
    }
}

void DataXls::takeContent()
{
    //行数不对则返回
    int count = dataContent.content.count();
    if (count <= 0) {
        return;
    }

    //取出当前所有行内容
    mutex.lock();
    QStringList listContent = dataContent.content;
    QStringList listSubTitle1 = dataContent.subTitle1;
    QStringList listSubTitle2 = dataContent.subTitle2;
    dataContent.content.clear();
    dataContent.subTitle1.clear();
    dataContent.subTitle2.clear();
    mutex.unlock();

    QElapsedTimer time;
    time.start();

    int contentCount = listContent.count();
    int subTitle1Count = listSubTitle1.count();
    int subTitle2Count = listSubTitle2.count();

    if (subTitle1Count > 0 && subTitle2Count > 0 && subTitle1Count == subTitle2Count) {
        //循环添加子标题/字段名/内容
        for (int i = 0; i < contentCount; i++) {
            //加个空行隔开
            stream << "   <Row ss:AutoFitHeight=\"0\">\n";
            stream << "   </Row>";
            QString strRow = "   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n";
            QString strCell = QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">").arg(columnCount - 1);

            //子标题1
            if (subTitle1Count > i) {
                if (!listSubTitle1.at(i).isEmpty()) {
                    stream << strRow;
                    stream << QString("%1%2</Data></Cell>\n").arg(strCell).arg(listSubTitle1.at(i));
                    stream << "   </Row>";
                }
            }

            //子标题2
            if (subTitle2Count > i) {
                if (!listSubTitle2.at(i).isEmpty()) {
                    stream << strRow;
                    stream << QString("%1%2</Data></Cell>\n").arg(strCell).arg(listSubTitle2.at(i));
                    stream << "   </Row>";
                }
            }

            //逐个添加字段名称
            appendColumn();
            //逐行添加该分类下的内容
            appendContent(listContent.at(i).split(dataContent.subSeparator));
        }
    } else {
        appendContent(listContent);
    }

    emit appendFinshed(count, time.elapsed());
}

void DataXls::appendEnd()
{
    //表格结尾
    stream << "  </Table>\n";
    //固定结尾格式
    stream << "  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n";
    stream << "   <PageSetup>\n";
    stream << "    <Header x:Margin=\"0.3\"/>\n";
    stream << "    <Footer x:Margin=\"0.3\"/>\n";
    stream << "    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n";
    stream << "   </PageSetup>\n";
    stream << "   <Unsynced/>\n";
    stream << "   <Selected/>\n";
    stream << "   <ProtectObjects>False</ProtectObjects>\n";
    stream << "   <ProtectScenarios>False</ProtectScenarios>\n";
    stream << "  </WorksheetOptions>\n";
    stream << " </Worksheet>\n";
    stream << "</Workbook>\n";
}

void DataXls::setDataContent(const DataContent &dataContent)
{
    this->dataContent = dataContent;
}

void DataXls::init()
{
    stopped = false;
}

void DataXls::open()
{
    //校验字段名称和宽度,个数不能为0,而且字段名称的个数要和字段宽度的个数一致
    columnCount = dataContent.columnNames.count();
    int widthCount = dataContent.columnWidths.count();
    if (columnCount == 0 || widthCount == 0 || columnCount != widthCount) {
        return;
    }
    if (dataContent.fileName.isEmpty()) {
        return;
    }
    if (file.isOpen()) {
        file.close();
    }

    //指定文件名称
    file.setFileName(dataContent.fileName);
    if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
        return;
    }

    //设置流对象及编码
    stream.setDevice(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    stream.setCodec("utf-8");
#endif

    //输出固定头部信息
    appendHead();
    //输出文档信息
    appendDocument();
    //输出边框样式
    appendStyle();
    //输出表结构
    appendTable();

    //输出字段名称
    if (dataContent.subTitle1.count() == 0) {
        appendColumn();
    }
}

void DataXls::close()
{
    if (file.isOpen()) {
        appendEnd();
        file.close();
    }
}

void DataXls::stop()
{
    stopped = true;
}

void DataXls::append(const QString &content, const QString &subTitle1, const QString &subTitle2)
{
    if (content.isEmpty()) {
        return;
    }

    mutex.lock();
    dataContent.content << content;
    if (!subTitle1.isEmpty()) {
        dataContent.subTitle1 << subTitle1;
    }
    if (!subTitle2.isEmpty()) {
        dataContent.subTitle2 << subTitle2;
    }
    mutex.unlock();
}
