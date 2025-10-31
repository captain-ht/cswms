#include "appstyle.h"
#include "appnav.h"
#include "quihelper.h"

QString AppStyle::styleName = ":/qss/blackvideo.css";
void AppStyle::addNavBtnStyle(QStringList &list)
{
    //顶部导航按钮,可以自行修改圆角角度,采用弱属性机制[flag=\"btnNavTop\"],也可采用对象名#widgetBtn
    int topBtnRadius = AppNav::NavMainLeft ? 20 : 5;
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton{font-size:%1px;border-radius:%2px;}")
            .arg(QUIConfig::FontSize + 3).arg(topBtnRadius);
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton{background:%1;border:2px solid %2;}")
            .arg("transparent").arg("transparent");
    //悬停和选中可以分开不同颜色
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton:hover{background:%1;border:2px solid %2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::BorderColor);
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton:checked{background:%1;border:2px solid %2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::BorderColor);

    //左侧导航按钮,可以自行修改圆角角度,采用弱属性机制[flag=\"btnNavLeft\"],也可采用对象名#widgetLeft
    int leftBtnRadius = 5;
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton{font-size:%1px;border-radius:%2px;}")
            .arg(QUIConfig::FontSize + 3).arg(leftBtnRadius);
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton{background:%1;border:2px solid %2;}")
            .arg("transparent").arg("transparent");
    //悬停和选中可以分开不同颜色
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton:hover{background:%1;border:2px solid %2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::BorderColor);
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton:checked{background:%1;border:2px solid %2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::BorderColor);
}

void AppStyle::addNavPageStyle(QStringList &list)
{
    //分页导航
    list << QString("NavPage{qproperty-pageButtonCount:%1;qproperty-showLabInfo:%2;}")
            .arg(7).arg("false");
    list << QString("NavPage{qproperty-fontSize:%1;qproperty-borderWidth:%2;qproperty-borderRadius:%3;qproperty-borderColor:%4;}")
            .arg(QUIConfig::FontSize + 5).arg(0).arg(5).arg(QUIConfig::BorderColor);
    list << QString("NavPage{qproperty-normalBgColor:%1;qproperty-normalTextColor:%2;}")
            .arg(QUIConfig::NormalColorEnd).arg(QUIConfig::TextColor);
    list << QString("NavPage{qproperty-hoverBgColor:%1;qproperty-hoverTextColor:%2;}")
            .arg(QUIConfig::DarkColorStart).arg(QUIConfig::TextColor);
    list << QString("NavPage{qproperty-pressedBgColor:%1;qproperty-pressedTextColor:%2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::TextColor);
    list << QString("NavPage{qproperty-checkedBgColor:%1;qproperty-checkedTextColor:%2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::TextColor);
}

void AppStyle::addSwitchButtonStyle(QStringList &list)
{
    //开关按钮
    bool black = (styleName.contains("black") || styleName.contains("darkblue") || styleName.contains("purple"));
    bool dark = (styleName.contains("darkblue") || styleName.contains("darkblack") || styleName.contains("purple") || styleName.contains("blackblue") || styleName.contains("blackvideo"));
    if (dark) {
        list << QString("SwitchButton{min-width:%1px;max-width:%1px;min-height:%2px;max-height:%2px;"
                        "qproperty-bgColorOn:%3;qproperty-bgColorOff:%4;"
                        "qproperty-textColorOn:%5;qproperty-textColorOff:%5;"
                        "qproperty-sliderColorOn:%6;qproperty-sliderColorOff:%6;}")
                .arg(AppData::SwitchBtnWidth).arg(AppData::SwitchBtnHeight)
                .arg(black ? QUIConfig::DarkColorEnd : QUIConfig::NormalColorEnd)
                .arg(black ? QUIConfig::NormalColorEnd : QUIConfig::DarkColorEnd)
                .arg(QUIConfig::TextColor).arg(QUIConfig::PanelColor);

        list << QString("XSlider{qproperty-normalColor:%1;qproperty-grooveColor:%2;qproperty-handleBorderColor:%2;qproperty-handleColor:%3;qproperty-textColor:%4;}")
                .arg(QUIConfig::BorderColor).arg(QUIConfig::DarkColorEnd).arg(QUIConfig::TextColor).arg(QUIConfig::BorderColor);
        //曲线图表样式
//        list << QString("CustomPlot{qproperty-bgColor:%1;qproperty-textColor:%2;qproperty-gridColor:%2;}")
//                .arg(QUIConfig::BorderColor).arg(QUIConfig::TextColor);
    } else {
        list << QString("SwitchButton{min-width:%1px;max-width:%1px;min-height:%2px;max-height:%2px;"
                        "qproperty-bgColorOn:%3;qproperty-bgColorOff:%4;"
                        "qproperty-textColorOn:%5;qproperty-textColorOff:%5;"
                        "qproperty-sliderColorOn:%6;qproperty-sliderColorOff:%6;}")
                .arg(AppData::SwitchBtnWidth).arg(AppData::SwitchBtnHeight)
                .arg(QColor(21, 156, 119).name()).arg(QColor(111, 122, 126).name())
                .arg(QColor(250, 250, 250).name()).arg(QColor(255, 255, 255).name());

        list << QString("XSlider{qproperty-normalColor:%1;qproperty-grooveColor:%2;qproperty-handleBorderColor:%2;qproperty-handleColor:%3;qproperty-textColor:%4;}")
                .arg(QUIConfig::NormalColorStart).arg(QUIConfig::BorderColor).arg(QUIConfig::PanelColor).arg(QUIConfig::TextColor);
        //曲线图表样式
//        list << QString("CustomPlot{qproperty-bgColor:%1;qproperty-textColor:%2;qproperty-gridColor:%2;}")
//                .arg(QUIConfig::NormalColorStart).arg(QUIConfig::TextColor);
    }
}

void AppStyle::addGaugeCloudStyle(QStringList &list)
{
    //根据不同的样式设置不同,云台控件
    if (styleName == ":/qss/blackvideo.css") {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Black;}");
    } else if (styleName == ":/qss/blackblue.css") {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Blue;}");
    } else if (styleName == ":/qss/purple.css") {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Purple;}");
    } else if (styleName == ":/qss/silvery.css") {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_White;}");
    } else {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Custom;}");
        //根据样式的不同颜色设置云台的颜色
        list << QString("GaugeCloud{qproperty-baseColor:%1;qproperty-bgColor:%2;qproperty-arcColor:%3;"
                        "qproperty-borderColor:%4;qproperty-textColor:%5;}")
                .arg(QUIConfig::PanelColor).arg(QUIConfig::BorderColor).arg(QUIConfig::NormalColorStart)
                .arg(QUIConfig::HighColor).arg(QUIConfig::TextColor);
    }

    //云台控件鼠标进入+按下颜色设置
    list << QString("GaugeCloud{qproperty-enterColor:%1;qproperty-pressColor:%2;}").arg("#47CAF6").arg(QUIConfig::HighColor);
}

void AppStyle::addCommonStyle(QStringList &list)
{
    //对话框样式
    list << QString("QDialogButtonBox>QPushButton{min-width:%1px;}").arg(50);
    list << QString("QMessageBox{background:%1;}").arg("#FFFFFF");
    list << QString("QMessageBox>QLabel{color:%1;}").arg("#000000");

    //主背景
    list << QString("QWidget#widgetMain{background:%1;}").arg(QUIConfig::BorderColor);
    //右上角菜单
    list << QString("QWidget#widgetMenu>QPushButton{border-radius:0px;padding:0px;margin:1px 1px 2px 1px;}");

    //停靠窗体分隔条样式,他娘的根本不是QSplitter,做梦也想不到这样设置,搞了很久
    list << QString("QMainWindow::separator{width:%1px;height:%1px;margin:%2px;padding:%2px;background:%3;}").arg(2).arg(0).arg(QUIConfig::BorderColor);
    //悬浮窗体背景色
    list << QString("QDockWidget{background:%1;}").arg(QUIConfig::BorderColor);
    //分隔条背景颜色
    list << QString("QSplitter{qproperty-handleWidth:1px;}QSplitter::handle{background:%1;}").arg(QUIConfig::BorderColor);
    //停靠窗体标题栏样式
    list << QString("CustomTitleBar{background:%1;border-bottom:1px solid %2;}").arg(QUIConfig::NormalColorStart).arg(QUIConfig::BorderColor);

    //选项卡居中
    //list << QString("QTabBar,QTabWidget::tab-bar{alignment:center;}");
    //选项卡 不显示切换按钮 自动拉伸-居然自带了卧槽之前还自己来计算
    list << QString("QTabBar{qproperty-usesScrollButtons:false;qproperty-expanding:true;}");
    //个别设置宽度
    int tabWidth = (AppData::RightWidth - 40) / 2;
    list << QString("#tabWidgetRoute>QTabBar::tab{min-width:%1px;}").arg(tabWidth);

    //去掉边框
    list << QString("QTreeView,QTabWidget::pane{border:0px;}");
    //部分控件需要设置边框
    list << QString("#tableWidgetMsg{border:0px;}");    
    //list << QString("#txtPollPlusMain,#txtPollPlusSub,#listWidgetPollGroup{border:1px solid %1;}").arg(QUIConfig::BorderColor);
    //树状控件
    list << QString("QTreeView{padding:5px 0px 5px 5px;}");
    //文本框控件
    //list << QString("QTextEdit,QTextEdit:hover,QTextEdit:focus{border:0px;border-radius:0px;}");
    //表格选中背景
    list << QString("QTableView::item:hover,QTableView::item:selected,QHeaderView::section,QHeaderView,QTableCornerButton:section{background:%1;}").arg(QUIConfig::NormalColorStart);
    //提示信息背景透明度
    list << QString("QToolTip{opacity:230;}");
    //日历控件
    list << QString("QCalendarWidget{qproperty-gridVisible:true;}");

    //属性控件
    list << QString("QtPropertyEditorView{border:1px solid %1;padding:0px;}").arg(QUIConfig::BorderColor);
}

void AppStyle::addOtherStyle(QStringList &list)
{
    //主背景
    list << QString("QWidget#widgetMain{background:%1;}").arg(QUIConfig::BorderColor);

    //中英文标题字体大小
    list << QString("#labTitleCn{font-size:%1px;}#labTitleEn{font-size:%2px;}")
            .arg(QUIConfig::FontSize + 13).arg(QUIConfig::FontSize + 4);

    //右上角菜单
    list << QString("QWidget#widgetMenu>QPushButton{border-radius:0px;padding:0px;margin:1px 1px 2px 1px;}");

    //视频画面
    bool black = (AppConfig::StyleName.contains("black") || AppConfig::StyleName.contains("darkblue") || AppConfig::StyleName.contains("purple"));
    AppConfig::LogoImage = QString("config/logo_%1.png").arg(black ? "white" : "black");

    list << QString("QWidget#widgetVideo>QLabel{font-size:%1px;border:2px solid %2;background:%3;}")
            .arg(QUIConfig::FontSize + 10).arg(QUIConfig::BorderColor).arg(QUIConfig::NormalColorStart);
    list << QString("QWidget#widgetVideo>QLabel:focus{border:2px solid %1;background:%2;}")
            .arg(QUIConfig::HighColor).arg(black ? QUIConfig::NormalColorEnd : QUIConfig::DarkColorStart);

    //画面分割栏
    list << QString("QWidget#widgetSplite>QPushButton{background:transparent;border:none;}");

    //顶部导航按钮,可以自行修改圆角角度
    int topBtnRadius = 20;
    list << QString("QWidget#widgetBtn>QPushButton{font-size:%1px;background:transparent;border:none;border-radius:%2px;}")
            .arg(QUIConfig::FontSize + 3).arg(topBtnRadius);
    list << QString("QWidget#widgetBtn>QPushButton:checked,QWidget#widgetBtn>QPushButton:hover{background:%1;border:2px solid %2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::BorderColor);

    //左侧导航按钮,可以自行修改圆角角度
    int leftBtnRadius = 5;
    list << QString("QWidget[flag=\"btn\"]>QPushButton{min-height:%1px;max-height:%1px;}").arg(30);
    list << QString("QWidget[flag=\"btn\"]>QPushButton{font-size:%1px;background:transparent;border:none;border-radius:%2px;}")
            .arg(QUIConfig::FontSize + 3).arg(leftBtnRadius);
    list << QString("QWidget[flag=\"btn\"]>QPushButton:checked,QWidget[flag=\"btn\"]>QPushButton:hover{background:%1;border:2px solid %2;}")
            .arg(QUIConfig::DarkColorEnd).arg(QUIConfig::BorderColor);

    //面板框
    list << QString("QFrame[flag=\"btn\"]{background:%1;}").arg(QUIConfig::BorderColor);
    list << QString("QFrame[flag=\"btn\"]>QPushButton{font-size:%1px;font-weight:bold;}").arg(QUIConfig::FontSize + 3);

    //翻页导航变量
    list << QString("QFrame[flag=\"navlab\"] QLabel{min-height:%1px;max-height:%1px;font:%2px;}").arg(AppData::RowHeight).arg(QUIConfig::FontSize + 2);
    list << QString("QFrame[flag=\"navlab\"] QLabel{background-color:%1;}").arg(QUIConfig::NormalColorEnd);
    //list << QString("#labSelectTime{background-color:%1;}").arg(QUIConfig::DarkColorEnd);
    list << QString("#labTip{background-color:%1;}").arg(QUIConfig::NormalColorEnd);

    //字体放大
    list << QString("#widgetControl>QLabel{font-size:%1px;}").arg(QUIConfig::FontSize + 12);
    list << QString("#widgetControl>QCheckBox::indicator{width:%1px;height:%1px;}").arg(30);

    //左侧导航栏及按钮图标大小
    list << QString("QLineEdit#txtCard,QLabel#labTip,QLabel#labCard,QLabel#labTime{font-weight:bold;font-size:%1px;min-height:%2px;qproperty-alignment:AlignCenter;}").arg(QUIConfig::FontSize + 2).arg(28);
    list << QString("QWidget[flag=\"navbtn\"] QPushButton,QFrame[flag=\"navbtn\"] QPushButton{qproperty-iconSize:%1px;min-width:%2px;}").arg(AppData::BtnIconSize).arg(AppData::BtnMinWidth);
    //list << QString("QLCDNumber{qproperty-segmentStyle:Flat;background-color:rgb(0,0,0,0);color:%1;}").arg(QUIConfig::TextColor);

    //选项卡
    list << QString("QTabWidget::pane{border:0px;}QTabBar::tab{min-width:%1px;min-height:%2px;}").arg(52).arg(20);

    //控件无边框
    list << QString("#widgetRight>QListView,#widgetRight>QTreeView,#widgetLeft>QTableWidget{border-width:0px;}");
    list << QString("#treeWidget{padding:5px 0px 0px 0px;}");
    list << QString("#frameXml{border-radius:0px;}");

    //表格选中背景及扁平化
    list << QString("QTableView::item:hover,QTableView::item:selected,QHeaderView::section,QHeaderView,QTableCornerButton:section{background:%1;}").arg(QUIConfig::NormalColorStart);
    //表格奇数偶数行不同颜色
    list << QString("QTableView,QTreeView{qproperty-alternatingRowColors:false;}");

    //元素间隔
    //list << QString("QTableWidget[flag=\"table\"]::item{margin:2px;color:%1;background:%2;}").arg(QUIConfig::TextColor).arg(QUIConfig::DarkColorStart);

    //文本框控件
    //list << QString("QTextEdit,QTextEdit:hover,QTextEdit:focus{border-width:0px;border-radius:0px;}");

    //自定义面板标题控件
    list << QString("NavTitle{qproperty-bgColor:%1;qproperty-textColor:%2;qproperty-borderColor:%3;"
                    "qproperty-iconNormalColor:%4;qproperty-iconHoverColor:%5;qproperty-iconPressColor:%6;min-height:%7px;}")
            .arg(QUIConfig::NormalColorStart).arg(QUIConfig::TextColor).arg(QUIConfig::BorderColor)
            .arg(QUIConfig::TextColor).arg(QUIConfig::HighColor).arg(QUIConfig::BorderColor).arg(30);

    //logo
    list << QString("#labTitleCn{font-size:%1px;}#labTitleEn,#labTimePlay,#labTimeAll{font-size:%2px;}")
            .arg(QUIConfig::FontSize + 13).arg(QUIConfig::FontSize + 2);

    //面板框
    list << QString("QFrame[flag=\"btn\"]{background:%1;}").arg(QUIConfig::BorderColor);
    list << QString("QFrame[flag=\"btn\"]>QPushButton{font-size:%1px;font-weight:bold;}")
            .arg(QUIConfig::FontSize + 3);

    //翻页导航变量
    list << QString("QFrame[flag=\"navlab\"] QLabel{min-height:%1px;max-height:%1px;font:%2px;}").arg(AppData::RowHeight).arg(QUIConfig::FontSize + 1);
    list << QString("QFrame[flag=\"navlab\"] QLabel{background-color:%1;}").arg(QUIConfig::NormalColorEnd);
    //单独颜色显示
    //list << QString("#labSelectTime{background-color:%1;}").arg(QUIConfig::DarkColorEnd);

    //云台圆形按钮
    list << QString("#widgetPtz>QPushButton{border-width:2px;border-radius:%1px;min-width:%2px;max-width:%2px;min-height:%2px;max-height:%2px;}").arg(17).arg(20);

    //提示信息变量背景颜色
    list << QString("#labTip{background-color:%1;}").arg(QUIConfig::NormalColorEnd);

    //左侧导航栏及按钮图标大小
    list << QString("QLineEdit#txtCard,QLabel#labTip,QLabel#labCard,QLabel#labTime{font-weight:bold;font-size:%1px;min-height:%2px;qproperty-alignment:AlignCenter;}").arg(QUIConfig::FontSize + 2).arg(26);
    list << QString("QWidget[flag=\"navbtn\"] QPushButton,QFrame[flag=\"navbtn\"] QPushButton{min-width:%1px;}").arg(AppData::BtnMinWidth);

    //播放按钮
    list << QString("QPushButton#btnPlayVideo{background:none;border:none;padding:0px 0px 0px 0px;}");
    list << QString("QPushButton#btnPlayVideo{color:%1;}").arg(QUIConfig::TextColor);
    //list << QString("QPushButton#btnPlayVideo:hover{color:%1;}").arg(QUIConfig::DarkColorEnd);

    //进度条
    list << QString("XSlider{qproperty-sliderHeight:%1;qproperty-showText:%2;}").arg(12).arg(false);
    list << QString("#sliderPtzStep,#sliderFps{qproperty-sliderHeight:%1;qproperty-showText:%2;}").arg(15).arg(true);

    //停靠窗体标题栏字体放大
    list << QString("#labTitle{font:%1px;min-height:%2px;}").arg(QUIConfig::FontSize + 3).arg(20);
    list << QString("QLabel[flag=\"title\"]{border:none;padding:5px;}");

    list << QString("#labNodeInfo{background-color:%1;}").arg(QUIConfig::DarkColorEnd);
}

void AppStyle::initStyle(const QString &styleName)
{
    AppStyle::styleName = styleName;
    QFile file(styleName);
    if (!file.open(QFile::ReadOnly)) {
        return;
    }

    QString qss = QLatin1String(file.readAll());
    file.close();

    QUIStyle::getQssColor(qss, QUIConfig::TextColor, QUIConfig::PanelColor, QUIConfig::BorderColor, QUIConfig::NormalColorStart,
                          QUIConfig::NormalColorEnd, QUIConfig::DarkColorStart, QUIConfig::DarkColorEnd, QUIConfig::HighColor);

    //追加样式
    QStringList list;
    addNavBtnStyle(list);
    addNavPageStyle(list);
    addSwitchButtonStyle(list);
    //addGaugeCloudStyle(list);
    addCommonStyle(list);
    addOtherStyle(list);

    //将新增的样式加到统一样式表中
    qss += list.join("");
    qApp->setPalette(QPalette(QUIConfig::PanelColor));
    qApp->setStyleSheet(qss);
}

