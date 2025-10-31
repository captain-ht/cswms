#include "datacsv.h"

QString DataCsv::CsvFilter = QString::fromUtf8("保存文件(*.csv)");
QString DataCsv::CsvExtension = ".csv";
QString DataCsv::CsvSpliter = ",";

QStringList DataCsv::getContent(const QString &tableName,
                                const QString &columnNames,
                                const QString &whereSql,
                                const QString &title,
                                const QString &spliter)
{
    QStringList content;

    //如果标题字段名称不为空则先添加标题
    if (!title.isEmpty()) {
        content << title;
    }

    QString sql = QString("select %1 from %2 %3").arg(columnNames).arg(tableName).arg(whereSql);
    QSqlQuery query;
    query.exec(sql);

    int columnCount = query.record().count();
    while (query.next()) {
        QStringList list;
        for (int i = 0; i < columnCount; i++) {
            list << query.value(i).toString();
        }

        content << list.join(spliter);
    }

    return content;
}

bool DataCsv::inputData(int columnCount,
                        const QString &columnNames,
                        const QString &tableName,
                        QString &fileName,
                        const QString &defaultDir,
                        bool existTitle)
{
    fileName = QFileDialog::getOpenFileName(0, "选择文件", defaultDir, DataCsv::CsvFilter);
    if (fileName.isEmpty()) {
        return false;
    }

    QFile file(fileName);
    bool ok = file.open(QIODevice::ReadOnly | QFile::Text);
    if (!ok) {
        return false;
    }

    //传入了字段集合则取字段
    if (!columnNames.isEmpty()) {
        columnCount = columnNames.split(",").count();
    }

    //先读取第一行判断列数是否和目标列数一致,不一致则返回
    QString line = QString::fromUtf8(file.readLine());
    QStringList list = line.split(DataCsv::CsvSpliter);
    if (list.count() != columnCount) {
        return false;
    }

    //先删除原来的数据
    QString sql = QString("delete from %1").arg(tableName);
    QSqlQuery query;
    query.exec(sql);

    //cvs格式需要gbk编码才能正常
    QTextStream in(&file);
    in.seek(0);
    if (fileName.endsWith(".csv")) {
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
        in.setCodec("gbk");
#endif
    }

    //开启数据库事务加速处理
    QSqlDatabase::database().transaction();

    bool isremoveTitle = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(DataCsv::CsvSpliter);

        //如果存在标题则不需要处理第一行标题
        if (existTitle && !isremoveTitle) {
            isremoveTitle = true;
            continue;
        }

        //列数必须完全一致才行
        if (list.count() == columnCount) {
            if (!columnNames.isEmpty()) {
                sql = QString("insert into %1(%2) values('").arg(tableName).arg(columnNames);
            } else {
                sql = QString("insert into %1 values('").arg(tableName);
            }

            for (int i = 0; i < columnCount - 1; i++) {
                sql = sql + list.at(i).trimmed() + "','";
            }

            sql = sql + list.at(columnCount - 1).trimmed() + "')";
            query.clear();
            query.exec(sql);
        }
    }

    //提交数据库事务
    if (!QSqlDatabase::database().commit()) {
        QSqlDatabase::database().rollback();
        return false;
    }

    file.close();
    return true;
}

bool DataCsv::outputData(const QString &defaultName,
                         const QStringList &content,
                         QString &fileName,
                         const QString &defaultDir)
{
    bool result = true;
    QString defaultPath = QString("%1/%2").arg(defaultDir).arg(defaultName);
    fileName = QFileDialog::getSaveFileName(0, "选择文件", defaultPath, DataCsv::CsvFilter);
    outputData(fileName, content);
    return result;
}

bool DataCsv::outputData(QString &fileName, const QStringList &content)
{
    if (fileName.isEmpty()) {
        return false;
    }

    int rowCount = content.count();
    if (rowCount == 0) {
        fileName.clear();
        return false;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        //cvs格式需要gbk编码才能正常
        QTextStream out(&file);
        if (fileName.endsWith(".csv")) {
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
            out.setCodec("gbk");
#endif
        }

        for (int i = 0; i < rowCount; i++) {
            out << content.at(i) << "\n";
        }

        file.close();
    }

    return true;
}
