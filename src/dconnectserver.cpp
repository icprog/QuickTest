#include "dconnectserver.h"
#include "ui_dconnectserver.h"

#include "quicktestcore.h"

extern QuickTestCore core;
#include <QDebug>

dConnectServer::dConnectServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dConnectServer)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

dConnectServer::~dConnectServer()
{
    delete ui;
}

void dConnectServer::initWindow()
{
    this->setWindowTitle(QObject::tr("Substation Server"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->textBrowser->setDisabled(true);
    ui->btn_savesoftplate->setDisabled(true);
}

void dConnectServer::initSignals()
{
    connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(slotConnect()));
    connect(ui->btn_return, SIGNAL(clicked()), this, SLOT(close()));

    connect(this, SIGNAL(signalConnect()), core.substationServer(), SLOT(slotConnect()));
    connect(this, SIGNAL(signalDisconnect()), core.substationServer(), SLOT(slotDisconnect()));

//    connect (core.substationServer(), SIGNAL(signalServerChanged()), this, SLOT(slotUpdateServerInfo()));
    connect(core.substationServer(), SIGNAL(signalConnectStateChanged(int)), this, SLOT(slotConnectStateChanged(int)));
    connect(core.substationServer(), SIGNAL(signalConnectFaild(QString)), this, SLOT(slotHint(QString)));
    connect(core.substationServer(), SIGNAL(signalHint(QString)), this, SLOT(slotHint(QString)));

    connect(core.substationServer(), SIGNAL(signalExportSoftPlateDone(bool)), SLOT(slotExportDone(bool)));
}

void dConnectServer::on_btn_savesoftplate_clicked()
{
    if (!core.substationServer()->isConnected()) {
        return;
    }

    core.substationServer()->querySoftPlate(true);

}
#include <QMessageBox>
void dConnectServer::slotExportDone(bool ret)
{
    QString title = QObject::tr("导出");
    QString detail = QObject::tr("None");
    if (ret) {
        detail = QObject::tr("成功！");
    } else {
        detail = QObject::tr("失败！");
    }

    QMessageBox::warning(this, title, detail, QMessageBox::Ok);
}

bool dConnectServer::updateThis()
{
//    core.substationServer()->up
    ui->lineEdit_cid->setText(core.substationServer()->ied());
    ui->lineEdit_ap->setText(core.substationServer()->accessPoint());
    ui->lineEdit_ip->setText(core.substationServer()->IP());
    return true;
}

void dConnectServer::slotConnect()
{
        if (ui->btn_connect->text() == QString("连接")) {
            emit signalConnect();
        } else {
            emit signalDisconnect();
        }

}

void dConnectServer::slotConnectStateChanged(int state)
{
    switch (state) {
    case CSubstationServer::CONNECT :
            ui->btn_connect->setText("断开");
            ui->btn_connect->setEnabled(true);
            ui->btn_savesoftplate->setEnabled(true);
        break;
    case CSubstationServer::WAIT :
        ui->btn_connect->setDisabled(true);
        ui->btn_savesoftplate->setEnabled(false);
        break;
    case CSubstationServer::DISCONNECT :
            ui->btn_connect->setText("连接");
            ui->btn_connect->setEnabled(true);
            ui->btn_savesoftplate->setEnabled(false);
        break;
    default:
        break;
    }
}

//void dConnectServer::slotConnectFaild(QString err)
//{

//}

void dConnectServer::slotHint(QString hint)
{
    ui->textBrowser->append(hint);
}

void dConnectServer::on_btn_setIP_clicked()
{
    core.substationServer()->setIp(ui->lineEdit_ip->text());
}


