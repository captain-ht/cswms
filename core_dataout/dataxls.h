#ifndef DATAXLS_H
#define DATAXLS_H

/**
 * 导出数据到Xls 2017-2-17
 * 原理:写入xml格式的数据,10万行数据10个字段只需要1秒钟完成
 * 缺点:生成的文件体积较大,还可以考虑优化部分样式之类的
 * 只需要四个步骤即可开始急速导出大量数据到Xls
 * 第一步:设置文件路径,标题等信息;
 * 第二步:打开文件,启动线程;
 * 第三步:逐行输入数据;
 * 第四步:关闭文件,关闭线程;
 * 1. 多线程写入数据
 * 2. 可设置标题/副标题/表名
 * 3. 可设置字段名称和列宽度
 * 4. 可设置是否启用校验过滤数据,校验的列,校验规则和校验值
 * 5. 可设置随机背景颜色及需要随机背景色的列集合
 * 6. 可分组输出数据
 * 7. 可设置行内容分隔符
 * 8. 可追加数据形式写入数据,建议每次追加的数据小于10000条
 * 9. 可直接设置表格数据,一次性写入
 * 10. 灵活性超高,可自由更改源码设置对齐方式/文字颜色/背景颜色等
 * 11. 支持任意excel表格软件,包括但不限于excel2003/2007/2010/2013 wps openoffice
 */

#include "datahead.h"

class DataXls : public QThread
{
    Q_OBJECT
public:
    //全局导出数据类
    static DataXls *dataXls;
    //导出数据到xls
    static void saveXls(const DataContent &dataContent);
    static void saveXls(const QString &fileName,
                        const QString &sheetName,
                        const QString &title,
                        const QList<QString> &columnNames,
                        const QList<int> &columnWidths,
                        const QStringList &content);

    explicit DataXls(QObject *parent = 0);
    ~DataXls();

protected:
    void run();

private:
    QMutex mutex;               //共享锁
    volatile bool stopped;      //标志位用于停止线程
    QElapsedTimer time;         //计算用时对象

    int columnCount;            //列数
    QFile file;                 //文件对象
    QTextStream stream;         //数据流对象
    DataContent dataContent;    //数据内容

public:
    //获取执行所用时间
    QString getUseTime();

public slots:
    //输出头部信息
    void appendHead();
    //输出文档信息
    void appendDocument();
    //输出边框样式
    void appendBorder();
    //输出样式信息
    void appendStyle();
    //输出表结构
    void appendTable();
    //输出字段名称
    void appendColumn();
    //输出结尾信息
    void appendEnd();

    //输出当前队列内容
    void appendContent(const QStringList &listContent);
    //核心处理循环输出内容
    void takeContent();

public slots:
    //设置数据内容
    void setDataContent(const DataContent &dataContent);

    //初始化
    void init();
    //打开文件准备输出数据
    void open();
    //关闭文件
    void close();

    //暂停线程
    void stop();
    //追加数据
    void append(const QString &content, const QString &subTitle1 = "", const QString &subTitle2 = "");

signals:
    //处理完一批次内容
    void appendFinshed(int count, int mesc);
};

#endif // DATAXLS_H
