#include "importdlg.h"
#include "ui_importdlg.h"

#include "QFileSystemModel"
#include <QScrollBar>
#include <QPushButton>
importDlg::importDlg(const QString &title, const QString &dir, const QStringList &nameFilters/* *.jpg */, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::importDlg),
    exportFilePath(QString())
{
    ui->setupUi(this);


    this->setWindowTitle(title);
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);

    model = new QFileSystemModel();
    model->setNameFilterDisables(false);
    model->setFilter(QDir::Files|QDir::NoDotAndDotDot);

    model->setNameFilters(nameFilters);
    //禁止用户移动视图中的项目
    ui->listView->setMovement(QListView::Static);
    ui->listView->setViewMode(QListView::ListMode);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setSelectionMode(QListView::SingleSelection);
    ui->listView->setModel(model);

    QString rootPath;
    if (dir.isEmpty()) {
        rootPath = "./";
    } else {
        rootPath = dir;
    }
    ui->listView->setRootIndex(model->setRootPath(rootPath));

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accepted()));

}

importDlg::~importDlg()
{
    delete ui;
    delete model;
}

#include <QDebug>
void importDlg::accepted()
{
    exportFilePath = model->filePath(ui->listView->currentIndex());
}
