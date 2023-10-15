#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCombobox>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
private:
    QPoint z;//来记录鼠标相对于窗口的位置
    bool isTop=false;//是否置顶标识
    bool isDrag=false;//是否可拖曳标识

    QString Bing="Bing";
    QString Baidu="百度";
    QString _360="360";
    QString fastApp="应用快启.txt";
    QString fastLink="快速链接.txt";
    QString dataFolder="data";

    QString bingUrl="https://cn.bing.com/search?ie=UTF-8&q=";
    QString baiDuUrl="http://www.baidu.com/s?ie=UTF-8&wd=";
    QString _360Url="https://www.so.com/s?ie=utf8&q=";

    QString zhToen="https://fanyi.baidu.com/translate#zh/auto/";//中翻英
    QString enTozh="https://fanyi.baidu.com/translate#en/auto/";//英翻中

public:
    void iniEdgeCombo();//初始化combobox
    bool openApplicationOrLink(const QString getText,const QString textName);//打开应用程序
    bool changeApplicationOrLink();//更改链接信息
    void browseSearch(const QString inputText,const QString edgeType);//浏览器搜索
    QString iniCustomTxt(const QString str);//初始化数据文件
    QString transType();//获取翻译类型


    void paintEvent(QPaintEvent *event) override;//重绘函数
    //移动窗口
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_searchBtn_clicked();//获取lineText输入内容

    void on_topBtn_clicked();

    void on_dragBtn_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
