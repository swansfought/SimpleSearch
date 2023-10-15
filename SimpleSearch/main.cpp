#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QSharedMemory>
#include <QIcon>


int main(int argc, char *argv[])
{
    //解决中文乱码问题
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);

    static QSharedMemory *shareMem = new QSharedMemory("SingleApp"); //创建【SingleApp】的共享内存块
    if (!shareMem->create(1)){//创建大小1b的内存
        qApp->quit(); //创建失败，说明已经有一个程序运行，退出当前程序
        return -1;
    }

    QDesktopWidget *desktop = QApplication::desktop();

    Widget w;
    w.move((desktop->width()-w.width())/2, (desktop->height()-w.height())-70);//(desktop->height()-w.height())/2-150)
    w.setWindowIcon(QIcon(":/images/icons/ApplicationIcon.ico"));
    w.show();

    return a.exec();
}
