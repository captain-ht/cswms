#include "datahelper.h"
#include "dataxls.h"
#include "dataprint.h"

bool DataHelper::CheckColumn(const QString &sourceValue, const QString &checkType, const QString &checkValue)
{
    bool exist = false;

    //转成 QByteArray 比较就涵盖了所有数据类型
    //最开始用的 QVariant 后面发现Qt4不支持
#if 0
    QVariant sourceValue1 = sourceValue;
    QVariant checkValue1 = checkValue;

    //checkType如果后面带了数据类型(==|int >|double)则转为对应的数据类型
    QString checkType1 = checkType;
    QStringList list = checkType.split("|");
    if (list.count() == 2) {
        //将正确的比较类型取出来
        checkType1 = list.first();
        //可以自行增加其他类型
        QString type = list.last();
        if (type == "int") {
            sourceValue1 = sourceValue.toInt();
            checkValue1 = checkValue.toInt();
        } else if (type == "float") {
            sourceValue1 = sourceValue.toFloat();
            checkValue1 = checkValue.toFloat();
        } else if (type == "double") {
            sourceValue1 = sourceValue.toDouble();
            checkValue1 = checkValue.toDouble();
        }
    }

    if (checkType1 == "==") {
        exist = (sourceValue1 == checkValue1);
    } else if (checkType1 == ">") {
        exist = (sourceValue1 > checkValue1);
    } else if (checkType1 == ">=") {
        exist = (sourceValue1 >= checkValue1);
    } else if (checkType1 == "<") {
        exist = (sourceValue1 < checkValue1);
    } else if (checkType1 == "<=") {
        exist = (sourceValue1 <= checkValue1);
    } else if (checkType1 == "!=") {
        exist = (sourceValue1 != checkValue1);
    } else if (checkType1 == "contains") {
        exist = sourceValue.contains(checkValue);
    }
#else
    QByteArray sourceValue1;
    QByteArray checkValue1;

    //checkType如果后面带了数据类型(==|int >|double)则转为对应的数据类型
    QString checkType1 = checkType;
    QStringList list = checkType.split("|");
    if (list.count() == 2) {
        sourceValue1.clear();
        checkValue1.clear();
        //将正确的比较类型取出来
        checkType1 = list.first();
        //可以自行增加其他类型
        QString type = list.last();
        if (type == "int") {
            sourceValue1.append(sourceValue.toInt());
            checkValue1.append(checkValue.toInt());
        } else if (type == "float") {
            sourceValue1.append(sourceValue.toFloat());
            checkValue1.append(checkValue.toFloat());
        } else if (type == "double") {
            sourceValue1.append(sourceValue.toDouble());
            checkValue1.append(checkValue.toDouble());
        } else {
            sourceValue1.append(sourceValue.toUtf8());
            checkValue1.append(checkValue.toUtf8());
        }
    } else {
        sourceValue1.append(sourceValue.toUtf8());
        checkValue1.append(checkValue.toUtf8());
    }

    if (checkType1 == "==") {
        exist = (sourceValue1 == checkValue1);
    } else if (checkType1 == ">") {
        exist = (sourceValue1 > checkValue1);
    } else if (checkType1 == ">=") {
        exist = (sourceValue1 >= checkValue1);
    } else if (checkType1 == "<") {
        exist = (sourceValue1 < checkValue1);
    } else if (checkType1 == "<=") {
        exist = (sourceValue1 <= checkValue1);
    } else if (checkType1 == "!=") {
        exist = (sourceValue1 != checkValue1);
    } else if (checkType1 == "contains") {
        exist = sourceValue.contains(checkValue);
    }
#endif

    //qDebug() << TIMEMS << exist << sourceValue << checkType << checkValue;
    return exist;
}

void DataHelper::Init()
{
    //没有实例化过则先实例化
    if (DataPrint::dataPrint == 0) {
        DataPrint::dataPrint = new DataPrint;
    }
    if (DataXls::dataXls == 0) {
        DataXls::dataXls = new DataXls;
    }
}

void DataHelper::DataOut(const DataContent &dataContent)
{
    //判断规则
    //存在 fileName+sheetName 则表示导出 xls
    //存在 fileName 则表示导出 pdf
    //其余表示打印
    if (!dataContent.sheetName.isEmpty()) {
        DataXls::saveXls(dataContent);
    } else if (!dataContent.fileName.isEmpty()) {
        DataPrint::savePdf(dataContent);
    } else {
        DataPrint::print(dataContent);
    }
}

void DataHelper::DataOut(const QString &fileName, const QString &sheetName, const QString &title,
                         const QList<QString> &columnNames, const QList<int> &columnWidths,
                         const QStringList &content, bool landscape)
{
    DataContent dataContent;
    dataContent.fileName = fileName;
    dataContent.sheetName = sheetName;
    dataContent.title = title;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    dataContent.landscape = landscape;
    DataOut(dataContent);
}
