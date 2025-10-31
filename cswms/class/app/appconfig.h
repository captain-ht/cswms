#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "head.h"

class AppConfig
{
public:
    static QString ConfigFile;      //配置文件文件路径及名称

    //基本配置参数1
    static bool AutoRun;            //开机自动运行
    static bool AutoPwd;            //记住密码
    static bool AutoLogin;          //自动登录
    static bool AutoConfirm;        //自动确认
    static bool SaveLog;            //启动日志调试

    //基本配置参数2
    static QString Copyright;       //版权所有
    static QString TitleCn;         //软件中文名称
    static QString TitleEn;         //软件英文名称
    static QString LogoImage;       //软件logo
    static QString HttpUrl;         //网址
    static QString StyleName;       //样式
    static QString LastLoginer;     //最后登录用户
    static QString LastFormMain;    //最后主界面
    static QString LastFormData;    //最后的数据查询界面
    static QString LastFormConfig;  //最后的系统设置界面

    //基本配置参数3
    static int WorkMode;            //工作模式 0-从设备采集 1-从数据库采集
    static int PanelMode;           //面板模式
    static bool PlayAlarmSound;     //播放报警声音
    static int PlayCount;           //报警声音播放次数
    static int MsgCount;            //主界面警情状态条数
    static int DeviceColumn;        //主界面设备列数
    static int RecordsPerpage;           //表格分页时每页数据
    static int TipInterval;         //弹框显示的时间 单位秒 0表示一直显示除非手动关闭 10000-表示禁用
    static int Precision;           //采集到的值的精确度

    //记录控制配置参数
    static int AlarmLogMaxCount;    //报警记录最大数,单位万
    static int NodeLogMaxCount;     //运行记录最大数,单位万
    static int UserLogMaxCount;     //用户记录最大数,单位万
    static QString AlarmLogOrder;   //报警记录排序
    static QString NodeLogOrder;    //运行记录排序
    static QString UserLogOrder;    //用户记录排序

    //颜色配置参数
    static QString ColorZero;       //0值颜色
    static QString ColorUpper;      //高报颜色
    static QString ColorLimit;      //低报颜色
    static QString ColorNormal;     //正常颜色
    static QString ColorPlotBg;     //曲线背景颜色
    static QString ColorPlotText;   //曲线文字颜色
    static QString ColorPlotLine;   //曲线线条颜色
    static QString ColorOther;      //其他颜色

    //数据转发配置参数
    static bool UseNetSend;         //启用数据转发
    static QString NetSendInfo;     //转发ip地址+端口
    static bool UseNetReceive;      //启用数据接收
    static int NetReceivePort;      //数据接收端口

    //告警短信转发配置参数
    static QString MsgPortName;     //短信猫通信串口号
    static int MsgBaudRate;         //短信猫通信波特率
    static int MsgInterval;         //短信告警间隔 10000-表示禁用
    static QString MsgTel;          //告警短信接收号码

    //报警邮件转发配置参数
    static QString SendEmailAddr;   //发件人地址
    static QString SendEmailPwd;    //发件人密码
    static int EmailInterval;       //邮件报警间隔 10000-表示禁用
    static QString ReceiveEmailAddr;//报警邮件接收地址

    //本地数据库配置
    static QString LocalDbType;     //本地数据库类型
    static QString LocalDbName;     //本地数据库名称
    static QString LocalHostName;   //本地数据库主机
    static int LocalHostPort;       //本地数据库端口    
    static QString LocalUserName;   //本地数据库用户名
    static QString LocalUserPwd;    //本地数据库密码

    //远程数据库配置参数
    static bool UseNetDb;           //启用远程数据同步
    static QString NetDbType;       //远程数据库类型
    static QString NetDbName;       //远程数据库名称
    static QString NetHostName;     //远程数据库主机
    static int NetHostPort;         //远程数据库端口    
    static QString NetUserName;     //远程数据库用户名
    static QString NetUserPwd;      //远程数据库密码

    //调试配置参数
    static int PortNameIndex;       //端口名称下拉框索引
    static int DeviceNameIndex;     //设备名称下拉框索引
    static QString KeyValue1;       //关键字1
    static QString KeyColor1;       //关键字颜色1
    static QString KeyValue2;       //关键字1
    static QString KeyColor2;       //关键字颜色1
    static QString SelectDirName;   //最后保存的文件夹

    //调试工具配置参数
    static QString DeviceType;      //设备类型
    static int DeviceAddr;          //设备地址
    static QString PortName;        //串口号
    static int BaudRate;            //波特率
    static int ListenPort;          //监听端口
    static QString NodeValues;      //节点的值集合
    static bool AutoData;           //自动模拟
    static int TabIndex;            //选项卡索引

    static void readConfig();       //读取配置文件,在main函数最开始加载程序载入
    static void checkConfig();      //检验配置参数,比如限定最小值最大值重新校正配置参数等
    static void writeConfig();      //写入配置文件,在更改配置文件程序关闭时调用
};

#endif // APPCONFIG_H
