#ifndef QTCHINESENAME_H
#define QTCHINESENAME_H

#include <QStringList>

class QtChineseName
{
public:
    //2019-10-05 改成多链表存储,以便拓展同一属性不同类区别显示和其他语言支持
    //存储格式 属性名|控件类名(可选),中文属性,其他属性(依次类推)
    //控件类名是为了重复的属性英文名称可以对应不同的类不同的中文
    //比如freeColor在GaugeEdit控件叫进度背景在其他控件叫空余颜色
    //可以自行拓展其他语言的属性
    static QStringList listPropertyName;
    static QStringList listPropertyClass;
    static QStringList listPropertyChinese;

    //2021-7-30 增加控件名称中文对照
    static QStringList listControlName;
    static QStringList listControlChinese;

    //加载中文属性名称对照表
    static void loadFile(const QString &fileName);
    //追加一行属性
    static void appendName(int type, const QString &data);

    //获取中文属性名称
    static QString getPropertyName(const QString &propertyName, const QString &className = "");
    //获取中文控件名称
    static QString getControlName(const QString &controlName);
};

#endif // QTCHINESENAME_H
