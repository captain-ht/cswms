#ifndef FRMCONFIGSCADA_H
#define FRMCONFIGSCADA_H

#include <QWidget>

class QListWidgetItem;
class SelectWidget;
class QtObjectController;
class QDesignerCustomWidgetInterface;

namespace Ui {
class frmConfigScada;
}

class frmConfigScada : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigScada(QWidget *parent = 0);
    ~frmConfigScada();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmConfigScada *ui;

    //背景图片
    QPixmap bgPix;
    //打开的xml文件的名称
    QString xmlName;
    //按下的控件节点名称
    QString pressedItem;
    //选中的控件索引
    int widgetIndex;

    //存储插件中的控件名称列表
    QList<QString> listNames;
    //存储跟随拖动窗体
    QList<SelectWidget *> listSelect;    
    //存储控件的用户属性
    QList<QStringList> listUserProperty;
    //存储插件中的所有控件集合
    QList<QDesignerCustomWidgetInterface *> listWidgets;

    QtObjectController *propertyView;

private slots:
    //初始化窗体和表格数据
    void initForm();
    void initTable();

    //载入插件
    void initPlugin();
    void loadPlugin(const QString &fileName);
    void openPlugin(const QString &fileName);

    //打开和保存xml数据
    void openFile(const QString &fileName);
    void saveFile(const QString &fileName);

    //创建控件以及控件的描点跟随窗体
    QWidget *createWidget(int row);
    void newSelect(QWidget *widget, const QStringList &property);
    void newWidget(int row, const QPoint &point);

    //清除焦点+控件按下事件+控件删除事件
    void clearFocus();
    void widgetPressed(QWidget *widget);
    void widgetDelete(QWidget *widget);

    //生成用户属性到表格和控件
    void newProperty(int widgetIndex);
    void widgetProperty(int widgetIndex);

private slots:
    void on_btnInput_clicked();
    void on_btnOutput_clicked();
    void on_listWidget_itemDoubleClicked();
    void on_listWidget_itemPressed(QListWidgetItem *item);

    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();
    void on_btnSave_clicked();
};

#endif // FRMCONFIGSCADA_H
