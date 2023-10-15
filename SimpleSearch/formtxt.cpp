#include "formtxt.h"
#include "ui_formtxt.h"
#include <QToolBar>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


void FormTxt::iniModelFromStringList(QStringList &fileContent)
{
    int rowCnt=fileContent.count(); //文本行数
    theModel->setRowCount(rowCnt-1); //数据行数

    //设置表头文字
    QString header=fileContent.at(0); //设置表头
    QStringList headerList=header.split("\t\t");
    theModel->setHorizontalHeaderLabels(headerList);

    //设置表格数据
    QStandardItem *item;
    QString lineText;
    for (int i=1;i<rowCnt;i++){//不算标题
        lineText=fileContent.at(i); //获取一行数据
        QStringList dataList=lineText.split("\t\t");
        for (int j=0;j<FixedColumnCount;j++){
            item=new QStandardItem(dataList.at(j));
            if(j==0)
                item->setTextAlignment(Qt::AlignCenter);
            theModel->setItem(i-1,j,item); //为模型的某个行列位置设置Item
        }
    }
}

void FormTxt::iniNewItem(const QString str)
{
    QList<QStandardItem*> itemList;//QStandardItem的列表类
    QStandardItem *item;
    for(int i=0;i<FixedColumnCount;i++){
        item=new QStandardItem("null");
        item->setTextAlignment(Qt::AlignCenter);
        itemList<<item;//添加到列表类
    }

    QModelIndex curIndex;
    if(str=="插入"){
        curIndex=theSelection->currentIndex(); //获取当前选中项的模型索引
        theModel->insertRow(curIndex.row(),itemList);  //在当前行的前面插入
    }else{//"添加"
        theModel->insertRow(theModel->rowCount(),itemList);  //在当前行的前面插入
        curIndex=theSelection->currentIndex(); //获取当前选中项的模型索引
    }

    //重新选择选中情况
    theSelection->clearSelection();
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);

}

FormTxt::FormTxt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTxt)
{
    ui->setupUi(this);
    this->setFixedSize(600,420);
    this->setWindowFlags(Qt::WindowCloseButtonHint);//只有关闭按钮
    theModel=new QStandardItemModel(1,FixedColumnCount);//数据模型
    theSelection=new QItemSelectionModel(theModel);//选择模型

    //为tableView设置数据模型
    ui->tableView->setModel(theModel); //设置数据模型
    ui->tableView->setSelectionModel(theSelection);//设置选择模型
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

    ui->tableView->setColumnWidth(0,150);//设置固定列宽
    ui->tableView->setColumnWidth(1,398);
    // 将所有列锁住，不可进行设置列宽
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    //创建工具栏添加action
    QToolBar *toolBar=new QToolBar("文本文档",this);
    toolBar->addAction(ui->actOpen);
    toolBar->addAction(ui->actSave);
    toolBar->addAction(ui->actAppend);
    toolBar->addAction(ui->actInsert);
    toolBar->addAction(ui->actDelete);

}

FormTxt::~FormTxt()
{
    delete ui;
}

//打开文件
void FormTxt::on_actOpen_triggered()
{
    QString currPath=QCoreApplication::applicationDirPath()+"/"+FormTxt::dataFolder;
    QString dlgTitle("选择一个txt文件");
    QString filter("文本文件(*.txt)");
    QString fileName=QFileDialog::getOpenFileName(this,dlgTitle,currPath,filter);
    FormTxt::currFile=fileName;//保存当前文件名

    if(fileName.isEmpty())
        return;

    QStringList fileContent;//文件内容字符串列表
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream input(&file);
        while (!input.atEnd())
            fileContent.append(input.readLine());
        file.close();
        iniModelFromStringList(fileContent);//从StringList的内容初始化数据模型
    }

    //使能action
    ui->actAppend->setEnabled(true);
    ui->actInsert->setEnabled(true);
    ui->actDelete->setEnabled(true);
    ui->actSave->setEnabled(true);
}

//保存文件
void FormTxt::on_actSave_triggered()
{
    //以读写、覆盖原有内容方式打开文件
    QFile file(FormTxt::currFile);
    if (!(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)))
        return;

    QString str("");
    QStandardItem *item;
    QTextStream outPut(&file);

    //获取表头文字
    for (int i=0;i<FixedColumnCount;i++){
        item=theModel->horizontalHeaderItem(i);
        if(i<FixedColumnCount-1)//最后一列不加\t\t
            str=str+item->text()+"\t\t";
        else
            str=str+item->text();
    }
    outPut<<str<<"\n";

    //获取数据区文字
    for (int j=0;j<theModel->rowCount();j++){
        str="";
        for(int k=0;k<FixedColumnCount;k++){
            item=theModel->item(j,k);
            if(k<FixedColumnCount-1)
                str=str+item->text()+"\t\t";//最后一列不加\t\t
            else
                str=str+item->text();
        }
        outPut<<str<<"\n";
    }
}

//插入一行
void FormTxt::on_actInsert_triggered()
{
    iniNewItem("插入");
}

//添加一行
void FormTxt::on_actAppend_triggered()
{
    iniNewItem("添加");
}

//删除一行
void FormTxt::on_actDelete_triggered()
{
    QModelIndex currIndex=theSelection->currentIndex();
    if (currIndex.row()==theModel->rowCount()-1){//删除最后一行情况
        theModel->removeRow(currIndex.row());
        theSelection->setCurrentIndex(currIndex,QItemSelectionModel::Select);
    }
    else{
        theModel->removeRow(currIndex.row());
        theSelection->setCurrentIndex(currIndex,QItemSelectionModel::Select);
    }
}
