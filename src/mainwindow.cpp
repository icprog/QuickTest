#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWindow()
{
    ui->stackedWidget->addWidget(&m_wBuildTemplate);
    ui->stackedWidget->addWidget(&m_mappingLinkWidget);
    ui->stackedWidget->addWidget(&m_testWidget);

    btn_help = new QPushButton(QObject::trUtf8("帮助"), this);
    connect(btn_help, SIGNAL(clicked()), SLOT(btn_help_clicked()));
    ui->navi->addButton(btn_help);

    btn_build = new QPushButton(QObject::trUtf8("新建\n模板"), this);
    connect(btn_build, SIGNAL(clicked()), SLOT(btn_build_clicked()));
    ui->navi->addButton(btn_build);

    btn_open = new QPushButton(QObject::trUtf8("打开\n模板"), this);
    connect(btn_open, SIGNAL(clicked()), SLOT(btn_open_clicked()));
    ui->navi->addButton(btn_open);

    btn_sysconfig = new QPushButton(QObject::trUtf8("系统\n配置"), this);
    connect(btn_sysconfig, SIGNAL(clicked()), SLOT(btn_sysconfig_clicked()));
    ui->navi->addButton(btn_sysconfig);

    btn_test = new QPushButton(QObject::trUtf8("开始\n测试"), this);
    connect(btn_test, SIGNAL(clicked()), SLOT(btn_test_clicked()));
    ui->navi->addButton(btn_test, true);

//    btn_quit = new QPushButton(QObject::trUtf8("退出"), this);
//    connect(btn_quit, SIGNAL(clicked()), SLOT(btn_quit_clicked()));
//    ui->navi->addButton(btn_quit, true);

}

void MainWindow::initSignals()
{
    connect(&m_wBuildTemplate, SIGNAL(signal_return()), this, SLOT(slot_returnToMainWindow()));

    connect(&m_mappingLinkWidget, SIGNAL(quit()), this, SLOT(slot_returnToMainWindow()));

    connect(&m_testWidget, SIGNAL(quit()), this, SLOT(slot_returnToMainWindow()));
}
#include <QCloseEvent>
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int ret;

    switch (event->key()) {
    case Qt::Key_Escape:
        event->accept();
        ret = QMessageBox::warning(this, trUtf8("退出"), "是否退出一键测试?", QMessageBox::Ok, QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            qApp->quit();
        } else {
            //Do nothing.
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }

    return ;
}

void MainWindow::btn_help_clicked()
{
    //btn_help licked.
}

void MainWindow::btn_build_clicked()
{
    //btn_build licked.
    if (m_wBuildTemplate.updateWidgets()) {
        ui->stackedWidget->setCurrentWidget(&m_wBuildTemplate);
    } else {
        QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("界面刷新失败"), QMessageBox::Ok);
    }
}
#include "customWidget/importdlg.h"
#include "filepath.h"
void MainWindow::btn_open_clicked()
{
    QStringList nameFilters;
    nameFilters << "*.prj";

#if defined (Q_OS_WIN32)
    QString rootPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(WIN_SCHEME_SAVE_DIR);
#else
    QString rootPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(LINUX_SCHEME_SAVE_DIR);
#endif

    importDlg import( trUtf8("打开测试模板"), rootPath, nameFilters, 0 );
    if (import.exec() == QDialog::Accepted) {

        if (import.filePath().isEmpty()) {
            return;
        }
        QMessageBox::about(this, "accepted", import.filePath());
    }

}

void MainWindow::btn_test_clicked()
{
    //btn_test licked.
    if (m_testWidget.initWidget())
        ui->stackedWidget->setCurrentWidget(&m_testWidget);
}

void MainWindow::btn_sysconfig_clicked()
{
    //btn_sysconfig licked.
    ui->stackedWidget->setCurrentWidget(&m_mappingLinkWidget);
}

/*
void MainWindow::btn_quit_clicked()
{
    //btn_quit clicked.
    qApp->quit();
}
*/

void MainWindow::slot_returnToMainWindow()
{
    ui->stackedWidget->setCurrentWidget(ui->page);
}
