#include "widget.h"
#include "ui_widget.h"

#include "formtxt.h"
#include <QPainter>
#include <QPaintEvent>
#include <QUrl>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>
#include <QIcon>
#include <QSize>
#include <QStringList>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint);//窗体没有最大化最小化按钮,设置无边框窗口

    this->iniEdgeCombo();//初始化combobox控件
    this->iniCustomTxt(Widget::fastApp); //初始化数据文件
    this->iniCustomTxt(Widget::fastLink);

    //关联信号与槽
    connect(ui->inputLineEdit, SIGNAL(returnPressed()), this, SLOT(on_searchBtn_clicked()));//连接信号与槽
}

void Widget::iniEdgeCombo() //"初始化列表"按键
{
    //设置按钮提示信息
    ui->topBtn->setToolTip("置顶");
    ui->dragBtn->setToolTip("拖动");
    ui->edgeCombo->setToolTip("浏览器类型");
    ui->transCombo->setToolTip("翻译类型");
    QIcon   bingIcon,duIcon,_360Icon,transIcon;

    //添加图标
    bingIcon.addFile(":/images/icons/Edge.ico");
    duIcon.addFile(":/images/icons/du.ico");
    _360Icon.addFile(":/images/icons/360.ico");

    QSize size(15,15);//缩小翻译图标
    ui->transCombo->setIconSize(size);
    transIcon.addFile(":/images/icons/trans.ico");

    //添加edgeCombo内容
    ui->edgeCombo->addItem(bingIcon,Widget::Bing);
    ui->edgeCombo->addItem(duIcon,Widget::Baidu);
    ui->edgeCombo->addItem(_360Icon,Widget::_360);

    //添加transCombo内容
    ui->transCombo->addItem(transIcon,0);
    ui->transCombo->addItem("中文");
    ui->transCombo->addItem("英文");

}

//搜索按钮槽函数
void Widget::on_searchBtn_clicked()
{
    QString getText;
    getText = ui->inputLineEdit->text();//获取输入文本

    if(getText.isEmpty())
        return;

    //链接更改
    if(getText=="config"||getText=="update"||getText=="链接"||getText=="文件"||getText=="修改文件"||getText=="修改链接"){
        changeApplicationOrLink();
        return;
    }

    //检测是否为打开应用程序操作
    if(openApplicationOrLink(getText,Widget::fastApp)){
        ui->inputLineEdit->clear();
        return;
    }

    //检测是否为打开快速链接操作
    if(openApplicationOrLink(getText,Widget::fastLink)){
        ui->inputLineEdit->clear();
        return;
    }

    //正常搜索操作
    QString edgeType;
    edgeType=ui->edgeCombo->currentText();
    browseSearch(getText,edgeType);
}

//初始化数据文件
QString Widget::iniCustomTxt(const QString str)
{
    QString txtPath=QCoreApplication::applicationDirPath()+"/"+Widget::dataFolder+"/"+str;//完整路径
    QDir totalPath(txtPath);//txt路径

    if(totalPath.exists())//文件存在
        return txtPath;

    //不存在data文件夹情况
    QDir dataPath(QCoreApplication::applicationDirPath()+"/"+Widget::dataFolder);//data的文件路径
    if(!dataPath.exists()){
        dataPath = QCoreApplication::applicationDirPath();
        dataPath.mkdir("data");//在当前目录下创建一个名称为data的文件
    }

    //不存在txt文件情况
    QFile file(txtPath);//创建txt文件
    if(str==Widget::fastApp){
        if(file.open(QIODevice::Text|QIODevice::ReadWrite)){
            QTextStream out(&file);
            out<<tr("快捷名称\t\t程序路径(路径符用[/],路径带\"\"的打不开!)\n");
            file.close();//关闭文件
        }
    }else{ //Widget::fastLink
        if(file.open(QIODevice::Text|QIODevice::ReadWrite)){
            QTextStream out(&file);
            out<<tr("快捷名称\t\t链接地址(网页或网站链接等)\n");
            file.close();//关闭文件
        }
    }
    //    if(!isIniData){
    //        QString dlgTitle("通知框");
    //        QString strInfo("数据文件初始化完成！\n文件路径:"+QCoreApplication::applicationDirPath()+"/"+Widget::dataFolder);
    //        QMessageBox::information(this,dlgTitle,strInfo,QMessageBox::Ok,QMessageBox::NoButton);
    //    }
    //    isIniData=true;//已初始化
    ui->inputLineEdit->clear();
    return txtPath;
}

//打开应用程序或者打开链接
bool Widget::openApplicationOrLink(const QString getText,const QString textName)
{
    QString str = iniCustomTxt(textName);//获取完整路径

    QFile input(str);
    if(!input.open(QIODevice::Text|QIODevice::ReadWrite))//打开文件
        return false;

    QString text,filterTxt;
    QTextStream stream(&input);
    QStringList textList;
    filterTxt=stream.readLine();//把标题读走
    while(!stream.atEnd()){
        text=stream.readLine();//逐行扫描
        textList=text.split("\t\t");//分割字符
        if(textList[0]==getText && textList[1]!=NULL){
            //            QProcess process;
            //            process.startDetached(textList[1]);//启动程序
            //            input.close();
            //            return true;
            //            }
            if(QDesktopServices::openUrl(QUrl(textList[1]))){//打开应用程序
                input.close();
                return true;
            }
        }
        textList.clear();
    }
    input.close();
    return false;
}

//修改信息
bool Widget::changeApplicationOrLink()
{
    //创建用于编辑txt文件的窗口
    FormTxt *txtWidget=new FormTxt(this);
    txtWidget->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    txtWidget->setWindowTitle("修改数据文件");
    txtWidget->setWindowFlag(Qt::Window,true);

    //设置窗口位置
    QDesktopWidget *desktop = QApplication::desktop();
    txtWidget->move((desktop->width()-txtWidget->width())/2,
                    (desktop->height()-txtWidget->height())/2);
    txtWidget->show();

    ui->inputLineEdit->clear();
    return true;
}

//对输入框中的数据添加至浏览器搜索
void Widget::browseSearch(const QString inputText,const QString edgeType)
{
    QString transUrl;
    transUrl=transType();//获取翻译类型

    //搜索内容，翻译
    if(edgeType==Widget::Bing){
        if(!transUrl.isEmpty()){
            QDesktopServices::openUrl(QUrl(transUrl+inputText));//跳转至翻译
            ui->inputLineEdit->clear();
            return;
        }
        QDesktopServices::openUrl(QUrl(Widget::bingUrl+inputText));//正常搜索
    }else if(edgeType==Widget::Baidu){
        if(!transUrl.isEmpty()){
            QDesktopServices::openUrl(QUrl(transUrl+inputText));
            ui->inputLineEdit->clear();
            return;
        }
        QDesktopServices::openUrl(QUrl(Widget::baiDuUrl+inputText));
    }else if(edgeType==Widget::_360){
        if(!transUrl.isEmpty()){
            QDesktopServices::openUrl(QUrl(transUrl+inputText));
            ui->inputLineEdit->clear();
            return;
        }
        QDesktopServices::openUrl(QUrl(Widget::_360Url+inputText));
    }
    ui->inputLineEdit->clear();
}

//获取翻译类型
QString Widget::transType()
{
    if(ui->transCombo->currentText()=="中文")//中翻英
        return Widget::zhToen;
    if(ui->transCombo->currentText()=="英文")//英翻中
        return Widget::enTozh;
    return NULL;
}

//设置窗口置顶
void Widget::on_topBtn_clicked()
{
    //窗口未置顶情况
    if(!Widget::isTop){
        Widget::isTop=true;//窗口已置顶
        QIcon icon(":/images/icons/top.ico");
        QSize size(29,19);//恢复默认大小
        ui->topBtn->setIconSize(size);
        ui->topBtn->setIcon(icon);

        //设置窗口置顶
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        this->showNormal();
    }else{
        Widget::isTop=false;//取消窗口置顶
        QIcon icon(":/images/icons/noTop.ico");
        QSize size(15,15);//设置大小，原因是箭头图片太大了...
        ui->topBtn->setIconSize(size);
        ui->topBtn->setIcon(icon);

        //取消窗口置顶
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
        this->showNormal();
    }
}

//拖动按钮槽函数
void Widget::on_dragBtn_clicked()
{
    if(!Widget::isDrag){
        Widget::isDrag=true;//窗口可拖动
        QIcon icon(":/images/icons/drag.ico");
        ui->dragBtn->setIcon(icon);
        ui->inputLineEdit->setEnabled(false);
        ui->searchBtn->setEnabled(false);
        ui->edgeCombo->setEnabled(false);
        ui->topBtn->setEnabled(false);
        ui->transCombo->setEnabled(false);
        ui->inputLineEdit->setText("拖动完成先锁定！");
    }else{
        Widget::isDrag=false;//窗口不可拖动
        QIcon icon(":/images/icons/noDrag.ico");
        ui->dragBtn->setIcon(icon);
        ui->inputLineEdit->setEnabled(true);
        ui->searchBtn->setEnabled(true);
        ui->edgeCombo->setEnabled(true);
        ui->topBtn->setEnabled(true);
        ui->transCombo->setEnabled(true);
        ui->inputLineEdit->clear();
    }
}

//重写重绘函数，在paintEvent事件中绘制窗口边框。圆角实现
void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 15, 15);//rect:绘制区域  圆角弧度:10
    QWidget::paintEvent(event);
}

//窗口拖动：鼠标移动事件
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(isDrag){
        QWidget::mouseMoveEvent(event);
        QPoint y = event->globalPos(); // 鼠标相对于桌面左上角位置
        QPoint x = y - this->z;
        this->move(x);
    }
}

//窗口拖动：鼠标按压事件
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(isDrag){
        QWidget::mousePressEvent(event);
        QPoint x = this->geometry().topLeft(); // 窗口左上角相对于桌面左上角位置
        QPoint y = event->globalPos(); // 鼠标相对于桌面左上角位置
        this->z = y - x; // 这是个定值
    }
}

//窗口拖动：鼠标释放事件
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(isDrag){
        QWidget::mouseReleaseEvent(event);
        this->z = QPoint();
    }
}

Widget::~Widget()
{
    delete ui;
}

