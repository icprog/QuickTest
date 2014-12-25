#include "dsyncboard.h"
#include "ui_dsyncboard.h"

#include <QFileDialog>

#include "quicktestcore.h"
extern QuickTestCore core;

dSyncBoard::dSyncBoard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dSyncBoard),
    bUpdateSoftPlate(true)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

dSyncBoard::~dSyncBoard()
{
    delete ui;
}

void dSyncBoard::initWindow()
{
    this->setWindowTitle(QObject::tr("软压板设置"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QObject::tr("确定"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QObject::tr("取消"));

    for(int i = 0; i < 2; i++) {
        m_softPlateModel[i].setColumnCount(3);
        m_softPlateModel[i].setHeaderData(0, Qt::Horizontal, tr("序号"));
        m_softPlateModel[i].setHeaderData(1, Qt::Horizontal, tr("描述"));
        m_softPlateModel[i].setHeaderData(2, Qt::Horizontal, tr("当前值"));
    }

    ui->tableView->horizontalHeader()->setDefaultSectionSize(50);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setHidden(true);
    ui->tableView->setAlternatingRowColors(true);

    ui->radioButton_before->setChecked(true);
}

void dSyncBoard::initSignals()
{

    connect (ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_accepted(QAbstractButton*)));

    connect (ui->radioButton_before, SIGNAL(clicked()), this, SLOT(slotShowSoftPlateBefore()));
    connect (ui->radioButton_after, SIGNAL(clicked()), this, SLOT(slotShowSoftPlateAfter()));

    connect(&m_softPlateModel[1], SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotSoftPlateItemChanged(QStandardItem*)));

    connect (this, SIGNAL(signalRequireSoftPlateData()), core.substationServer(), SLOT(slotRequireSoftPlateData()));
    connect (core.substationServer(), SIGNAL(signalSoftPlateDataReady()), this, SLOT(slotUpdateFromServer()));

//    connect (core.substationServer(), SIGNAL(signalServerChanged()), this, SLOT(slotUpdateServerInfo()));
}
#include <QDebug>
bool dSyncBoard::updateThis(bool connected)
{
//    qDebug() << connected;

    if (bUpdateSoftPlate) {

        if (connected) {//update from server.
//            qDebug() << "connect";
            this->slotUpdateServerInfo();
            emit signalRequireSoftPlateData();
            ui->label->setText(QString("正在从服务器加载软压板信息,请稍后..."));
        } else {
//            qDebug() << "disconnect";
            ui->label->setText(QString("服务器断开,请从本地加载软压板配置信息"));
        }

    } else {
        //data not changed.
    }
//    if (connected) {//update from server.

////        this->updateDataFromServer();

//    } else {    //update from file in local.
//        if (!bUpdateSoftPlate) {
////            ui->tableView->setModel(&m_softPlateModel[0]);
////            ui->label->setText(QString("[本地]软压板数据更新成功！"));
//            return true;
//        } else {
//            ui->label->setText(QString("服务器断开,请从本地加载软压板配置信息"));
//        }
//    }

    return true;
}

void dSyncBoard::slot_accepted(QAbstractButton *btn)
{
    if (btn == ui->buttonBox->button(QDialogButtonBox::Ok)) {

        this->saveSoftPlateData();

    } else {
        //do nothing.
    }
}
//#define SUBSTATION_SOFTPLATE_PATH ("./config/quicktest/SubstationModel/SoftPlate/")
//#define SUBSTATION_SOFTPLATE_DATA ("./SubstationModel/SoftPlate/softplate.xml")
#include "filepath.h"
#include "customWidget/importdlg.h"
void dSyncBoard::on_btn_import_clicked()
{


    QStringList nameFilters;
    nameFilters << "*.xml";

#if defined (Q_OS_WIN32)
    QString rootPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(WIN_YB_OUTLINE_FILE);
#elif defined (Q_OS_LINUX)
    QString rootPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(LINUX_YB_OUTLINE_FILE);
#endif

    importDlg import( trUtf8("导入离线软压板配置"), rootPath, nameFilters, 0 );
    if (import.exec() == QDialog::Accepted) {

        if (import.filePath().isEmpty()) {
            return;
        }

        this->updateFromLocal(import.filePath());
    }

    return;

}
#include <QMessageBox>
void dSyncBoard::slotUpdateServerInfo()
{
    this->m_pThread = core.substationServer()->workThread();
    this->m_curServerName = core.substationServer()->serverName();

    if (m_pThread == NULL || m_curServerName.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("服务器信息无效"), QObject::tr("软压板更新失败,请选择软压板文件手动配置."), QMessageBox::Ok);
    }
}

void dSyncBoard::slotUpdateFromServer()
{
//    qDebug() << "recive";
    PEQU_PARAM pParam = m_pThread->getEquipParam(m_curServerName.toAscii().data());

    for (int k = 0; k < 2; k++) {
        if (!pParam)
        {
            //清除数据
            m_softPlateModel[k].removeRows(0, m_softPlateModel[k].rowCount());
            return ;
        }

        bUpdateSoftPlate = true;

        QStandardItemModel *pCurModel = &m_softPlateModel[k];

        //清除数据
        pCurModel->removeRows(0,pCurModel->rowCount());

        for (int i = 0 ; i < pParam->yb_Count ; i++ )
        {
            SETTING_ITEM1 &item = pParam->yb_Item[i];

            int count = pCurModel->rowCount();
            pCurModel->insertRows(count,1);

            pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(count+1),	   Qt::EditRole);
            pCurModel->item(count,0)->setEditable(false);

            //pCurModel->setData(pCurModel->index(count,1),QString::fromUtf8(item.desc,strlen(item.desc)),	   Qt::EditRole);
            pCurModel->setData(pCurModel->index(count,1),item.desc,	   Qt::EditRole);
            pCurModel->item(count,1)->setEditable(false);

            pCurModel->setData(pCurModel->index(count,2),QString("%1").arg(item.val_Str),		   Qt::EditRole);

            pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(item.ref),       Qt::UserRole);
            pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(item.data_type), Qt::UserRole+1);
            pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(item.data_size), Qt::UserRole+2);
            pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(item.data_len), Qt::UserRole+3);

        }
    }
    bUpdateSoftPlate = false;
    ui->tableView->setModel(&m_softPlateModel[0]);

    ui->label->setText(QString("[服务器]软压板数据更新成功！"));
}

#include <QXmlStreamReader>
#include <QStandardItemModel>
#include <QDebug>
void dSyncBoard::updateFromLocal(QString xmlPath)
{
    //清除数据
    m_softPlateModel[0].removeRows(0, m_softPlateModel[0].rowCount());
    m_softPlateModel[1].removeRows(0, m_softPlateModel[1].rowCount());
    bUpdateSoftPlate = true;

    QFile file(xmlPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "SoftPlate") {


                for (int i = 0; i < 2; i++) {

                    QStandardItemModel *pCurModel = &m_softPlateModel[i];

                    int count = pCurModel->rowCount();
                    pCurModel->insertRows(count,1);

                    pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(count+1), Qt::EditRole);
                    pCurModel->item(count,0)->setEditable(false);

                    //pCurModel->setData(pCurModel->index(count,1),QString::fromUtf8(item.desc,strlen(item.desc)),	   Qt::EditRole);
                    pCurModel->setData(pCurModel->index(count,1), xml.attributes().value("desc").toString(), Qt::EditRole);
                    pCurModel->item(count,1)->setEditable(false);

                    pCurModel->setData(pCurModel->index(count,2),QString("%1").arg(xml.attributes().value("value").toString()), Qt::EditRole);

                    pCurModel->setData(pCurModel->index(count,0),QString("%1").arg(xml.attributes().value("ref").toString()), Qt::UserRole);
                }

            }
        }
    }

    if (xml.hasError()) {
        QMessageBox::information(this, "解析软压板信息", xml.errorString());
    }

    xml.clear();
    file.close();

    bUpdateSoftPlate = false;
    ui->tableView->setModel(&m_softPlateModel[0]);

    ui->label->setText(QString("[本地]软压板数据更新成功！"));
}

void dSyncBoard::slotShowSoftPlateBefore()
{
    ui->tableView->setModel(&m_softPlateModel[0]);
}
void dSyncBoard::slotShowSoftPlateAfter()
{
    ui->tableView->setModel(&m_softPlateModel[1]);
}

void dSyncBoard::slotSoftPlateItemChanged(QStandardItem* item)
{
    if( (item == NULL) ||  (item->column()!= 2)  || (bUpdateSoftPlate == true) )
        return ;
    //设置个标志
    item->setData(1,Qt::UserRole);
}

bool dSyncBoard::getItemFlagsOfSoftPlate(QList<MODIFY_ITEM *> &settingList)
{
    QStandardItemModel *m_boardModel = &m_softPlateModel[1];
    bool bRet = false;

     for (int iRow = 0 ; iRow < m_boardModel->rowCount() ; iRow++)
     {
         QStandardItem *item0 = m_boardModel->item(iRow,0);
         QStandardItem *item1 = m_boardModel->item(iRow,1);
         QStandardItem *item2 = m_boardModel->item(iRow,2);


         if ( (item0 == NULL) || (item1 == NULL) || (item2 == NULL))
             continue;

         int flag= item2->data(Qt::UserRole).toInt();
         if (flag != 1)
             continue;

         MODIFY_ITEM *pSItem = new MODIFY_ITEM();
         settingList.push_back(pSItem);

         pSItem->pStanItem = item2;
         sprintf(pSItem->item.ref,"%s",item0->data(Qt::UserRole).toString().toAscii().data() );
         pSItem->item.data_type = item0->data(Qt::UserRole+1).toInt();
         pSItem->item.data_size = item0->data(Qt::UserRole+2).toInt();
         pSItem->item.data_len = item0->data(Qt::UserRole+3).toInt();

         sprintf(pSItem->item.desc,"%s",item1->data(Qt::EditRole).toString().toAscii().data());
         sprintf(pSItem->item.val_Str,"%s",item2->data(Qt::EditRole).toString().toAscii().data());


         bRet = true;

     }

     return bRet;
}

void dSyncBoard::saveSoftPlateData()
{
    QList<MODIFY_ITEM *> ybList;

    if (getItemFlagsOfSoftPlate(ybList) == false)
    {
        QMessageBox::warning(this, QObject::tr("注意"), QObject::tr("没有修改的软压板项"), QMessageBox::Ok);
    }

    //将软压板信息写入模板
    for (int i = 0; i < this->m_softPlateModel[0].rowCount(); i++) {
        QString desc;
        int value;
        QString ref;
        value = m_softPlateModel[0].item(i, 2)->text().toInt();
        desc = m_softPlateModel[0].item(i, 1)->text();
        ref = m_softPlateModel[0].item(i, 0)->data(Qt::UserRole).toString();
        core.templateModel()->currentAction()->softPlate()->append(CActionSoftPlate::Before, desc, value, ref);
    }

    for (int i = 0; i < this->m_softPlateModel[1].rowCount(); i++) {
        QString desc;
        int value;
        QString ref;
        value = m_softPlateModel[1].item(i, 2)->text().toInt();
        desc = m_softPlateModel[1].item(i, 1)->text();
        ref = m_softPlateModel[1].item(i, 0)->data(Qt::UserRole).toString();
        core.templateModel()->currentAction()->softPlate()->append(CActionSoftPlate::After, desc, value, ref);
    }

    //    core.templateModel()->currentAction()->board()->
}
