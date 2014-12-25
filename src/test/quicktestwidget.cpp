#include "quicktestwidget.h"
#include "ui_quicktestwidget.h"
#include "ui_quicktestconfigdialog.h"

#include <QFileDialog>
#include <QMessageBox>

QuickTestWidget::QuickTestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuickTestWidget),
    ui_configDialog(new Ui::QuickTestConfigDialog),
    configDialog(0),
    d(0)
{

    ui->setupUi(this);

    configDialog = new QDialog(parent);
    ui_configDialog->setupUi(configDialog);

}

QuickTestWidget::~QuickTestWidget()
{
    delete ui;
    delete ui_configDialog;
    delete configDialog;
}

bool QuickTestWidget::initWidget()
{

    if (!d) {
        d = new QuickTestInterface(this);
    }

    connect(d, SIGNAL(parseDone()), this, SLOT(updateUI()));
    connect(this, SIGNAL(start()), d, SLOT(start()));
    connect(this, SIGNAL(stop()), d, SLOT(stop()));
    connect(d, SIGNAL(connectFailure(QString)), SLOT(connectFailure(QString)));

//    connect(d, SIGNAL(updateHint(QString)), this, SLOT(updateHint(QString)));
    connect(d, SIGNAL(hint(QString)), ui->textBrowser, SLOT(insertPlainText(QString)));
    connect(d, SIGNAL(error(QString)), this, SLOT(interrupt(QString)));

    //开打光口
    QUdpSocket *sender = new QUdpSocket(this);
    QByteArray data = QByteArray::fromHex("a6a6a6a60402a3ff");//打开所有光口
    sender->writeDatagram(data, HostAddress, HostPort);

    delete sender;
    sender = NULL;


    btn_help = new QPushButton(QObject::trUtf8("帮助"), this);
    connect(btn_help, SIGNAL(clicked()), SLOT(btn_help_clicked()));
    ui->navi->addButton(btn_help);

    btn_scheme = new QPushButton(QObject::trUtf8("选择\n模板"), this);
    connect(btn_scheme, SIGNAL(clicked()), SLOT(btn_scheme_clicked()));
    ui->navi->addButton(btn_scheme);

    btn_start = new QPushButton(QObject::trUtf8("开始\n测试"), this);
    connect(btn_start, SIGNAL(clicked()), SLOT(btn_start_clicked()));
    ui->navi->addButton(btn_start);

    btn_report = new QPushButton(QObject::trUtf8("测试\n报告"), this);
    connect(btn_report, SIGNAL(clicked()), SLOT(btn_report_clicked()));
    ui->navi->addButton(btn_report);

    btn_config = new QPushButton(QObject::trUtf8("设置"), this);
    connect(btn_config, SIGNAL(clicked()), SLOT(btn_config_clicked()));
    ui->navi->addButton(btn_config, true);

//    btn_quit = new QPushButton(QObject::trUtf8("退出"), this);
//    connect(btn_quit, SIGNAL(clicked()), SLOT(btn_quit_clicked()));
//    ui->navi->addButton(btn_quit, true);


    return true;



}
#include <QKeyEvent>
void QuickTestWidget::keyPressEvent(QKeyEvent *event)
{
    int ret = -1;

    switch (event->key()) {
    case Qt::Key_Escape:
        event->accept();
        if (isTestLasting()) {
            ret = QMessageBox::warning(this, trUtf8("退出测试"), trUtf8("测试正在进行，是否退出？"), QMessageBox::Ok, QMessageBox::Cancel);
            if (ret == QMessageBox::Ok) {
                //停止测试相关动作
            }
        }
        this->stopTest();//关闭光口
        break;
    default:
        QWidget::keyPressEvent(event);
    }

    return ;
}

void QuickTestWidget::btn_help_clicked()
{

}
#include "filepath.h"
#include "customWidget/importdlg.h"
void QuickTestWidget::btn_scheme_clicked()
{

    QStringList nameFilters;
    nameFilters << "*.prj";

#if defined (Q_OS_WIN32)
    QString rootPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(WIN_SCHEME_SAVE_DIR);
#elif defined (Q_OS_LINUX)
    QString rootPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(LINUX_SCHEME_SAVE_DIR);
#endif

    importDlg import( trUtf8("选择测试模板"), rootPath, nameFilters, 0 );
    if (import.exec() == QDialog::Accepted) {

        if (import.filePath().isEmpty()) {
            return;
        }

        QString errorString;
        if (!d->loadScheme(import.filePath(), errorString)) {
            QMessageBox::about(this,
                               QObject::trUtf8("打开模板错误"),
                               errorString);
            return;
        }

    }

    return;

}
#include <QDebug>
void QuickTestWidget::btn_start_clicked()
{

    //获取测试列表
    d->selectedItems();
    //检查测试列表是否有效

    //连接服务器

    //连接成功？

    //手动设置软压板

    //自动设置软压板

    //修改软压板结果

    //等待开始测试

    //报告测试结果

    if (btn_start->text() == "开始") {
        ui->textBrowser->clear();   //清空Log
        btn_start->setText(QObject::trUtf8("停止"));  //更改Button状态

        emit start();

    } else if (btn_start->text() == "停止") {

        btn_start->setText(QObject::trUtf8("开始"));
        emit stop();

    } else {
        //never.
    }
}

void QuickTestWidget::btn_report_clicked()
{

}

void QuickTestWidget::btn_config_clicked()
{
//    configDialog
    configDialog->show();
}

bool QuickTestWidget::isTestLasting()
{
    return true;
}

void QuickTestWidget::stopTest()
{
    //清理Widget接口并退出
    //关闭光口
    QUdpSocket *sender = new QUdpSocket(this);
    QByteArray data = QByteArray::fromHex("a6a6a6a60402b3ff");//打开所有光口
    sender->writeDatagram(data, HostAddress, HostPort);

    delete sender;
    sender = NULL;

    emit quit();
}

void QuickTestWidget::updateUI()
{

    ui->treeView->setModel(d->treeModel());
    if (ui->treeView->model()) {
        ui->treeView->expandAll();
    }

}

void QuickTestWidget::interrupt(QString error)
{
    ui->textBrowser->insertPlainText(QObject::trUtf8("测试中断"));
    QMessageBox::critical(this,
                          QObject::trUtf8("测试中断"),
                          QString("%1\n%2").arg(error).arg(QObject::trUtf8("请检查。")),
                          QMessageBox::Ok);
    btn_start->setText(QObject::trUtf8("开始"));
    return;
}

void QuickTestWidget::connectFailure(QString error)
{
    ui->textBrowser->insertPlainText(QString("%1,%2").arg(error).arg(trUtf8("测试中断")));
    btn_start->setText(QObject::trUtf8("开始"));
    return;
}
