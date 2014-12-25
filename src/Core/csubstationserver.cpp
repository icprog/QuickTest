#include "csubstationserver.h"
#include <QMessageBox>
#include <QDebug>

CSubstationServer* CSubstationServer::m_pInstance = 0;

CSubstationServer::CSubstationServer(QObject *parent) :
    QObject(parent),
    bConnectState(false),
    bExportSoftPlate(false)
{}

bool CSubstationServer::init()
{
    this->readClientInfo();
    if (m_sysImpl.m_sysNode.domainList.size() >0)
    {
        m_domain = QString(m_sysImpl.m_sysNode.domainList[0]->name);
    }

    loadClientInfo();

    m_pThread = new ClientCmdThread(this);
    if (!m_pThread) {
        return false;
    }
    connect(m_pThread,    SIGNAL(resultData_signals(CMD_GEN*)),
            this,         SLOT(resultData_slots(CMD_GEN *)));
    m_pThread->setWinFlag(true);
    sprintf(m_sysImpl.m_envirNode.cfgDir,"%s/config",QApplication::applicationDirPath().toAscii().data());
    sprintf(m_sysImpl.m_envirNode.dataDir,"%s/data",QApplication::applicationDirPath().toAscii().data());

    m_pThread->setDir(m_sysImpl.m_envirNode.cfgDir,m_sysImpl.m_envirNode.dataDir);

    //启动命令处理线程
    if(!m_pThread->start()) {
        return false;
    }

    /***/
    if(auto_test_init(NULL) != 0)
    {

        //复制文件
        QString  osicfgName = QString("%1/config/osicfg.xml").arg(QApplication::applicationDirPath());
        QString  osicfgName1 = QString("%1/config/osicfg1.xml").arg(QApplication::applicationDirPath());


        bool bRet = QFile::remove(osicfgName);
        bRet = QFile::copy(osicfgName1,osicfgName);

        QString  logcfgName = QString("%1/config/logcfg.xml").arg(QApplication::applicationDirPath());
        QString  logcfgName1 = QString("%1/config/logcfg1.xml").arg(QApplication::applicationDirPath());

        bool bRet1 = QFile::remove(logcfgName);
        bRet1 = QFile::copy(logcfgName1,logcfgName);

        if(auto_test_init(NULL) != 0)
        {
//            QMessageBox::critical(NULL, QObject::tr("错误"), QObject::tr("MMS初始化失败！"));
            return false;
        }

    }

    return true;
}

void CSubstationServer::readClientInfo()
{
    QString fileName = QString("%1/config/%2")
                    .arg(QApplication::applicationDirPath())
                    .arg(SYSCFG);
    m_sysImpl.read(fileName.toAscii().data());
}

void CSubstationServer::loadClientInfo()
{
    for (uint i = 0 ; i < m_sysImpl.m_sysNode.domainList.size() ; i++)
    {
        PDOMAINNODE pDomain = m_sysImpl.m_sysNode.domainList[i];

        for (uint j = 0 ; j < pDomain->staList.size() ; j++)
        {
            PSTANODE pSta = pDomain->staList[j];


            for (uint ll = 0 ; ll < pSta->iedList.size()  ;ll++)
            {
                PEQUIPNODE pEqu = pSta->iedList[ll];


                QString serverName1 = tr("%1").arg(pEqu->name);
                m_curServerName = serverName1.mid(0,serverName1.indexOf("("));


                m_curIpAddr = tr("%1").arg(pEqu->ip);

                strcpy(m_sclPath.scl_file_name,pEqu->scl.scl_file_name);
                strcpy(m_sclPath.ied_name,pEqu->scl.ied_name);
                strcpy(m_sclPath.access_point_name,pEqu->scl.access_point_name);

                emit signalServerChanged();
                //更新mms_configdialog
//                ui.lineEdit_2->setText(QString(m_sclPath.ied_name));
//                ui.lineEdit->setText(QString(m_sclPath.scl_file_name));
//                ui.lineEdit_3->setText(QString(m_sclPath.access_point_name));
//                ui.lineEdit_4->setText(m_curIpAddr);

                break;
            }
        }
    }
}

void CSubstationServer::slotConnect()
{
    if (m_curServerName == "")
    {
        QMessageBox::warning(0, "请选择要操作的装置", "你还没有选择要操作的装置,请导入要操作的设备文件");
        return ;
    }

    CMD_GEN cmd;

    cmd.cmdCode = CMD_CONNECT;
    sprintf(cmd.serverName,"%s",m_curServerName.toAscii().data());
    sprintf(cmd.equpName,"%s",m_sclPath.ied_name);

    sprintf(cmd.sclPath.scl_file_name,"%s",m_sclPath.scl_file_name);
    sprintf(cmd.sclPath.ied_name,"%s",m_sclPath.ied_name);
    sprintf(cmd.sclPath.access_point_name,"%s",m_sclPath.access_point_name);
    cmd.sclPath.enabled = m_sclPath.enabled;

    m_pThread->appendCmd(cmd);

    emit signalConnectStateChanged(this->WAIT);
}

void CSubstationServer::slotDisconnect()
{
    if (m_curServerName == "")
    {
        QMessageBox::warning(0, "请选择要操作的装置", "你还没有选择要操作的装置");
        return ;
    }
    CMD_GEN cmd;

    cmd.cmdCode = CMD_DISTCNNECT;
    sprintf(cmd.serverName,"%s",m_curServerName.toAscii().data());

    m_pThread->appendCmd(cmd);

    emit signalConnectStateChanged(this->WAIT);
}

void CSubstationServer::resultData_slots(PCMD_GEN pCmd)
{

    static bool bConnected = true;

    bool bLog = true;

    switch(pCmd->cmdCode ) {

    case CMD_CONNECT:                   //连接
        {

            if (pCmd->result == CMD_RESULT_RESULT_FAILD)  //连接失败，弹出对话框
            {
                QString event = QString::fromLocal8Bit(pCmd->resultDesc);
                bConnected = false;
                emit signalConnectFaild(event);
                return ;
            }
            else if (pCmd->result == CMD_RESULT_BEGINPROCESS)
            {
                bLog = false;
            }
            else if (pCmd->result ==  CMD_RESULT_ENDBEGINPROCESS)
            {
                if (bConnected) {
                    emit signalConnectStateChanged(this->CONNECT);
                    bConnectState = true;
                } else {
                    emit signalConnectStateChanged(this->DISCONNECT);
                    bConnectState = false;
                }
                bLog = false;
            }

//            bDisconnected = false;
        }
        break;
    case CMD_DISTCNNECT:                    //断开连接
        {
            emit signalConnectStateChanged(this->DISCONNECT);
        }
        break;
    case CMD_INIT_MODEL:
        {

        if ((pCmd->result == CMD_RESULT_SUCCESS) && (QString::fromLocal8Bit(pCmd->resultDesc) == m_curServerName) )
            {
                //未知
                bConnectState = false;
            }

        }
        break;
    case CMD_EQU_PARAM:                     //召唤装置参数
        {

            if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
                if (&m_parameter)
                {
                    //调用Mms_Parameter类显示定值参数的方法
                    m_parameter.bUpdateParameter();
                }
                */
            }
            else if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {
                bLog = false;
            }
        }
        break;
    case CMD_MODIFY_PARAM:                  //装置参数修改结果
        {

            if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {

                bLog = false;
            }
            else if (pCmd->result ==CMD_RESULT_BEGINPROCESS )
            {
                bLog = false;
            }
            else if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
                if (&m_parameter) {
                    //调用Mms_Parameter类还原定值表标志的方法
                  m_parameter.modifyParameterFlag(pCmd);
                }
                */
                bLog = true;
            }
            else if (pCmd->result == CMD_RESULT_RESULT_FAILD)
            {
//                QMessageBox::critical(0, "警告", "修改装置参数失败！");
                bLog = true;
            }

        }
        break;
    case CMD_BECKON_SETTING:                //召唤定值
        {

            if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
                if (&m_parameter)
                {
                    //调用Mms_Parameter类显示定值参数的方法
                    m_parameter.bUpdateValue();
                }
                */
            }
            else if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {
                bLog = false;
            }

        }
        break;
    case CMD_MODIFY_SETTING:                //定值修改结果
        {

            if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {
                bLog = false;
            }
            else if (pCmd->result ==CMD_RESULT_BEGINPROCESS )
            {
                bLog = false;
            }
            else if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
                if (&m_parameter) {
                    //调用Mms_Parameter类还原定值表标志的方法
                  m_parameter.modifyValueFlag(pCmd);
                }
                */
                bLog = true;
            }
            else if (pCmd->result == CMD_RESULT_RESULT_FAILD)
            {
                QMessageBox::critical(0, "警告", "修改定值失败！");
                bLog = true;
            }

        }
        break;
    case CMD_BECKON_YB:                     //召唤压板
        {
            if (pCmd->result == CMD_RESULT_SUCCESS)
            {

                if (this->bExportSoftPlate) {

                    if (this->exportSoftPlate())
                        emit signalExportSoftPlateDone(true);
                    else
                        emit signalExportSoftPlateDone(false);

                    bExportSoftPlate = false;
                } else {

                    emit signalSoftPlateDataReady();

                }

            }
            else if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {
                bLog = false;
            }
        }
        break;
    case CMD_MODIFY_YB:                     //修改软压板结果
        {

            if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {
                bLog = false;
            }
            else if (pCmd->result ==CMD_RESULT_BEGINPROCESS )
            {
                bLog = false;
            }
            else if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
                //调用Mms_Parameter类还原压板表标志的方法
                if (&m_parameter) {
                    m_parameter.modifyBoardFlag(pCmd);
                }
                */
                emit modifyYbSuccess();

                bLog = true;
            }
            else if (pCmd->result == CMD_RESULT_RESULT_FAILD)
            {
                QMessageBox::critical(0, "警告", "修改压板失败！");
                bLog = true;
            }

        }
        break;
    case CMD_BECKON_SAMPLE:                //召唤模拟量
        {

            if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
               if (&m_signal)
                   m_signal.bUpdateSimulation();
                */
            }
            else if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {

                bLog = false;
            }
        }
        break;
    case CMD_BECKON_SWITCH:                  //召唤开关量
        {
            if (pCmd->result == CMD_RESULT_SUCCESS)
            {
                /*
                if (&m_signal)
                    m_signal.bUpdateSwitch();
                */
            }
            else if  (pCmd->result == CMD_RESULT_ENDBEGINPROCESS)
            {
                bLog = false;
            }
        }
        break;
    case CMD_REPORT:                       //报告
        {
            if (pCmd->result == CMD_RESULT_PROCESSING)
            {
                /*
                if (&m_report)
                 m_report.showReport(pCmd);
                */
                bLog = false;

            }
        }
        break;
    case CMD_SCD_CHECK:
        {
            if (pCmd->pos == COMP_ITEM)
            {
                /*
                if (&m_checkModel)
                {
                    m_checkModel.bUpdateDetail(pCmd);
                }
                */

                bLog = false;
                pCmd->result = CMD_RESULT_SUCCESS;

            }
            else if (pCmd->pos == COMP_RESULT)
            {
                bLog = false;
                /*
                if (&m_checkModel)
                {
                    m_checkModel.bUpdateResult(pCmd);
                }
                */

                pCmd->result = CMD_RESULT_SUCCESS;
            }

        }
        break;
    }//End Of Switch

    if(pCmd->result == CMD_RESULT_HINT)
        bLog = true;

//    QString text = QString::fromAscii(pCmd->resultDesc);
    QString text = QString::fromLocal8Bit(pCmd->resultDesc);

    if (bLog)
    {
        emit signalHint(text);
    }
}
#include <vector>
void CSubstationServer::setIp(QString strIP)
{

    QStringList strList = strIP.split(".");

    if( strList.count() !=4)
        return ;


    std::vector <PDOMAINNODE>::iterator Iter;
    std::vector <PSTANODE>::iterator Iter1;
    std::vector <PEQUIPNODE>::iterator Iter2;

    for ( Iter = m_sysImpl.m_sysNode.domainList.begin( ) ; Iter != m_sysImpl.m_sysNode.domainList.end( ) ; Iter++ )
    {

          for (Iter1 = (*Iter)->staList.begin() ; Iter1 != (*Iter)->staList.end() ; Iter1++)
          {
              //if (QString("%1").arg((*Iter1)->name) == m_sta)   //站
              {
                 for (Iter2 = (*Iter1)->iedList.begin() ; Iter2 != (*Iter1)->iedList.end() ; Iter2++)
                  {
                      QString strName = QString("%1").arg((*Iter2)->name);
                     //if (strName == m_equ)  //装置
                      {
                          sprintf((*Iter2)->ip,"%s",strIP.toAscii().data());
                          break;
                      }

                  }

              }


          }
    }


    QString fileName1 = QString("%1/config/%2")
                    .arg(QApplication::applicationDirPath())
                    .arg(SYSCFG);

    m_sysImpl.write(fileName1.toAscii().data());

    loadClientInfo();
    updateOsicfg();

}

void CSubstationServer::slotRequireSoftPlateData()
{
    PEQU_PARAM pEqu = m_pThread->getEquipWithAppend(m_curServerName.toAscii().data());

    if (pEqu == NULL)
        return ;


    CMD_GEN cmd;

    cmd.cmdCode = CMD_BECKON_YB;
    sprintf(cmd.serverName,"%s",m_curServerName.toAscii().data());


    m_pThread->appendCmd(cmd);
}
#include <QTextCodec>
void CSubstationServer::updateOsicfg()
{


    QString  osicfgName = QString("config/osicfg.xml");

    QDomDocument doc;
    QFile xmlFile;
    xmlFile.setFileName(osicfgName);

    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        QString title = QString("打开%1文件").arg(osicfgName);
        QString text  = QString("打开%1文件,可能不存在该文件!").arg(osicfgName);
        QMessageBox::warning(NULL,title,text);
        return ;
    }

    QTextStream floStream(&xmlFile);
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    floStream.setCodec(codec);

    QString xmlDataStr = floStream.readAll();


    if (!doc.setContent(xmlDataStr))
    {

        QString title = QString("装载%1文件").arg(osicfgName);
        QString text  = QString("装载%1文件,可能该XML文件有错误!").arg(osicfgName);
        QMessageBox::warning(NULL,title,text);

        xmlFile.close();
        return ;
    }

    xmlFile.close();


     QDomNodeList addrListListNode;
     addrListListNode = doc.elementsByTagName("RemoteAddressList");

     //删除远程对象列表
     QDomNode  addrListNode;
     if (addrListListNode.count() >0)
     {
         addrListNode = addrListListNode.at(0);

         QDomNodeList  addrChildList = addrListNode.childNodes();

         for (int j = addrChildList.count() ; j > 0  ;j--)
         {

             QDomNode node1 = addrChildList.at(j-1);
             addrListNode.removeChild(node1);
         }
     }

     int ipCount = 0;

    //添加远程对象列表
     for (uint i = 0 ; i < m_sysImpl.m_sysNode.domainList.size() ; i++)
    {
        PDOMAINNODE pDomain = m_sysImpl.m_sysNode.domainList[i];


        for (uint j = 0 ; j < pDomain->staList.size() ; j++)
        {
            PSTANODE pSta = pDomain->staList[j];


            for (uint ll = 0 ; ll < pSta->iedList.size()  ;ll++)
            {
                PEQUIPNODE pEqu = pSta->iedList[ll];

                if (QString(pEqu->ip) == "")
                    continue;

                ipCount++;

                QDomElement remoteAddrElem = doc.createElement("RemoteAddress");
                addrListNode.appendChild(remoteAddrElem);

                QDomElement AR_NameElem = doc.createElement("AR_Name");


                 QString serverName1 = tr("%1").arg(pEqu->name);
                 QString serverName = serverName1.mid(0,serverName1.indexOf("("));
                 QDomText newnode=doc.createTextNode(serverName);
                 remoteAddrElem.appendChild(AR_NameElem);
                 AR_NameElem.appendChild(newnode);


                 QDomElement AP_TitleElem = doc.createElement("AP_Title");
                 newnode=doc.createTextNode(tr("1 3 9999 23"));
                 remoteAddrElem.appendChild(AP_TitleElem);
                 AP_TitleElem.appendChild(newnode);

                 QDomElement AE_QualifierElem = doc.createElement("AE_Qualifier");
                 newnode=doc.createTextNode(tr("23"));
                 remoteAddrElem.appendChild(AE_QualifierElem);
                 AE_QualifierElem.appendChild(newnode);

                 QDomElement PselElem = doc.createElement("Psel");
                 newnode=doc.createTextNode(tr("00 00 00 01"));
                 remoteAddrElem.appendChild(PselElem);
                 PselElem.appendChild(newnode);

                 QDomElement SselElem = doc.createElement("Ssel");
                 newnode=doc.createTextNode(QString("00 01"));
                 remoteAddrElem.appendChild(SselElem);
                 SselElem.appendChild(newnode);

                 QDomElement TselElem = doc.createElement("Tsel");
                 newnode=doc.createTextNode(tr("00 01"));
                 remoteAddrElem.appendChild(TselElem);
                 TselElem.appendChild(newnode);

                 QDomElement NetAddrElem = doc.createElement("NetAddr");
                 newnode=doc.createTextNode(tr("%1").arg(pEqu->ip));
                 remoteAddrElem.appendChild(NetAddrElem);
                 NetAddrElem.appendChild(newnode);
                 QDomAttr    netAddrattrib = doc.createAttribute("Type");
                 netAddrattrib.setValue("IPADDR");
                 NetAddrElem.setAttributeNode(netAddrattrib);



                 ///多加一个为mms模型检查
                 remoteAddrElem = doc.createElement("RemoteAddress");
                addrListNode.appendChild(remoteAddrElem);

                 AR_NameElem = doc.createElement("AR_Name");


                 QString serverName2 = QString("%1_R").arg(serverName);
                 newnode=doc.createTextNode(serverName2);
                 remoteAddrElem.appendChild(AR_NameElem);
                 AR_NameElem.appendChild(newnode);


                 AP_TitleElem = doc.createElement("AP_Title");
                 newnode=doc.createTextNode(tr("1 3 9999 23"));
                 remoteAddrElem.appendChild(AP_TitleElem);
                 AP_TitleElem.appendChild(newnode);

                 AE_QualifierElem = doc.createElement("AE_Qualifier");
                 newnode=doc.createTextNode(tr("23"));
                 remoteAddrElem.appendChild(AE_QualifierElem);
                 AE_QualifierElem.appendChild(newnode);

                 PselElem = doc.createElement("Psel");
                 newnode=doc.createTextNode(tr("00 00 00 01"));
                 remoteAddrElem.appendChild(PselElem);
                 PselElem.appendChild(newnode);

                 SselElem = doc.createElement("Ssel");
                 newnode=doc.createTextNode(QString("00 01"));
                 remoteAddrElem.appendChild(SselElem);
                 SselElem.appendChild(newnode);

                 TselElem = doc.createElement("Tsel");
                 newnode=doc.createTextNode(tr("00 01"));
                 remoteAddrElem.appendChild(TselElem);
                 TselElem.appendChild(newnode);

                 NetAddrElem = doc.createElement("NetAddr");
                 newnode=doc.createTextNode(tr("%1").arg(pEqu->ip));
                 remoteAddrElem.appendChild(NetAddrElem);
                 NetAddrElem.appendChild(newnode);
                 netAddrattrib = doc.createAttribute("Type");
                 netAddrattrib.setValue("IPADDR");
                 NetAddrElem.setAttributeNode(netAddrattrib);

            }

        }

    }

     //如果远程地址的个数为0 ，则手动填写一个，不能一个都没有,要不mms启动会报错
     if (ipCount == 0)
     {
         QDomElement remoteAddrElem = doc.createElement("RemoteAddress");
        addrListNode.appendChild(remoteAddrElem);

        QDomElement AR_NameElem = doc.createElement("AR_Name");


        QString serverName = tr("%1").arg("127.0.0.1");
         QDomText newnode=doc.createTextNode(serverName);
         remoteAddrElem.appendChild(AR_NameElem);
         AR_NameElem.appendChild(newnode);


         QDomElement AP_TitleElem = doc.createElement("AP_Title");
         newnode=doc.createTextNode(tr("1 3 9999 23"));
         remoteAddrElem.appendChild(AP_TitleElem);
         AP_TitleElem.appendChild(newnode);

         QDomElement AE_QualifierElem = doc.createElement("AE_Qualifier");
         newnode=doc.createTextNode(tr("23"));
         remoteAddrElem.appendChild(AE_QualifierElem);
         AE_QualifierElem.appendChild(newnode);

         QDomElement PselElem = doc.createElement("Psel");
         newnode=doc.createTextNode(tr("00 00 00 01"));
         remoteAddrElem.appendChild(PselElem);
         PselElem.appendChild(newnode);

         QDomElement SselElem = doc.createElement("Ssel");
         newnode=doc.createTextNode(QString("00 01"));
         remoteAddrElem.appendChild(SselElem);
         SselElem.appendChild(newnode);

         QDomElement TselElem = doc.createElement("Tsel");
         newnode=doc.createTextNode(tr("00 01"));
         remoteAddrElem.appendChild(TselElem);
         TselElem.appendChild(newnode);

         QDomElement NetAddrElem = doc.createElement("NetAddr");
         newnode=doc.createTextNode(tr("%1").arg("127.0.0.1"));
         remoteAddrElem.appendChild(NetAddrElem);
         NetAddrElem.appendChild(newnode);
         QDomAttr    netAddrattrib = doc.createAttribute("Type");
         netAddrattrib.setValue("IPADDR");
         NetAddrElem.setAttributeNode(netAddrattrib);

     }

     //重新写入文件
     QFile xmlFile1(osicfgName);

    if(!xmlFile1.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return  ;
    }

    QTextStream out(&xmlFile1);

    QTextCodec *codec1=QTextCodec::codecForName("UTF-8");
    out.setCodec(codec1);

    doc.save(out, 4);


    //立刻把文件写进硬盘
    xmlFile1.flush();
    #if !defined(Q_OS_WIN)
      ::fsync(xmlFile1.handle());
    #endif

    xmlFile1.close();

    auto_test_reload_osicfg();//yhc


    //复制文件
    QString  osicfgName1 = QString("config/osicfg1.xml");

    QFile::remove(osicfgName1);
    QFile::copy(osicfgName,osicfgName1);


    QString  logcfgName = QString("config/logcfg.xml");
    QString  logcfgName1 = QString("config/logcfg1.xml");

    QFile::remove(logcfgName1);
    QFile::copy(logcfgName,logcfgName1);

}

void CSubstationServer::querySoftPlate(bool isExport)
{
    this->bExportSoftPlate = isExport;

    PEQU_PARAM pEqu = m_pThread->getEquipWithAppend(m_curServerName.toAscii().data());

    if (pEqu == NULL)
        return ;


    CMD_GEN cmd;

    cmd.cmdCode = CMD_BECKON_YB;
    sprintf(cmd.serverName,"%s",m_curServerName.toAscii().data());


    m_pThread->appendCmd(cmd);
}

#include <QXmlStreamWriter>
#include <QDir>

#define SUBSTATION_SOFTPLATE_PATH ("./SubstationModel/SoftPlate/")
#define SUBSTATION_SOFTPLATE_DATA ("./SubstationModel/SoftPlate/softplate.xml")
bool CSubstationServer::exportSoftPlate()
{
    PEQU_PARAM pParam = m_pThread->getEquipParam(m_curServerName.toAscii().data());

    if (!pParam)
    {
        return false;
    }

    if (!QDir().mkpath(SUBSTATION_SOFTPLATE_PATH)) {
        return false;
    }

    QFile file(SUBSTATION_SOFTPLATE_DATA);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("SoftPlateGroup");

    for (int i = 0 ; i < pParam->yb_Count ; i++ )
    {
        SETTING_ITEM1 &item = pParam->yb_Item[i];

        writer.writeStartElement("SoftPlate");

        writer.writeAttribute("desc", item.desc);
        writer.writeAttribute("value", item.val_Str);
        writer.writeAttribute("ref", item.ref);

        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();

//    this->bExportSoftPlate = false;
    return true;
}
