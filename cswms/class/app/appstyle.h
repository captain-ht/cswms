#ifndef APPSTYLE_H
#define APPSTYLE_H

#include "head.h"

class AppStyle
{
public:
    //当前样式名称
    static QString styleName;
    //加入导航按钮样式
    static void addNavBtnStyle(QStringList &list);
    //加入分页导航样式
    static void addNavPageStyle(QStringList &list);
    //加入开关按钮样式
    static void addSwitchButtonStyle(QStringList &list);
    //加入云台控件样式
    static void addGaugeCloudStyle(QStringList &list);
    //加入通用控件样式
    static void addCommonStyle(QStringList &list);
    //加入其他控件样式 一般这个函数里面的内容不同的项目不一样
    static void addOtherStyle(QStringList &list);
    //初始化全局样式
    static void initStyle(const QString &styleName = AppConfig::StyleName);
};

#endif // APPSTYLE_H
