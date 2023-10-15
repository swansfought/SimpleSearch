#ifndef FORMTXT_H
#define FORMTXT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#define FixedColumnCount 2  //文件固定2列

namespace Ui {
class FormTxt;
}

class FormTxt : public QWidget
{
    Q_OBJECT
private:
    QString currFile;
    QString fastApp="应用快启.txt";
    QString fastLink="快速链接.txt";
    QString dataFolder="data";

    QStandardItemModel  *theModel;//数据模型
    QItemSelectionModel *theSelection;//Item选择模型

    void iniModelFromStringList(QStringList&);//从StringList初始化数据模型
    void iniNewItem(const QString str);
public:
    explicit FormTxt(QWidget *parent = 0);
    ~FormTxt();

private slots:
    void on_actOpen_triggered();

    void on_actSave_triggered();

    void on_actInsert_triggered();

    void on_actAppend_triggered();

    void on_actDelete_triggered();

private:
    Ui::FormTxt *ui;
};

#endif // FORMTXT_H
