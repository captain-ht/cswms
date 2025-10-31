#ifndef DATAPRINT_H
#define DATAPRINT_H

/**
 * 打印数据到pdf和纸张 2017-2-18
 * 0. 原理:写入table格式的数据
 * 1. 多线程写入数据
 * 2. 可设置标题/副标题/表名
 * 3. 可设置字段名称和列宽度
 * 4. 可设置是否启用校验过滤数据,校验的列,校验规则和校验值
 * 5. 可分组输出数据
 * 6. 可设置行内容分隔符
 * 7. 可追加数据形式写入数据,建议每次追加的数据小于1000条
 * 8. 可直接设置表格数据,一次性写入
 * 9. 灵活性超高,可自由更改源码设置对齐方式/文字颜色/背景颜色等
 * 10. 打印数据目前不支持多线程
 */

#include "datahead.h"

class DataPrint : public QThread
{
    Q_OBJECT
public:
    //全局打印数据类
    static DataPrint *dataPrint;
    //打印数据到pdf
    static void savePdf(const DataContent &dataContent);
    static void savePdf(const QString &fileName,
                        const QString &title,
                        const QList<QString> &columnNames,
                        const QList<int> &columnWidths,
                        const QStringList &content);

    //打印数据到纸张
    static void print(const DataContent &dataContent);
    static void print(const QString &title,
                      const QList<QString> &columnNames,
                      const QList<int> &columnWidths,
                      const QStringList &content,
                      bool landscape = false);

    explicit DataPrint(QObject *parent = 0);
    ~DataPrint();

protected:
    void run();

private:
    QMutex mutex;               //共享锁
    volatile bool stopped;      //标志位用于停止线程
    QElapsedTimer time;         //计算用时对象

    int columnCount;            //列数
    QPrinter printer;           //打印对象
    QTextDocument textDocument; //文档对象    
    DataContent dataContent;    //数据内容
    QStringList list;           //构建html字符串

public:
    //获取执行所用时间
    QString getUseTime();
    //获取打印机对象
    QPrinter *getPrinter();

private slots:
    //输出头部信息
    void appendHead();
    //输出标题信息
    void appendTitle();
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
    bool append();
    //打开文件准备输出数据
    void open(bool thread = false);

    //打印到纸张
    void print();
    //打印到pdf
    void close();
    //打印输出
    void printView(QPrinter *printer);

    //暂停线程
    void stop();
    //追加数据
    void append(const QString &content, const QString &subTitle1 = "", const QString &subTitle2 = "");

signals:
    //处理完一批次内容
    void appendFinshed(int count, int mesc);
    //html数据
    void receiveHtml(const QString &html);
};

#endif // DATAPRINT_H
