#ifndef APPDATA_H
#define APPDATA_H

#include <QObject>

class AppData
{
public:
    //全局通用变量
    static QString Version;             //版本
    static int MapWidth;                //地图宽度
    static int MapHeight;               //地图高度
    static bool IsMove;                 //设备是否可以移动
    static bool IsReboot;               //是否正在重启

    static QString CurrentImage;        //当前对应地图
    static QString CurrentUserName;     //当前用户名
    static QString CurrentUserPwd;      //当前用户密码
    static QString CurrentUserType;     //当前用户类型(值班员/管理员)
    static QList<bool> UserPermission;//当前用户权限集合
    static QStringList PermissionName;   //权限模块名称

    //全局布局宽度高度值
    static int RowHeight;               //行高
    static int LeftWidth;               //左边宽度
    static int RightWidth;              //右边宽度
    static int TopHeight;               //顶部高度
    static int BottomHeight;            //底部高度
    static int DeviceWidth;             //设备对象宽度
    static int DeviceHeight;            //设备对象高度

    static int BtnMinWidth;             //按钮最小宽度
    static int BtnMinHeight;            //左侧导航按钮最小高度
    static int BtnIconSize;             //按钮图标宽度高度
    static int BorderWidth;             //高亮边框宽度
    static int IconSize;                //图标字体大小
    static int IconWidth;               //图标宽度
    static int IconHeight;              //图标高度
    static int SwitchBtnWidth;          //开关按钮宽度
    static int SwitchBtnHeight;         //开关按钮高度

    static QString DefaultPositionID;   //默认位号
    static QStringList AlarmTypes;      //报警类型
    static QString NodeDataColumns;     //节点数据字段名称集合
    static QString TableDataColumns;    //表格数据字段名称集合
    static QString TypeInfoColumns;     //类型数据字段名称集合
    static QString NodeInfoColumns;     //变量字段名称集合

    static QString MapPath;             //地图目录
    static QList<QString> MapNames;     //地图文件名称集合
    static QString SoundPath;           //声音目录
    static QList<QString> SoundNames;   //声音文件名称集合
    static QList<QString> PortNames;    //串口号集合    
};

#endif // APPDATA_H
