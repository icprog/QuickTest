#include "clientcmdthread.h"
#include "clienttoolfun.h"
#include "clientscdphase.h"
#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtDebug>
#include <time.h>



ClientCmdThread *g_thread = NULL;


ClientCmdThread::ClientCmdThread(QObject *parent)
	: QObject(parent)
{

	g_thread = this;
	initVars();

}

ClientCmdThread::~ClientCmdThread()
{

	closeThread();
}


void  ClientCmdThread::initVars()
{

	memset(m_cfgDir,0x00,sizeof(m_cfgDir));
	memset(m_dataDir,0x00,sizeof(m_dataDir));
	m_bReadScd = false;
	m_isShowDownDlg = false;


}


bool ClientCmdThread::start()
{

    m_exit = false;
	m_trheadExt = false;

	bool  ret = Comm_CreateThread(m_handle,(ptrFun)circleThread,this);

	if (ret == false)
	{
		 QString str = "创建监听线程失败";
		 ToolFun_TraceInfo(NULL,str.toAscii().data());
	}

	

	return ret;



}


void ClientCmdThread::closeThread()
{

	m_exit = true;

	qint32   interval   = 25*1000 ;  //2s

	ulong	 beginTick	= Comm_GetTickCount();
	ulong   curTick ;

	while(  !m_trheadExt )
	{	
		curTick = Comm_GetTickCount();
		ulong  offset = curTick - beginTick;

		if (offset > interval) 
		{	
			break;
		}
		mySleep(50);
	}


}

void  ClientCmdThread::circleThread(void *pVoid)
{

   ClientCmdThread *pInf = (ClientCmdThread *)pVoid;

   Q_ASSERT(pInf);

   pInf->circleFun();


}

void ClientCmdThread::circleFun()
{

	while(!m_exit)
	{	
		readScdCfg();
	    dealCmd();
	
		mySleep(1);
				
	}
    m_exit = true;
	m_trheadExt = true;
	QString str = "线程退出";
	ToolFun_TraceInfo(NULL,str.toAscii().data());
	Comm_exitThread(m_handle);

}


void   ClientCmdThread::dealCmd()
{


	m_cmdLock.lock();

	if (m_cmdList.isEmpty())
	{
		m_cmdLock.unlock();
	}
	else
	{
		PCMD_GEN pCmd =m_cmdList.first();
		m_cmdLock.unlock();

		sendCmd(pCmd);
		
		//从队列中删除
		m_cmdLock.lock();
		m_cmdList.removeAll(pCmd);
		 delete pCmd;
		m_cmdLock.unlock();

	}

}


void  ClientCmdThread::sendCmd(PCMD_GEN pCmd)
{
	


	switch(pCmd->cmdCode )
	{
		case CMD_CONNECT:                       //连接
			connectServer(pCmd);
			break;
		case CMD_DISTCNNECT:                    //断开
			destConnect(pCmd);
			break;
		
		case CMD_BECKON_SETTING:                //召唤定值
		case CMD_BECKON_YB:
		case CMD_EQU_PARAM:
		case CMD_BECKON_SWITCH:
		case CMD_BECKON_SAMPLE:
			{
				getRealay(pCmd);
			}
		break;
		case CMD_MODIFY_SETTING:                    //修改定值
			{
				modifySetting(pCmd);
			}
			break;
		case CMD_MODIFY_YB:                 //修改压板
		{
			modifyYb(pCmd);
		}
		break;
		case CMD_MODIFY_PARAM:               //修改装置参数
		{
		   modifyParam(pCmd);
		}
		break;
		case CMD_DESCODE_PT_SCL:
		case CMD_DESCODE_IP_SCL:                   //解析scl文件
		{
			decodeScl(pCmd);
		}
	    break;
		case CMD_SCD_CHECK:
			{
				modelCheck(pCmd);
			}
			break;
	}



}



PEQU_PARAM ClientCmdThread::getEquipParam(t_char *serverName)
{

	for (int i = 0 ; i < m_equParamList.count() ; i++)
	{

		PEQU_PARAM pEqu = m_equParamList.at(i);

		if (strcmp(pEqu->serverName,serverName) == 0)
		{
			return pEqu;
		}
	}

	return NULL;

}


PEQU_PARAM ClientCmdThread::getEquipWithAppend(t_char *serverName)
{


	PEQU_PARAM pEqu = getEquipParam(serverName);
	if (!pEqu)
	{
		pEqu = new EQU_PARAM();

		pEqu->bInitModel = false;
		pEqu->bInitSetting = false;
		pEqu->bConnect = false;

		sprintf(pEqu->serverName,"%s",serverName);
		m_equParamList.push_back(pEqu);
	}

	return pEqu;

}


void    ClientCmdThread::destConnect(PCMD_GEN pCmd)
{

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	t_int  ret = auto_test_disconnect_equip(pCmd->serverName,NULL);

   t_char desc[MAX_NAMELEN];
	if (ret == 0 )
	{		
		sprintf(desc,"断开成功");
		pEqu->bConnect = false;
		pCmd->result = CMD_RESULT_SUCCESS;

	}
	else
	{
		sprintf(desc,"已经处于断开状态");
		pEqu->bConnect = false;
		pCmd->result = CMD_RESULT_RESULT_FAILD;

	}

	resultCmd(pCmd,desc);

}

int  ClientCmdThread::rpt_callback_fun(void* ptr, FAULT_REPORT *fault_report)
{
	t_char value[256];
	C61850_REPORT crpt;

	t_bool bRet = g_thread->getPointDesc(fault_report->server_name,fault_report->ref,false,crpt.desc,value);

	if (bRet == false)
		strcpy(crpt.desc,fault_report->ref);

	CMD_GEN *pCmd1 = new CMD_GEN();
	pCmd1->cmdCode = CMD_REPORT;


	sprintf(pCmd1->serverName,"%s",fault_report->server_name);
	sprintf(pCmd1->resultDesc,"report server:%s",fault_report->server_name);

	pCmd1->result  = CMD_RESULT_PROCESSING;

	strcpy(crpt.ref,fault_report->ref);
	strcpy(crpt.strVal,fault_report->state);
	strcpy(crpt.t,fault_report->time);

	memcpy(pCmd1->data,&crpt,sizeof(C61850_REPORT));

	emit g_thread->resultData_signals(pCmd1);

	
	return 0;
}

t_bool   ClientCmdThread::connectServer(PCMD_GEN pCmd)
{

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);

	auto_test_disconnect_equip(pCmd->serverName,NULL);

	t_char desc[MAX_NAMELEN];

	t_short cmdCode1 = pCmd->cmdCode;  

	sprintf(pEqu->equpName,"%s",pCmd->equpName);

	pCmd->cmdCode = cmdCode1;
	pCmd->result = CMD_RESULT_BEGINPROCESS;
	resultCmd(pCmd,desc);

	sprintf(desc,"正在连接 servername:%s,请稍侯...",pCmd->serverName);
	pCmd->result = CMD_RESULT_SUCCESS;
    resultCmd(pCmd,desc);

	t_int  ret = auto_test_connect_equip(pCmd->serverName, desc);
 
	if (ret == 0 )
	{		
		sprintf(desc,"连接成功 servername:%s...",pCmd->serverName);
		pEqu->bConnect = true;
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);

		if (pEqu->bInitModel == true)
		{
			pCmd->cmdCode = cmdCode1;
			pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
			resultCmd(pCmd,desc);
			return true;
		}


	   //注册故障报告回调函数
	   auto_test_set_fault_report_callback_fun(rpt_callback_fun,this);

	   m_sclFlag = false;

	   	if  ( (QString(pCmd->sclPath.scl_file_name) != "") && (QString(pCmd->sclPath.ied_name) != "") && (QString(pCmd->sclPath.access_point_name) != "") /*&& (pCmd->sclPath.enabled==true)*/ )
		{
			sprintf(desc,"%s正在装载SCL文件:%s,请稍侯...",pCmd->equpName,pCmd->sclPath.scl_file_name);
			pCmd->result = CMD_RESULT_HINT;
			resultCmd(pCmd,desc);

			strcpy(pEqu->sclPath.scl_file_name,pCmd->sclPath.scl_file_name);
			strcpy(pEqu->sclPath.ied_name,pCmd->sclPath.ied_name);
			strcpy(pEqu->sclPath.access_point_name,pCmd->sclPath.access_point_name);
			pEqu->sclPath.enabled = pCmd->sclPath.enabled;

			ret = auto_test_local_scl_model(pCmd->serverName, pCmd->sclPath.scl_file_name, pCmd->sclPath.ied_name, pCmd->sclPath.access_point_name, desc);
			if (ret != 0)
			{
				sprintf(desc,"装置SCL文件失败:%s,远程调取模型...",pCmd->sclPath.scl_file_name);
				pEqu->bConnect = false;
				pCmd->result = CMD_RESULT_SUCCESS;
				resultCmd(pCmd,desc);

				 pCmd->cmdCode = CMD_SCD_CHECK;
				 sprintf(desc,"%s装置装载SCL文件:%s失败,请确定该建模文件是否通过一致性检测",pCmd->equpName,pCmd->sclPath.scl_file_name);
				 pCmd->result = CMD_RESULT_RESULT_FAILD;
				 resultCmd(pCmd,desc);

				 strcpy(pEqu->sclPath.scl_file_name,"");
				 strcpy(pEqu->sclPath.ied_name,"");
				 strcpy(pEqu->sclPath.access_point_name,"");
				 pEqu->sclPath.enabled = 0;

				 pCmd->cmdCode =  cmdCode1 ;

			}
			else
			{
				pEqu->bInitModel = true;
				sprintf(desc,"%s装置装载SCL文件:%s成功,该配置文件检查正确",pCmd->equpName,pCmd->sclPath.scl_file_name);
				pCmd->result = CMD_RESULT_SUCCESS;
				resultCmd(pCmd,desc);

				pCmd->cmdCode = CMD_SCD_CHECK;
				sprintf(desc,"%s装置装载SCL文件:%s成功,该配置文件检查正确",pCmd->equpName,pCmd->sclPath.scl_file_name);
				pCmd->result = CMD_RESULT_SUCCESS;
				resultCmd(pCmd,desc);
				pCmd->cmdCode =  cmdCode1 ;

				m_sclFlag = true;
				if (m_winFlag )
				{
				
					ClientDSE_ScdManager scd;


					int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
					int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
					QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);	

					sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);
							
					scd.read(pEqu->ptFileName);

					pEqu->setting_Item = new SETTING_ITEM1[MAX_SETTING_NUMS];
					pEqu->param_Item   = new SETTING_ITEM1[MAX_PARAM_NUMS];
					pEqu->yb_Item      = new SETTING_ITEM1[MAX_YB_NUMS];
					pEqu->event_Item   = new SETTING_ITEM1[MAX_EVENT_NUMS];

					
   
					QHash<QString, int> hash;

					//装载事件点表
					sprintf(desc,"开始装载事件信息,请稍侯...,%s",pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);
					pEqu->event_Count = 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_EVENT,false,pCmd->sclPath.ied_name,pEqu->event_Item,(t_uint32 *)&pEqu->event_Count,MAX_EVENT_NUMS,hash,NULL);
					sprintf(desc,"装载事件点%d条,%s",pEqu->event_Count,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);



					//装载压板点表
					/*sprintf(desc,"开始装载压板信息,请稍侯...,%s",pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);
					pEqu->yb_Count = 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pCmd->sclPath.ied_name,pEqu->yb_Item,(t_uint32 *)&pEqu->yb_Count,MAX_YB_NUMS,hash,NULL);
					sprintf(desc,"装载告压板点%d条,%s",pEqu->yb_Count,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);	
					pEqu->bInitModel = true;*/
				}
			}

		}

		if (m_sclFlag == false) 
		{
			 //  调取模型
		   pCmd->cmdCode = CMD_INIT_MODEL;
		   initModel(pCmd,false);
		   pCmd->result = CMD_RESULT_SUCCESS;

		}


	}
	else
	{
		sprintf(desc,"连接失败 servername:%s...",pCmd->serverName);
		pEqu->bConnect = false;
		pCmd->cmdCode = CMD_CONNECT;
		pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		
	}

	if (pEqu->bInitModel)
	{
		pCmd->cmdCode=CMD_INIT_MODEL;
		pCmd->result = CMD_RESULT_SUCCESS;
		sprintf(desc,"初始化数据成功,可以进行装置的常规操作:%s...",pCmd->serverName);
		resultCmd(pCmd,desc);
	}

	pCmd->cmdCode = cmdCode1;
	pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
	resultCmd(pCmd,desc);
	
	pEqu->bResetConnect = true;


	return pEqu->bConnect;

}

void  ClientCmdThread::modifyYb(PCMD_GEN pCmd)
{

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	t_bool bRet = true; 

	t_char desc[MAX_NAMELEN];

	pCmd->cmdCode =  CMD_MODIFY_YB;

	 pCmd->result = CMD_RESULT_BEGINPROCESS;
	 resultCmd(pCmd,"");

	 sprintf(desc,"%s,修改压板开始",pCmd->serverName);
	 pCmd->result = CMD_RESULT_HINT;
	 resultCmd(pCmd,desc);


	ITEM_PARAM    *yb_Item = new ITEM_PARAM[pEqu->edit_yb_count];
    t_ushort      yb_Count = pEqu->edit_yb_count;


	//仿真子站修改当前区定值，定值修改目标值正常
	for (t_uint16 i = 0 ; i < pEqu->edit_yb_count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->edit_yb_item[i];

		ITEM_PARAM    *pItem1 = &yb_Item[i];


		strcpy(pItem1->ref,pItem->ref);
		strcpy(pItem1->str_value,pItem->val_Str);

		t_bool bRet1 = true;

		for (int j = 0; j < 3; j++)
		{

			if (auto_test_modify_yb(pCmd->serverName,pItem1,1,255, desc) != 0)
			{
				bRet1 = false;
			}
			else
			{
				break;
			}

			switch(j)
			{
			case 0:
				mySleep(1000);
				break;
			case 1:
				mySleep(2000);
				break;
			case 2:
				mySleep(5000);
				break;
			}
		}

		if (bRet1 == false)
		{
			sprintf(desc,"%s,修改压板%s失败",pCmd->serverName,pItem1->ref);
			pCmd->result = CMD_RESULT_HINT;
			resultCmd(pCmd,desc);
			bRet = false;
		}

		mySleep(1000);

	}


	delete[] yb_Item;


	 if (bRet )
	{
		sprintf(desc,"%s,修改压板成功",pCmd->serverName);
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);
	}
	else
	{
		sprintf(desc,"%s,修改压板失败",pCmd->serverName);
		pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
	}



	pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
	resultCmd(pCmd,"");


}

void  ClientCmdThread::modifyParam(PCMD_GEN pCmd)
{

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	t_bool bRet = true; 

	t_char desc[MAX_NAMELEN];

	pCmd->cmdCode =  CMD_MODIFY_PARAM;

	 pCmd->result = CMD_RESULT_BEGINPROCESS;
	 resultCmd(pCmd,"");

	 sprintf(desc,"%s,修改装置参数开始",pCmd->serverName);
	 pCmd->result = CMD_RESULT_HINT;
	 resultCmd(pCmd,desc);


	ITEM_PARAM    *param_Item = new ITEM_PARAM[pEqu->edit_param_count];
    t_ushort      param_Count = pEqu->edit_param_count;


	//仿真子站修改当前区定值，定值修改目标值正常
	for (t_uint16 i = 0 ; i < pEqu->edit_param_count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->edit_param_item[i];

		ITEM_PARAM    *pItem1 = &param_Item[i];


		strcpy(pItem1->ref,pItem->ref);
		strcpy(pItem1->str_value,pItem->val_Str);

		t_bool bRet1 = true;

		for (int j = 0; j < 3; j++)
		{

			if (auto_test_modify_equip_param(pCmd->serverName,pItem1,1, desc) != 0)
			{
				bRet1 = false;
			}
			else
			{
				break;
			}

			switch(j)
			{
			case 0:
				mySleep(1000);
				break;
			case 1:
				mySleep(2000);
				break;
			case 2:
				mySleep(5000);
				break;
			}
		}

		if (bRet1 == false)
		{
			sprintf(desc,"%s,修改装置参数%s失败",pCmd->serverName,pItem1->ref);
			pCmd->result = CMD_RESULT_HINT;
			resultCmd(pCmd,desc);
			bRet = false;
		}

		mySleep(1000);

	}


	delete[] param_Item;


	 if (bRet )
	{
		sprintf(desc,"%s,修改装置参数成功",pCmd->serverName);
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);
	}
	else
	{
		sprintf(desc,"%s,修改装置参数失败",pCmd->serverName);
		pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
	}



	pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
	resultCmd(pCmd,"");



}


void  ClientCmdThread::modifySetting(PCMD_GEN pCmd)
{

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	t_bool bRet = true; 

	 t_char desc[MAX_NAMELEN];

	 pCmd->cmdCode =  CMD_MODIFY_SETTING;

	 pCmd->result = CMD_RESULT_BEGINPROCESS;
	 resultCmd(pCmd,"");

	 sprintf(desc,"%s,修改定值开始",pCmd->serverName);
	 pCmd->result = CMD_RESULT_HINT;
	 resultCmd(pCmd,desc);


	ITEM_PARAM    *setting_Item = new ITEM_PARAM[pEqu->edit_setting_count];
    t_ushort      setting_Count = pEqu->edit_setting_count;


	//仿真子站修改当前区定值，定值修改目标值正常
	for (t_uint16 i = 0 ; i < pEqu->edit_setting_count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->edit_setting_item[i];

		ITEM_PARAM    *pItem1 = &setting_Item[i];


		strcpy(pItem1->ref,pItem->ref);
		strcpy(pItem1->str_value,pItem->val_Str);

	}

	if (auto_test_modify_setting(pCmd->serverName,setting_Item,setting_Count,desc) != 0)
	{
		bRet = false;
	}

	

	delete[] setting_Item;


	if (bRet )
	{
		sprintf(desc,"%s,修改定值成功",pCmd->serverName);
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);
	}
	else
	{
		sprintf(desc,"%s,修改定值失败",pCmd->serverName);
		pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
	}



	pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
	resultCmd(pCmd,"");

}


t_bool  ClientCmdThread::haveDataSetType(PCMD_GEN pCmd,t_char *str,t_char *desc)
{

	switch(pCmd->cmdCode)
	{
        case CMD_INIT_SETTING:
		case CMD_BECKON_SETTING:
		case CMD_UPDATE_SETTING:
			{
				if (desc)
				{
					sprintf(desc,"定值");
				}
				
				if ( (str != NULL)  && (strstr(str, DS_SETTING) != NULL)   )
				{
					return true;
				}
			}
		break;
		case CMD_EQU_PARAM:
		case CMD_INIT_PARAM:
		case CMD_UPDATE_PARAM:
			{
				if (desc)
				{
					sprintf(desc,"装置参数");
				}
				
				if ( (str != NULL)  && (strstr(str, DS_EQU_PARAM) != NULL)   )
				{
					return true;
				}
			}
			break;
		case CMD_BECKON_SAMPLE:
		case CMD_INIT_SAMPLE:
		case CMD_UPDATE_SAMPLE:
		{
			if (desc)
			{
				sprintf(desc,"遥测值");
			}
			
			if ( (str != NULL)  && ((strstr(str, DS_RELAY_AIN) != NULL)  || (strstr(str, DS_AIN) != NULL)  || (strstr(str, DS_URCB_RELAY_AIN) != NULL )  || (strstr(str, DS_UBRCB_AIN) != NULL)  )  )
			{
				return true;
			}
			
		}
		break;
		case CMD_BECKON_YB:
		case CMD_INIT_YB:
		case CMD_UPDATE_YB:
		{
			if (desc)
			{
				sprintf(desc,"压板");
			}

			if ( (str != NULL)  && ((strstr(str, DS_RELAY_ENA) != NULL) ||  (strstr(str, DS_ENA) != NULL) ||  (strstr(str, DS_BRCB_REALY_ENA) != NULL)  )  )
			{
				return true;
			}
			

		}
		break;
		case CMD_BECKON_SWITCH:
		case CMD_INIT_SWITCH:
		case CMD_UPDATE_SWITCH:
		{
			if (desc)
			{
				sprintf(desc,"遥信");
			}

			if ( (str != NULL)  && ( (strstr(str, DS_RELAY_DIN) != NULL)  || (strstr(str, DS_DIN) != NULL) || (strstr(str, DS_BRCB_DIN) != NULL) || (strstr(str, DS_BRCB_RELAY_DIN) != NULL) ) )
			{
				return true;
			}

			
		}
		break;
		case CMD_BECKON_EVENT:
		case CMD_INIT_EVET:
		case CMD_UPDATE_EVENT:
		{
			if (desc)
			{
				sprintf(desc,"事件");
			}

			if ( (str != NULL)  && ( (strstr(str, DS_TRIP_INFO) != NULL)  || (strstr(str, DS_BRCB_TRIP_INFO) != NULL) ))
			{
				return true;
			}
			
		}
		break;
		case CMD_INIT_ALARM:
		case CMD_INIT_WARING:
		{
			if (desc)
			{
				sprintf(desc,"告警");
			}

			if ( (str != NULL)  && ( (strstr(str, DS_WARNING) != NULL) || (strstr(str, DS_ALARM) != NULL)   || (strstr(str, DS_BRCB_ALARM) != NULL)   || (strstr(str, DS_BRCB_WARNING) != NULL)  ) )
			{
				return true;
			}
			
		}
		break;

		case CMD_INIT_RCD:
		{
			if (desc)
			{
				sprintf(desc,"录波");
			}

			if ( (str != NULL)  && ( (strstr(str, DS_RELAYREC) != NULL)    || (strstr(str, DS_BRCB_REALY_RECV) != NULL)    ) )
			{
				return true;
			}
			
		}
		break;

		default:
		{
			if (desc)
			{
				sprintf(desc,"其他");
			}

			if ( (str != NULL)  && ( ( strstr(str, DS_RELAY_AIN) != NULL) 
				                       || (strstr(str, DS_RELAY_ENA) != NULL) 
									   || (strstr(str, DS_RELAY_DIN) != NULL) 
									   || (strstr(str, DS_TRIP_INFO) != NULL)
									   || (strstr(str, DS_SETTING) != NULL)
									   ) )
				return false;
			else
				return true;

			
		}
		break;
	}

	return false;
}

void ClientCmdThread::getFileName(PCMD_GEN pCmd,t_char *fileName)
{


	switch(pCmd->cmdCode)
	{
	   case CMD_INIT_SETTING:
	   case CMD_BECKON_SETTING:
	   case CMD_UPDATE_SETTING:
		{
			sprintf(fileName,"setting.xml");
		}
		break;
	   case CMD_INIT_SAMPLE:
		case CMD_BECKON_SAMPLE:
		case CMD_UPDATE_SAMPLE:
		{
			sprintf(fileName,"sample.xml");
		}
		break;
		case CMD_INIT_YB:
		case CMD_BECKON_YB:
		case CMD_UPDATE_YB:
		{
		    sprintf(fileName,"yb.xml");	
 
		}
		break;
		case CMD_INIT_SWITCH:
		case CMD_BECKON_SWITCH:
		case CMD_UPDATE_SWITCH:
		{
			 sprintf(fileName,"switch.xml");	
		}
		break;
		case CMD_INIT_EVET:
		case CMD_BECKON_EVENT:
		case CMD_UPDATE_EVENT:
		{
		   sprintf(fileName,"event.xml");	
		}
		break;
		default:
		{
			 sprintf(fileName,"otherInfo.xml");		
		}
		break;
	}

}

void   ClientCmdThread::setEquipInitFlag(PCMD_GEN pCmd,PEQU_PARAM pEqu,t_bool flag)
{

	switch(pCmd->cmdCode)
	{  
	    case CMD_INIT_MODEL:
		{
			pEqu->bInitModel = flag;
		}
		break;
		case CMD_INIT_SETTING:
		{
		  pEqu->bInitSetting = flag;
		}
		break;
		case CMD_BECKON_SAMPLE:
		{
			pEqu->bInitSample = flag;
		}
		break;
		case CMD_BECKON_YB:
		{
		    pEqu->bInit_Yb = flag;
		}
		break;
		case CMD_BECKON_SWITCH:
		{
			pEqu->bInitSwitch = flag;
		}
		break;
		case CMD_BECKON_EVENT:
		{
		   pEqu->bInit_Event = flag;
		}
		break;
		default:
		{
				
		}
		break;
	}

}




void   ClientCmdThread::getRealay(PCMD_GEN pCmd)
{

	t_char      desc[MAX_NAMELEN];
	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);

	if (!pEqu)
		return ;


	t_char hint[100];
	memset(hint,0x00,sizeof(hint));
	haveDataSetType(pCmd,NULL,hint);
	
	SETTING_ITEM1 *pItem = NULL;
	t_ushort       *pItemCount=NULL;
	t_bool		  bSv = false;
	t_uint8       attribId = 0;
	PIED          pIed =NULL;
	t_uint8       cmdCode1 = pCmd->cmdCode;
	QString       cmdName;
	t_uint16      chnNums = 0;
	t_bool        bCfgCheck = true;
	t_bool        bSclEqu = true;
	t_char         **pdataSet = NULL;
	t_int          dataSetNums = 0;
	
	//获得attribId
	getAttribId(pCmd->cmdCode,bSv,attribId);
	//获得scd的装置
	pIed = getScdIED(pEqu->equpName);
	if (pIed == NULL)
	{
		bSclEqu = false;

	}

	switch(pCmd->cmdCode)
	{

	case CMD_BECKON_SETTING:
		{
			cmdName = QString("定值");
			//召唤定值点表
			if (pEqu->bInitSetting == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != "") )
				{
					sprintf(desc," 正在从配置文件载入%s点表,请稍侯...%s",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					ClientDSE_ScdManager scd;

					if (strlen(pEqu->ptFileName) == 0)
					{
						int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
						int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
						QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);
						sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);

					}

					
					scd.read(pEqu->ptFileName);

					if (pEqu->setting_Item == NULL)
						pEqu->setting_Item = new SETTING_ITEM1[MAX_SETTING_NUMS];

					QHash<QString, int> hash;

					pItem = pEqu->setting_Item;
				    pItemCount = &pEqu->setting_Count;
					//装载定值点表
					*pItemCount= 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_SETTING,true,pEqu->sclPath.ied_name,pItem,(t_uint32*)pItemCount,MAX_SETTING_NUMS,hash,NULL);
					sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitSetting = true;

					//分配
					pEqu->setting_dataSet =  (char **)calloc(hash.count(), sizeof(char *)); 
					pEqu->setting_dataSet_nums = hash.count();

					for (int i = 0; i < pEqu->setting_dataSet_nums; i++)
					{
						 pEqu->setting_dataSet[i] = (char *)calloc(65,sizeof(char));
					}

					QHashIterator<QString, int> dataSetIter(hash);
					int i = 0;
					 while (dataSetIter.hasNext()) 
					 {
						 dataSetIter.next();
						
						 QString dataSet = dataSetIter.key();
						 strcpy(pEqu->setting_dataSet[i++],dataSet.toAscii().data());
					 }

					 hash.clear();

					 pdataSet = pEqu->setting_dataSet;
	                 dataSetNums = pEqu->setting_dataSet_nums;

				}
				else
				{
					sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
					
					pCmd->cmdCode = CMD_INIT_SETTING;
					pEqu->setting_Count = 0;
					get_dyn_pt(pCmd->serverName,ITI_SETTING,&pEqu->setting_Item,&pEqu->setting_Count, NULL);
					pItem = pEqu->setting_Item;
				    pItemCount = &pEqu->setting_Count;
				

					sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					pEqu->bInitSetting = true;
					pCmd->cmdCode = tmpCmdCode;
				

				}
			}
			else
			{
				pItem = pEqu->setting_Item;
				pItemCount = &pEqu->setting_Count;
				pdataSet = pEqu->setting_dataSet;
	            dataSetNums = pEqu->setting_dataSet_nums;
			}

			pEqu->isBeckonSetting = true;

		}
		break;
	case CMD_EQU_PARAM:
		{
			cmdName = QString("装置参数");
			//召唤定值点表
			if (pEqu->bInitParam == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != "") )
				{
					sprintf(desc," 正在从配置文件载入%s点表,请稍侯...%s",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					if (strlen(pEqu->ptFileName) == 0)
					{
						int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
						int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
						QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);
						sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);

					}

					ClientDSE_ScdManager scd;

					if (pEqu->param_Item == NULL)
						pEqu->param_Item = new SETTING_ITEM1[MAX_PARAM_NUMS];

					QHash<QString, int> hash;

				    pItem = pEqu->param_Item;
				    pItemCount = &pEqu->paramCount;
					*pItemCount= 0;
					scd.read(pEqu->ptFileName);
					scd.read_event(pEqu->ptFileName,ATTRIB_EQU_PARAM,true,pEqu->sclPath.ied_name,pItem,(t_uint32*)pItemCount,MAX_PARAM_NUMS,hash,NULL);
					sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitParam = true;

					//分配
					pEqu->param_dataSet =  (char **)calloc(hash.count(), sizeof(char *)); 
					pEqu->param_dataSet_nums = hash.count();

					for (int i = 0; i < pEqu->param_dataSet_nums; i++)
					{
						 pEqu->param_dataSet[i] = (char *)calloc(65,sizeof(char));
					}

					QHashIterator<QString, int> dataSetIter(hash);
					int i = 0;
					 while (dataSetIter.hasNext()) 
					 {
						 dataSetIter.next();
						
						 QString dataSet = dataSetIter.key();
						 strcpy(pEqu->param_dataSet[i++],dataSet.toAscii().data());
					 }

					 hash.clear();

					pdataSet    = pEqu->param_dataSet;
	                dataSetNums = pEqu->param_dataSet_nums;


				}
				else
				{		
					sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
					
					pCmd->cmdCode = CMD_INIT_PARAM;
					pEqu->paramCount= 0;
					get_dyn_pt(pCmd->serverName,ITI_EQUP_PARAM,&pEqu->param_Item,&pEqu->paramCount, NULL);
					pItem = pEqu->param_Item;
					pItemCount = &pEqu->paramCount;
				

					sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
					pEqu->bInitParam = true;
					pCmd->cmdCode = tmpCmdCode;
				}
			}
			else
			{
				pItem = pEqu->param_Item;
				pItemCount = &pEqu->paramCount;
				pdataSet    = pEqu->param_dataSet;
	            dataSetNums = pEqu->param_dataSet_nums;

			}

			pEqu->isBeckonParam = true;
		}
		break;
	case CMD_BECKON_YB:
		{
			cmdName = QString("软压板");

			if (pEqu->bInit_Yb == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != ""))
				{
					sprintf(desc," 正在从配置文件载入%s点表,请稍侯...%s",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					if (strlen(pEqu->ptFileName) == 0)
					{
						int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
						int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
						QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);
						sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);

					}

					ClientDSE_ScdManager scd;
					scd.read(pEqu->ptFileName);

					if (pEqu->yb_Item == NULL)
						pEqu->yb_Item = new SETTING_ITEM1[MAX_YB_NUMS];

					QHash<QString, int> hash;

					pItem = pEqu->yb_Item;
					pItemCount = &pEqu->yb_Count;
					*pItemCount = 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pEqu->sclPath.ied_name,pItem,(t_uint32*)pItemCount,MAX_YB_NUMS,hash,NULL);
					sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInit_Yb = true;

					//分配
					pEqu->ybdataSet =  (char **)calloc(hash.count(), sizeof(char *)); 
					pEqu->yb_dataSet_nums = hash.count();

					for (int i = 0; i < pEqu->yb_dataSet_nums; i++)
					{
						 pEqu->ybdataSet[i] = (char *)calloc(65,sizeof(char));
					}

					QHashIterator<QString, int> dataSetIter(hash);
					int i = 0;
					 while (dataSetIter.hasNext()) 
					 {
						 dataSetIter.next();
						
						 QString dataSet = dataSetIter.key();
						 strcpy(pEqu->ybdataSet[i++],dataSet.toAscii().data());
					 }

					 hash.clear();

					pdataSet    = pEqu->ybdataSet;
	                dataSetNums = pEqu->yb_dataSet_nums;

				}
				else
				{
					sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
				
					pCmd->cmdCode = CMD_INIT_YB;
					pEqu->yb_Count= 0;
					get_dyn_pt(pCmd->serverName,ITI_YB,&pEqu->yb_Item,&pEqu->yb_Count, NULL);
					pItem = pEqu->yb_Item;
					pItemCount = &pEqu->yb_Count;
					

					sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
					pEqu->bInit_Yb = true;
					pCmd->cmdCode = tmpCmdCode;

				}	
			}
			else
			{
				pItem = pEqu->yb_Item;
				pItemCount = &pEqu->yb_Count;
				pdataSet    = pEqu->ybdataSet;
	            dataSetNums = pEqu->yb_dataSet_nums;

			}
			pEqu->isBeckonYb = true;
		}
		break;
	    case CMD_BECKON_SAMPLE:
		{
			cmdName = QString("遥测值");

			if (pEqu->bInitSample == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != ""))
				{
					sprintf(desc," 正在从配置文件载入%s点表,请稍侯...%s",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					if (strlen(pEqu->ptFileName) == 0)
					{
						int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
						int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
						QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);
						sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);

					}

					ClientDSE_ScdManager scd;

					if (pEqu->sample_Item == NULL)
						pEqu->sample_Item = new SETTING_ITEM1[MAX_SAMPLE_NUMS];

					
					QHash<QString, int> hash;

					pItem = pEqu->sample_Item;
					pItemCount = &pEqu->sample_Count;
					*pItemCount= 0;
					scd.read(pEqu->ptFileName);
					scd.read_event(pEqu->ptFileName,ATTRIB_SAMPLE,true,pEqu->sclPath.ied_name,pItem,(t_uint32*)pItemCount,MAX_SAMPLE_NUMS,hash,NULL);
					sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitSample = true;

					//分配
					pEqu->sampledataSet =  (char **)calloc(hash.count(), sizeof(char *)); 
					pEqu->sample_dataSet_nums = hash.count();

					for (int i = 0; i < pEqu->sample_dataSet_nums; i++)
					{
						 pEqu->sampledataSet[i] = (char *)calloc(65,sizeof(char));
					}

					QHashIterator<QString, int> dataSetIter(hash);
					int i = 0;
					 while (dataSetIter.hasNext()) 
					 {
						 dataSetIter.next();
						
						 QString dataSet = dataSetIter.key();
						 strcpy(pEqu->sampledataSet[i++],dataSet.toAscii().data());
					 }

					 hash.clear();

					pdataSet   = pEqu->sampledataSet;
	                dataSetNums = pEqu->sample_dataSet_nums;

				}
				else
				{
					sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
				
					pCmd->cmdCode = CMD_INIT_SAMPLE;
					pEqu->sample_Count= 0;
					get_dyn_pt(pCmd->serverName,ITI_TELEMETRY,&pEqu->sample_Item,&pEqu->sample_Count, NULL);
					pItem = pEqu->sample_Item;
					pItemCount = &pEqu->sample_Count;
					
					sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
					pEqu->bInitSample = true;
					pCmd->cmdCode = tmpCmdCode;
				}
				
			}
			else
			{
				pItem = pEqu->sample_Item;
				pItemCount = &pEqu->sample_Count;
				pdataSet   = pEqu->sampledataSet;
                dataSetNums = pEqu->sample_dataSet_nums;
			}
	
		}
		break;
	case CMD_BECKON_SWITCH:
		{
			cmdName = QString("遥信");
			if (pEqu->bInitSwitch == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != ""))
				{
					sprintf(desc," 正在从配置文件载入%s点表,请稍侯...%s",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					if (strlen(pEqu->ptFileName) == 0)
					{
						int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
						int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
						QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);
						sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);

					}

					ClientDSE_ScdManager scd;
					scd.read(pEqu->ptFileName);

					if (pEqu->swi_Item == NULL)
						pEqu->swi_Item = new SETTING_ITEM1[MAX_SWI_NUMS];

					QHash<QString, int> hash;

					pItem = pEqu->swi_Item;
					pItemCount = &pEqu->swi_Count;
					*pItemCount= 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,pItem,(t_uint32*)pItemCount,MAX_SWI_NUMS,hash,NULL);
					sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitSwitch = true;

					//分配
					pEqu->swidataSet =  (char **)calloc(hash.count(), sizeof(char *)); 
					pEqu->swi_dataSet_nums = hash.count();

					for (int i = 0; i < pEqu->swi_dataSet_nums; i++)
					{
						 pEqu->swidataSet[i] = (char *)calloc(65,sizeof(char));
					}

					QHashIterator<QString, int> dataSetIter(hash);
					int i = 0;
					 while (dataSetIter.hasNext()) 
					 {
						 dataSetIter.next();
						
						 QString dataSet = dataSetIter.key();
						 strcpy(pEqu->swidataSet[i++],dataSet.toAscii().data());
					 }

					 hash.clear();

					 pdataSet   = pEqu->swidataSet;
	                dataSetNums = pEqu->swi_dataSet_nums;

				}
				else
				{
					sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
				
					pCmd->cmdCode = CMD_INIT_SWITCH;
					pEqu->swi_Count= 0;
					get_dyn_pt(pCmd->serverName,ITI_SWI,&pEqu->swi_Item,&pEqu->swi_Count, NULL);
					pItem = pEqu->swi_Item;
					pItemCount = &pEqu->swi_Count;
					

					sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
					pEqu->bInitSwitch = true;
					pCmd->cmdCode = tmpCmdCode;

				}
			}
			else
			{
				pItem = pEqu->swi_Item;
				pItemCount = &pEqu->swi_Count;
				 pdataSet   = pEqu->swidataSet;
                dataSetNums = pEqu->swi_dataSet_nums;
			}
		}
	
	default:
		break;


	}


	QString str = QString("正在获取%1,请稍侯...").arg(hint);
	pCmd->result = CMD_RESULT_RESULT_FAILD;
	resultCmd(pCmd,str.toAscii().data());

	t_short scdErrNums = 0;


	//分配字符串数组
	char **strRef,**strVal;

	strRef = (char **)calloc( *pItemCount, sizeof(char *));
	strVal = (char **)calloc( *pItemCount, sizeof(char *));
	for (int i = 0; i <  *pItemCount; i++)
	{
		 strRef[i] = (char *)calloc(65,sizeof(char));
		 strVal[i] = (char *)calloc(65,sizeof(char));
		 memset(strVal[i],0,65*sizeof(char));

		 strcpy(strRef[i],pItem[i].ref);

	}


	//取远方的数据集获得值
	if (auto_test_read_dataSet_value(pCmd->serverName,pdataSet,dataSetNums,strRef,*pItemCount,strVal,NULL) != 0)
		{	 
			 sprintf(desc,"%s,获取%s信息失败",pCmd->serverName,pdataSet[0]);
			 pCmd->result = CMD_RESULT_RESULT_FAILD;
			 resultCmd(pCmd,desc);
		}
	else
	{
		for (int i = 0  ; i < *pItemCount;i++)
		{
			SETTING_ITEM1 &item = pItem[i];

			strcpy(item.val_Str,strVal[i]);
		}
	}

	/*for (int i = 0  ; i < *pItemCount;i++)
	{
		SETTING_ITEM1 &item = pItem[i];
	
		if (auto_test_read_value(pCmd->serverName,item.ref,item.val_Str, NULL))
		{	 
			 sprintf(desc,"%s,获取%s信息失败",pCmd->serverName,item.ref);
			 pCmd->result = CMD_RESULT_RESULT_FAILD;
			 resultCmd(pCmd,desc);
		}

	}*/

	//释放字符串数组
	for (int i = 0; i <  *pItemCount; i++)
    {
        free(strRef[i]);
		free(strVal[i]);
    }
	free(strRef);
	free(strVal);
	
	pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
	resultCmd(pCmd,"");
	

	str = QString("结束获取%1...").arg(hint);
	pCmd->result = CMD_RESULT_SUCCESS;
	resultCmd(pCmd,str.toAscii().data());

}

t_short ClientCmdThread::getUpdateCode(PCMD_GEN pCmd)
{

	switch(pCmd->cmdCode)
	{

	case CMD_INIT_SETTING:
	case CMD_BECKON_SETTING:
		return CMD_UPDATE_SETTING;
		break;
	case CMD_INIT_PARAM:
	case CMD_EQU_PARAM:
		return CMD_UPDATE_PARAM;
		break;
	case CMD_INIT_SAMPLE:
	case CMD_BECKON_SAMPLE:
		return CMD_UPDATE_SAMPLE;
		break;
	case CMD_INIT_YB:
	case CMD_BECKON_YB:
		return CMD_UPDATE_YB;
		break;
	case CMD_INIT_SWITCH:
	case CMD_BECKON_SWITCH:
		return CMD_UPDATE_SWITCH;
		break;
	case CMD_INIT_EVET:
	case CMD_BECKON_EVENT:
		return CMD_UPDATE_EVENT;
		break;
	case CMD_INIT_OTHER:
		return CMD_UPDATE_OTHER;
		break; 
	}


	return 0;
}

unsigned int  ClientCmdThread::get_dyn_pt(t_char *serverName,unsigned char info_type,SETTING_ITEM1 **pt,unsigned short *pt_nums, char* err)
{
	ITEM_PARAM *param = NULL;
	unsigned short nums = 0;

	if ( (auto_test_dyn_pt(serverName,info_type,&param,&nums,err  ) == 0) && (nums >0))
	{
		
		*pt = new SETTING_ITEM1[nums];

		for (int i = 0 ; i < nums;i++)
		{
			SETTING_ITEM1 *ptr = &(*pt)[i];
			strcpy(ptr->ref,param[i].ref);
			strcpy(ptr->desc,param[i].desc);
			//QString strGbk = QString::fromUtf8(param[i].desc,strlen(param[i].desc));
			//strcpy(ptr->desc,strGbk.toAscii().data());
			strcpy(ptr->max_val_str,param[i].max_val_str);
			strcpy(ptr->min_val_str,param[i].min_val_str);
			strcpy(ptr->step_str,param[i].step_str);
			strcpy(ptr->unit,param[i].unit);

		}

		*pt_nums = nums;

		auto_test_free_ptr(param);
	}
	else
	{
		return 1;
	}

	return 0;

}
void   ClientCmdThread::initModel(PCMD_GEN pCmd,t_bool bFlag)
{

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);

	if (!pEqu)
		return ;

	pEqu->bInitModel = false;
	QStringList strList;
	
	t_char desc[MAX_NAMELEN];
	sprintf(desc,"开始获取服务器的目录信息并缓存在本地内存数据库,请稍侯....server%s",pCmd->serverName);
	pCmd->result = CMD_RESULT_BEGINPROCESS;
	resultCmd(pCmd,desc);

	if (auto_test_dyn_scl_model(pCmd->serverName,NULL) == 0)
	{
		sprintf(desc,"获取远程装置模型成功,server:%s",pCmd->serverName);
		pCmd->result = CMD_RESULT_BEGINPROCESS;
		resultCmd(pCmd,desc);

	}

	pEqu->bInitModel = true;

	if (m_winFlag)
	{
		t_char err[256];

/*
		sprintf(desc,"开始动态召唤告警配置信息,请稍侯...,%s",pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
	
		if (get_dyn_pt(pCmd->serverName,ITI_ALARM,&pEqu->alarm_Item,(unsigned short*)&pEqu->alarm_Count,err) != 0)
		{
			sprintf(desc,"动态召唤告警配置信息失败,%s",pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);
		}
		else
		{
			sprintf(desc,"动态召唤到告警配置信息%d条,%s",pEqu->alarm_Count,pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);	
		}
*/
		sprintf(desc,"开始动态召唤事件配置信息,请稍侯...,%s",pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->event_Count = 0;
		if (get_dyn_pt(pCmd->serverName,ITI_EVENT,&pEqu->event_Item,(unsigned short*)&pEqu->event_Count,err) != 0)
		{
			sprintf(desc,"动态召唤事件配置信息失败,%s",pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);
		}
		else
		{
			sprintf(desc,"动态召唤到事件配置信息%d条,%s",pEqu->event_Count,pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);	
		}

		sprintf(desc,"开始动态召唤压板配置信息,请稍侯...,%s",pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->yb_Count = 0;
		if (get_dyn_pt(pCmd->serverName,ITI_YB,&pEqu->yb_Item,(unsigned short*)&pEqu->yb_Count,err) != 0)
		{
			printf(desc,"动态召唤压板配置信息失败,%s",pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);
		}
		else
		{
			sprintf(desc,"动态召唤到压板配置信息%d条,%s",pEqu->yb_Count,pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);	
		}	
	}

}


void  ClientCmdThread::decodeScl(PCMD_GEN pCmd)
{

	t_char fileName[MAX_NAMELEN];

	sprintf(fileName,"%s",pCmd->data);

	ClientDSE_ScdManager scd;
	
	t_char desc[MAX_NAMELEN];

	t_bool bRet = true,bRet1 = true;

	sprintf(desc,"开始解析%s文件...",fileName);
	pCmd->result = CMD_RESULT_BEGINPROCESS;
	resultCmd(pCmd,desc);

	QString fileName1;
	t_uint8 tmpType ;

	int index = QString(fileName).lastIndexOf(QString("/"));
	int index1 = QString(fileName).lastIndexOf(".");
	QString fileName2   =   QString(fileName).mid(index+1, index1-index-1);	

	if (pCmd->cmdCode == CMD_DESCODE_PT_SCL)
	{
		tmpType = DECODE_COMMLAYER;
		fileName1= tr("%1/%2_%3")
		            .arg(m_cfgDir)
					.arg(fileName2)
					.arg(QString(EQPPTCFG));
	}
	else if (pCmd->cmdCode == CMD_DESCODE_IP_SCL)
	{
		tmpType = DECODE_COMMLAYER;
		fileName1= tr("%1/%2_%3")
		            .arg(m_cfgDir)
					.arg(fileName2)
					.arg(QString(EQPIPCFG));
	}
	else
	{
		return ;
	}

	m_curDecodeSclType = tmpType;

	if (scd.readScd(fileName,progress_Hint,this,tmpType) != 0)
	{	
		bRet = false;
	}



	if (scd.write(fileName1.toAscii().data(),NULL) != 0)
	{
		bRet1 = false;
	}

	if (bRet && bRet1)
	{
		sprintf(desc,"解析%s文件成功...",fileName);
		pCmd->result = CMD_RESULT_SUCCESS;
	}
	else
	{
		sprintf(desc,"解析%s文件失败...",fileName);
		pCmd->result = CMD_RESULT_RESULT_FAILD;
	}

	sprintf(pCmd->data,"%s",pCmd->data);
	resultCmd(pCmd,desc);



	


}

t_int    ClientCmdThread:: progress_Hint(void* ptr, CMD_TYPE_DEF::CMD cmd)
{

	ClientCmdThread *pThis = (ClientCmdThread*)ptr;

	if (pThis == NULL)
		return -1 ;

		
	CMD_GEN *pCmd1 = new CMD_GEN();

	if (pThis->getCurDecodeSclType() == DECODE_COMMLAYER)
	{
	  pCmd1->cmdCode = CMD_PROGRESS_IP_HINT;
	}
	else/* if (pThis->getCurDecodeSclType() == DECODE_PT)*/
	{
	  pCmd1->cmdCode = CMD_PROGRESS_PT_HINT;
	}

	memcpy(pCmd1->data,cmd.data.str,1024);

	//发送返回信号
	emit pThis->resultData_signals(pCmd1);

	return  0;

}

t_bool ClientCmdThread::getPointDesc(t_char *serverName,t_char *ref,t_bool bBoolFlag,t_char *desc,t_char *state)
{
	PEQU_PARAM pEqu = getEquipWithAppend(serverName); 

	if (!pEqu)
		return false;


	for (int i = 0 ; i < pEqu->yb_Count ; i++)
	{
		SETTING_ITEM1 &item = pEqu->yb_Item[i];

		if (QString(item.ref ) == QString(ref))
		{
			sprintf(desc,"%s",item.desc);
		
			if (bBoolFlag)
			{
				if (atoi(state)>0)
					sprintf(state,"投入");
				else
					sprintf(state,"退出");
			}

			return true; 
		}
	}

	for (int i = 0 ; i < pEqu->swi_Count ; i++)
	{
		SETTING_ITEM1 &item = pEqu->swi_Item[i];

		if (QString(item.ref ) == QString(ref))
		{
			sprintf(desc,"%s",item.desc);
			if (bBoolFlag)
			{
				if (atoi(state)>0)
					sprintf(state,"合");
				else
					sprintf(state,"分");
			}
			return true; 
		}
	}

	
	for (int i = 0 ; i < pEqu->event_Count ; i++)
	{
		SETTING_ITEM1 &item = pEqu->event_Item[i];

		if (QString(item.ref ) == QString(ref))
		{
			sprintf(desc,"%s",item.desc);
			if (bBoolFlag)
			{
				if (atoi(state)>0)
					sprintf(state,"动作");
				else
					sprintf(state,"退出");
			}
			return true; 
		}
	}

	

	return false;
}

void  ClientCmdThread::resultCmd(PCMD_GEN pCmd,t_char *desc)
{


	CMD_GEN *pCmd1 = new CMD_GEN();

	memcpy(pCmd1,pCmd,sizeof(CMD_GEN));
	sprintf(pCmd1->resultDesc,"%s",desc);


	//发送返回信号
	emit resultData_signals(pCmd1);


}

void  ClientCmdThread::readScdCfg()
{

	if (m_bReadScd == false)
	{
		
		QString	fileName1= tr("%1/%2")  .arg(m_cfgDir).arg(QString(EQPPTCFG));

		m_scd.read(fileName1.toAscii().data());

		m_bReadScd = true;
	}
	
}

PIED ClientCmdThread::getScdIED(QString equName)
{

	for (t_uint8 subIndex = 0 ; subIndex < m_scd.m_subNetList.size() ; subIndex++)
	{
		PSUBNET pSubNet = m_scd.m_subNetList[subIndex];

		for (t_uint16 iedIndex = 0 ; iedIndex < pSubNet->iedList.size(); iedIndex++)
		{
			PIED pIed = pSubNet->iedList[iedIndex];

			if (QString(pIed->name) == equName)
			{
				return pIed;
			}
		}
	}

	return NULL;

}



t_uint16 ClientCmdThread::getChnNum(PIED pIed,t_bool bSv,t_uint8 attribId)
{

	t_uint16 chnNums = 0;

	if (bSv)    
	{
		for (t_uint8 svIndex = 0 ; svIndex < pIed->svList.size(); svIndex++)
		{
			SVINFO *pSvInfo = pIed->svList[svIndex];

			if (pSvInfo->attribId != attribId)
			   continue;

			chnNums +=pSvInfo-> datCount;
		
		}
	}
	else
	{
		for (t_uint8 gsIndex = 0 ; gsIndex < pIed->gsList.size(); gsIndex++)
		{
			GOOSEINFO *pGsInfo = pIed->gsList[gsIndex];

			if (pGsInfo->attribId != attribId)
			   continue;

			chnNums +=pGsInfo-> datCount;
		}

	}

	return chnNums;

}



t_bool  ClientCmdThread::getAttribId(t_uint8 cmdCode,t_bool &bSv,t_uint8 &attribId)
{
   t_bool  bRet = true;

	switch(cmdCode)
	{
	case CMD_BECKON_SETTING:
	case CMD_INIT_SETTING:
		attribId = ATTRIB_SETTING;
		bSv = true;
		break;
	case CMD_EQU_PARAM:
	case CMD_INIT_PARAM:
		attribId = ATTRIB_EQU_PARAM;
		bSv = true;
		break;
	case CMD_BECKON_SAMPLE:
	case CMD_INIT_SAMPLE:
		attribId = ATTRIB_SAMPLE;
		bSv = true;
		break;
	case CMD_BECKON_YB:
	case CMD_INIT_YB:
		attribId = ATTRIB_YB;
		bSv = false;
		break;
	case CMD_BECKON_SWITCH:
	case CMD_INIT_SWITCH:
		attribId = ATTRIB_SWI;
		bSv = false;
		break;
	case CMD_INIT_EVET:
		attribId = ATTRIB_EVENT;
		bSv = false;
		break;
	case CMD_INIT_ALARM:
	case CMD_INIT_WARING:
		attribId = ATTRIB_ALARM;
		bSv = false;
		break;
	default:
		bRet = false;
		break;

	}

	return bRet;

}


void  ClientCmdThread::initModelCheckVars()
{

   m_settingCount = 0;                 //定值个数不相等
   m_settingRefCount = 0;
   m_settingDescCount= 0;             //定值描述符不相等
   m_settingMaxCount= 0;              //定值描最大值不相等
   m_settingMinCount= 0;              //定值描最小值不相等
   m_settingStepCount= 0;             //定值步长不相等
   m_settingUnitCount= 0;             //定值单位不相符

   m_equipParamCount= 0;               //装置参数个数不相等
   m_equipParamRefCount = 0;
   m_equipParamDescCount= 0;           //装置参数描述符不相等
   m_equipParamMaxCount= 0;            //装置参数最大值不相等
   m_equipParamMinCount= 0;            //装置参数最小值不相等
   m_equipParamStepCount= 0;           //装置参数步长不相等
   m_equipParamUnitCount= 0;           //装置参数不相符

   m_analogCount= 0;                    //遥测值个数不相等
   m_analogRefCount = 0;
   m_analogDescCount= 0;                //遥测值描述符不相等
   m_analogMaxCount= 0;                 //遥测值最大值不相等
   m_analogMinCount= 0;                 //遥测值最小值不相等
   m_analogStepCount= 0;                //遥测值步长不相等
   m_analogUnitCount= 0;                //遥测值不相符

   m_ybCount= 0;                    //遥测值个数不相等
   m_ybRefCount = 0;
   m_ybDescCount= 0;                //遥测值描述符不相等

   m_swiCount= 0;                    //遥测值个数不相等
   m_swiRefCount = 0;
   m_swiDescCount= 0;                //遥测值描述符不相等

   m_alarmCount= 0;                    //遥测值个数不相等
   m_alarmRefCount = 0;
   m_alarmDescCount= 0;                //遥测值描述符不相等

   m_eventCount= 0;                    //遥测值个数不相等
   m_eventRefCount = 0;
   m_eventDescCount= 0;                //遥测值描述符不相等

}


void  ClientCmdThread::modelCheck(PCMD_GEN pCmd)
{
	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	t_char desc[MAX_NAMELEN];

	t_short cmdCode1 = pCmd->cmdCode; 
	QString cmdName;

	pCmd->cmdCode = CMD_CONNECT;

	////连接
	//if (connectServer(pCmd) == false)
	//	return ;

	pCmd->cmdCode = CMD_SCD_CHECK;

	ClientDSE_ScdManager scd;

	if (strlen(pEqu->ptFileName) == 0)
	{
		if (QString(pCmd->sclPath.scl_file_name) == "")
		{
			sprintf(desc,"没有比较的SCL文件");
    		pCmd->result = CMD_RESULT_SUCCESS;
			resultCmd(pCmd,desc);
			return ;
		}

		strcpy(pEqu->sclPath.scl_file_name,pCmd->sclPath.scl_file_name);
		strcpy(pEqu->sclPath.ied_name,pCmd->sclPath.ied_name);
		strcpy(pEqu->sclPath.access_point_name,pCmd->sclPath.access_point_name);
		pEqu->sclPath.enabled = pCmd->sclPath.enabled;
			
		int index = QString(pCmd->sclPath.scl_file_name).lastIndexOf("/");
		int index1 = QString(pCmd->sclPath.scl_file_name).lastIndexOf(".");
		QString fileName2   =  QString(pCmd->sclPath.scl_file_name).mid(index+1,index1-index-1);	

		sprintf(pEqu->ptFileName,"%s/%s_%s",m_cfgDir,fileName2.toAscii().data(),EQPPTCFG);
	}
	
	if (scd.read(pEqu->ptFileName) != 0)
	{
		sprintf(desc,"装载%s文件失败,%s",pEqu->ptFileName,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		return ;

	}

	pCmd->cmdCode = cmdCode1;


	initModelCheckVars();

	//获得文件点表
	if (pEqu->bInitSetting == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("定值");
		if  (pEqu->setting_Item == NULL)
			pEqu->setting_Item = new SETTING_ITEM1[MAX_SETTING_NUMS];

		scd.read_event(pEqu->ptFileName,ATTRIB_SETTING,true,pEqu->sclPath.ied_name,pEqu->setting_Item,(t_uint32*)&pEqu->setting_Count,MAX_SETTING_NUMS,hash,NULL);
		sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),pEqu->setting_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitSetting = true;
		hash.clear();

	}

	if (pEqu->bInitParam == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("装置参数");
		if  (pEqu->param_Item == NULL)
			pEqu->param_Item = new SETTING_ITEM1[MAX_PARAM_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_EQU_PARAM,true,pEqu->sclPath.ied_name,pEqu->param_Item,(t_uint32*)&pEqu->paramCount,MAX_PARAM_NUMS,hash,NULL);
		sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),pEqu->paramCount,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitParam = true;
		hash.clear();

	}

	if (pEqu->bInit_Yb == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("压板");
		if  (pEqu->yb_Item == NULL)
			pEqu->yb_Item = new SETTING_ITEM1[MAX_YB_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pEqu->sclPath.ied_name,pEqu->yb_Item,(t_uint32*)&pEqu->yb_Count,MAX_YB_NUMS,hash,NULL);
		sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),pEqu->yb_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInit_Yb = true;
		hash.clear();

	}

	if (pEqu->bInitSample == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("遥测值");
		if  (pEqu->sample_Item == NULL)
			pEqu->sample_Item = new SETTING_ITEM1[MAX_SAMPLE_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_SAMPLE,true,pEqu->sclPath.ied_name,pEqu->sample_Item,(t_uint32*)&pEqu->sample_Count,MAX_SAMPLE_NUMS,hash,NULL);
		sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),pEqu->sample_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitSample = true;
		hash.clear();

	}

	if (pEqu->bInitSwitch == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("遥信");
		if (pEqu->swi_Item == NULL)
			pEqu->swi_Item = new SETTING_ITEM1[MAX_SWI_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,pEqu->swi_Item,(t_uint32*)&pEqu->swi_Count,MAX_SWI_NUMS,hash,NULL);
		sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),pEqu->swi_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitSwitch = true;
		hash.clear();

	}

	if (pEqu->bInit_Event == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("事件");
		if (pEqu->event_Item == NULL)
			pEqu->event_Item = new SETTING_ITEM1[MAX_EVENT_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,pEqu->event_Item,(t_uint32*)&pEqu->event_Count,MAX_EVENT_NUMS,hash,NULL);
		sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),pEqu->event_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInit_Event = true;
		hash.clear();

	}



	//这里分配改成了动态库的
    SETTING_ITEM1 *scl_item_setting = new SETTING_ITEM1[MAX_SETTING_NUMS];
	t_ushort      scl_item_nums_setting = 0;
	cmdName = QString("定值");
	compSetting(pCmd,scd,cmdName,scl_item_setting,scl_item_nums_setting);
	delete[] scl_item_setting;


	SETTING_ITEM1 *scl_item_param   = new SETTING_ITEM1[MAX_PARAM_NUMS];
	t_ushort      scl_item_nums_param = 0; 
	cmdName = QString("装置参数");
	compEquipParam(pCmd,scd,cmdName,scl_item_param,scl_item_nums_param);
	delete[] scl_item_param;


   SETTING_ITEM1 *scl_item_analog   = new SETTING_ITEM1[MAX_SAMPLE_NUMS];
	t_ushort      scl_item_nums_analog = 0; 
	cmdName = QString("遥测值");
	compAnalog(pCmd,scd,cmdName,scl_item_analog,scl_item_nums_analog);
	delete[] scl_item_analog;

	//
    SETTING_ITEM1 *scl_item_yb   = new SETTING_ITEM1[MAX_YB_NUMS];
	t_ushort      scl_item_nums_yb = 0; 
	cmdName = QString("压板");
	compYb(pCmd,scd,cmdName,scl_item_yb,scl_item_nums_yb);	
	delete[] scl_item_yb;

    SETTING_ITEM1 *scl_item_swi   = new SETTING_ITEM1[MAX_SWI_NUMS];
	t_ushort      scl_item_nums_swi = 0; 
	cmdName = QString("遥信");
	compSwi(pCmd,scd,cmdName,scl_item_swi,scl_item_nums_swi);	
	delete[] scl_item_swi;


    SETTING_ITEM1 *scl_item_event   = new SETTING_ITEM1[MAX_EVENT_NUMS];
	t_ushort      scl_item_nums_event = 0; 
	cmdName = QString("事件");
	compEvent(pCmd,scd,cmdName,scl_item_event,scl_item_nums_event);
	delete[] scl_item_event;
	


	//错误统计
	compErrCount(pCmd,true);

	initModelCheckVars();

	/*compSetting1(pCmd,scd,cmdName,scl_item_setting,scl_item_nums_setting);
	compEquipParam1(pCmd,scd,cmdName,scl_item_param,scl_item_nums_param);
	compAnalog1(pCmd,scd,cmdName,scl_item_analog,scl_item_nums_analog);	
	compYb1(pCmd,scd,cmdName,scl_item_yb,scl_item_nums_yb);	
	compSwi1(pCmd,scd,cmdName,scl_item_swi,scl_item_nums_swi);	
	compEvent1(pCmd,scd,cmdName,scl_item_event,scl_item_nums_event);

	compErrCount(pCmd,false);*/

	

	
	
	
	

	pCmd->cmdCode = CMD_SCD_CHECK_FINISH;

	resultCmd(pCmd,"模型检查完成");

}



void  ClientCmdThread::compErrCount(PCMD_GEN pCmd,t_bool bFileFlag)
{

	t_char desc[MAX_NAMELEN];

	pCmd->cmdCode = CMD_SCD_CHECK;
	pCmd->result = CMD_RESULT_RESULT_FAILD;

	COMP_RESULT1  compResult;
	pCmd->pos = COMP_RESULT;



	if (m_settingCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值个数不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值个数不相等");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);
	}

	if (m_settingRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值引用不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值引用不相符");
		}
		compResult.nums = m_settingRefCount;
		resultCmd(pCmd,compResult);
	}

	if (m_settingDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值描述不相符");
		}
		compResult.nums = m_settingDescCount;
		resultCmd(pCmd,compResult);
	}

	if (m_settingMaxCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值最大值不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值最大值不相等");
		}
		compResult.nums = m_settingMaxCount;
		resultCmd(pCmd,compResult);
	}

	if (m_settingMinCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值小值不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值小值不相等");
		}
		compResult.nums = m_settingMinCount;
		resultCmd(pCmd,compResult);

	}

	if (m_settingStepCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值步长不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值步长不相等");
		}
		compResult.nums = m_settingStepCount;
		resultCmd(pCmd,compResult);

	}

	if (m_settingUnitCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:定值单位不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:定值单位不相符");
		}
		compResult.nums = m_settingUnitCount;
		resultCmd(pCmd,compResult);
	}

	if (m_equipParamCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:装置参数个数不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:装置参数个数不相等");
		}
		
		compResult.nums = 0;
		resultCmd(pCmd,compResult);
	}

	if (m_equipParamRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:装置参数引用不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:装置参数引用不相符");
		}
		compResult.nums = m_equipParamRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:装置参数描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:装置参数描述不相符");
		}
		compResult.nums = m_equipParamDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamMaxCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:装置参数最大值不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:装置参数最大值不相等");
		}
		
		compResult.nums = m_equipParamMaxCount;
		resultCmd(pCmd,compResult);
	}

	if (m_equipParamMinCount >0 )
	{
		if (bFileFlag)
		{
		     sprintf(compResult.desc,"文件->装置:装置参数最小值不相等");
		}
		else
		{
			 sprintf(compResult.desc,"装置->文件:装置参数最小值不相等");
		}
		compResult.nums = m_equipParamMinCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamStepCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:装置参数步长不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:装置参数步长不相等");
		}
		compResult.nums = m_equipParamStepCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamUnitCount >0 )
	{
		if (bFileFlag)
		{
		  sprintf(compResult.desc,"文件->装置:装置参数单位不相符");
		}
		else
		{	
			 sprintf(compResult.desc,"装置->文件:装置参数单位不相符");
		}
		compResult.nums = m_equipParamUnitCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值个数不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥测值个数不相等");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_analogRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值引用不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥测值引用不相符");
		}
		compResult.nums = m_analogRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥测值描述不相符");
		}
		compResult.nums = m_analogDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogMaxCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值最大值不相等");
		}
		else
		{	
			sprintf(compResult.desc,"装置->文件:遥测值最大值不相等");
		}
		compResult.nums = m_analogMaxCount;
		resultCmd(pCmd,compResult);
	}

	if (m_analogMinCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值最小值不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥测值最小值不相等");
		}
		compResult.nums = m_analogMinCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogStepCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值步长不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥测值步长不相等");
		}
		compResult.nums = m_analogStepCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogUnitCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥测值单位不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥测值单位不相符");
		}
		compResult.nums = m_analogUnitCount;
		resultCmd(pCmd,compResult);

	}

	if (m_ybCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:压板个数不相");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:压板个数不相");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);
	}

	if (m_ybRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:压板引用不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:压板引用不相符");
		}
		compResult.nums = m_ybRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_ybDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:压板描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:压板描述不相符");
		}
		compResult.nums = m_ybDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_swiCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥信个数不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥信个数不相等");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_swiRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥信引用不相符");
		}
		else
		{
			
			sprintf(compResult.desc,"装置->文件:遥信引用不相符");
		}
		compResult.nums = m_swiRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_swiDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:遥信描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:遥信描述不相符");
		}
		compResult.nums = m_swiDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_alarmCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:告警个数不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:告警个数不相等");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_alarmRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:告警引用不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:告警引用不相符");
		}
		compResult.nums = m_alarmRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_alarmDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:告警描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:告警描述不相符");
		}
		compResult.nums = m_alarmDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_eventCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:事件个数不相等");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:事件个数不相等");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_eventRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:事件引用不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:事件引用不相符");
		}
		
		compResult.nums = m_eventRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_eventDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"文件->装置:事件描述不相符");
		}
		else
		{
			sprintf(compResult.desc,"装置->文件:事件描述不相符");
		}
		compResult.nums = m_eventDescCount;
		resultCmd(pCmd,compResult);
	}

}

void  ClientCmdThread::compSetting(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{
	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	t_short cmdCode;
	SETTING_ITEM1 *pDync_item=NULL;
	t_ushort      *pDync_item_nums = &scl_item_nums;

	pCmd->cmdCode = CMD_INIT_SETTING;

	
	//先初始化定值
	//if (pEqu->bInitSetting == false)
	{
		cmdCode = pCmd->cmdCode;
		
		sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		
		get_dyn_pt(pCmd->serverName,ITI_SETTING,&pDync_item,pDync_item_nums, NULL);

	
		sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		//pEqu->bInitSetting = true;
		pCmd->cmdCode = cmdCode;

	}

	pCmd->cmdCode = CMD_SCD_CHECK;

	////装载scl的点表
	//QHash<QString, int> hash;
 //
	//scd.read_event(pEqu->ptFileName,ATTRIB_SETTING,true,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_SETTING_NUMS,hash,NULL);
	//sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_HINT;
 //   resultCmd(pCmd,desc);
	//hash.clear();
	

	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if ((*pDync_item_nums) != pEqu->setting_Count)
	{
	
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"定值总数:%d",pEqu->setting_Count);
		sprintf(compItem.dync_desc,"定值总数:%d",*pDync_item_nums);
		
		resultCmd(pCmd,compItem);

		m_settingCount++;

	}

	for (int i = 0 ; i < pEqu->setting_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->setting_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < *pDync_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &pDync_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_settingDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最大值是%s",pItem->ref,pItem->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最大值是%s",pItem1->ref,pItem1->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_settingMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最小值是%s",pItem->ref,pItem->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最小值是%s",pItem1->ref,pItem1->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_settingMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的步长值是%s",pItem->ref,pItem->step_str);
					sprintf(compItem.dync_desc,"ref:%s的步长值是%s",pItem1->ref,pItem1->step_str);
					
					resultCmd(pCmd,compItem);

					m_settingStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的单位是%s",pItem->ref,pItem->unit);
					sprintf(compItem.dync_desc,"ref:%s的单位是%s",pItem1->ref,pItem1->unit);
					
					resultCmd(pCmd,compItem);

					m_settingUnitCount++;

				}
				break;
			}
		}


		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
			
			resultCmd(pCmd,compItem);

			m_settingRefCount++;

		}
	}
	


}

void  ClientCmdThread::compSetting1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_SCD_CHECK;

	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if (scl_item_nums != pEqu->setting_Count)
	{

		pCmd->result = CMD_RESULT_RESULT_FAILD;
	
		/*sprintf(compItem.dync_desc,"定值总数:%d",pEqu->setting_Count);
		sprintf(compItem.file1_desc,"定值总数:%d",scl_item_nums);
		
		resultCmd(pCmd,compItem);

		m_settingCount++;*/

	}

	for (int i = 0 ;i < pEqu->setting_Count; i++)
	{

		SETTING_ITEM1 *pItem = &pEqu->setting_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < scl_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &scl_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

			/*	if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_settingDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最大值是%s",pItem1->ref,pItem1->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最大值是%s",pItem->ref,pItem->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_settingMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最小值是%s",pItem1->ref,pItem1->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最小值是%s",pItem->ref,pItem->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_settingMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的步长值是%s",pItem1->ref,pItem1->step_str);
					sprintf(compItem.dync_desc,"ref:%s的步长值是%s",pItem->ref,pItem->step_str);
					
					resultCmd(pCmd,compItem);

					m_settingStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的单位是%s",pItem1->ref,pItem1->unit);
					sprintf(compItem.dync_desc,"ref:%s的单位是%s",pItem->ref,pItem->unit);
					
					resultCmd(pCmd,compItem);

					m_settingUnitCount++;

				}*/

				break;
			}


		}


		
		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"没有对应的ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"ref:%s",pItem->ref);
			
			resultCmd(pCmd,compItem);

			m_settingRefCount++;

		}
	}

}


void  ClientCmdThread::compEquipParam(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{
	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	SETTING_ITEM1 *pDync_item=NULL;
	t_ushort      *pDync_item_nums = &scl_item_nums;

	t_short cmdCode;

	pCmd->cmdCode = CMD_INIT_PARAM;
	//召唤定值点表
	//if (pEqu->bInitParam == false)
	{
		cmdCode = pCmd->cmdCode;  
		sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

	
		get_dyn_pt(pCmd->serverName,ITI_EQUP_PARAM,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		//pEqu->bInitParam = true;
		pCmd->cmdCode = cmdCode;


	}

	//装载scl的点表
	/*QHash<QString, int> hash;
	scd.read_event(pEqu->ptFileName,ATTRIB_EQU_PARAM,true,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_PARAM_NUMS,hash,NULL);
	sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	pCmd->result = CMD_RESULT_RESULT_FAILD;
	resultCmd(pCmd,desc);
	hash.clear();*/


	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;


	//开始比较
	if ((*pDync_item_nums) != pEqu->paramCount)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"装置参数的总个数%d",pEqu->paramCount);
		sprintf(compItem.dync_desc,"装置参数的总个数%d",*pDync_item_nums);
		
		resultCmd(pCmd,compItem);

		m_equipParamCount++;

	}

	for (int i = 0 ; i < pEqu->paramCount; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->param_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < (*pDync_item_nums); j++)
		{
			SETTING_ITEM1 *pItem1 = &pDync_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_equipParamDescCount++;

				}


				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最大值是%s",pItem->ref,pItem->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最大值是%s",pItem1->ref,pItem1->max_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{

					sprintf(compItem.file1_desc,"ref:%s的最小值是%s",pItem->ref,pItem->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最小值是%s",pItem1->ref,pItem1->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的步长值是%s",pItem->ref,pItem->step_str);
					sprintf(compItem.dync_desc,"ref:%s的步长值是%s",pItem1->ref,pItem1->step_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的单位是%s",pItem->ref,pItem->unit);
					sprintf(compItem.dync_desc,"ref:%s的单位是%s",pItem1->ref,pItem1->unit);
					
					resultCmd(pCmd,compItem);

					m_equipParamUnitCount++;

				}
				break;
			}
		}


		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_equipParamRefCount++;

		}
	}
	


}


void ClientCmdThread::compEquipParam1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;


	//开始比较
	if (scl_item_nums != pEqu->paramCount)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"装置参数的总个数%d",scl_item_nums);
		sprintf(compItem.dync_desc,"装置参数的总个数%d",pEqu->paramCount);
		
		resultCmd(pCmd,compItem);

		m_equipParamCount++;*/

	}

	for (int i = 0 ;i < pEqu->paramCount;i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->param_Item[i];

		t_bool bRet= false;

		for (int j = 0 ;j < scl_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &scl_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				/*if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_equipParamDescCount++;

				}


				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最大值是%s",pItem1->ref,pItem1->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最大值是%s",pItem->ref,pItem->max_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{

					sprintf(compItem.file1_desc,"ref:%s的最小值是%s",pItem1->ref,pItem1->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最小值是%s",pItem->ref,pItem->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的步长值是%s",pItem1->ref,pItem1->step_str);
					sprintf(compItem.dync_desc,"ref:%s的步长值是%s",pItem->ref,pItem->step_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的单位是%s",pItem1->ref,pItem1->unit);
					sprintf(compItem.dync_desc,"ref:%s的单位是%s",pItem->ref,pItem->unit);
					
					resultCmd(pCmd,compItem);

					m_equipParamUnitCount++;

				}*/

				break;
			}
		}


		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"没有对应的ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_equipParamRefCount++;

		}
	}


}


void  ClientCmdThread::compAnalog(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	SETTING_ITEM1 *pDync_item=NULL;
	t_ushort      *pDync_item_nums = &scl_item_nums;

	t_short cmdCode;

	pCmd->cmdCode = CMD_INIT_SAMPLE;
	//召唤定值点表
	//if (pEqu->bInitSample == false)
	{
		cmdCode = pCmd->cmdCode;  
		
		sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		
		get_dyn_pt(pCmd->serverName,ITI_TELEMETRY,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		 resultCmd(pCmd,desc);
		//pEqu->bInitSample = true;
		pCmd->cmdCode = cmdCode;


	}

	//装载scl的点表

	//QHash<QString, int> hash;
	//scd.read_event(pEqu->ptFileName,ATTRIB_SAMPLE,true,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_SAMPLE_NUMS,hash,NULL);
	//sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_RESULT_FAILD;
	//resultCmd(pCmd,desc);
	//hash.clear();
	

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if ((*pDync_item_nums) != pEqu->sample_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"遥测值的总个数%d",pEqu->sample_Count);
		sprintf(compItem.dync_desc,"遥测值的总个数%d",*pDync_item_nums);
		
		resultCmd(pCmd,compItem);

		m_analogCount++;

	}

	for (int i = 0 ; i < pEqu->sample_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->sample_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < *pDync_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &pDync_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				if (strstr(pItem1->ref,"ang") != NULL)
				{
					sprintf(pItem1->desc,"%s(角度)",pItem1->desc);
				}

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_analogDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最大值是%s",pItem->ref,pItem->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最大值是%s",pItem1->ref,pItem1->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_analogMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最小值是%s",pItem->ref,pItem->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最小值是%s",pItem1->ref,pItem1->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_analogMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的步长值是%s",pItem->ref,pItem->step_str);
					sprintf(compItem.dync_desc,"ref:%s的步长值是%s",pItem1->ref,pItem1->step_str);
					
					resultCmd(pCmd,compItem);

					m_analogStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的单位是%s",pItem->ref,pItem->unit);
					sprintf(compItem.dync_desc,"ref:%s的单位是%s",pItem1->ref,pItem1->unit);
					
					resultCmd(pCmd,compItem);

					m_analogUnitCount++;

				}

				break;
			}
		}


		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_analogRefCount++;

		}
	}
}

void   ClientCmdThread::compAnalog1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if (scl_item_nums != pEqu->sample_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"遥测值的总个数%d",scl_item_nums);
		sprintf(compItem.dync_desc,"遥测值的总个数%d",pEqu->sample_Count);
		
		resultCmd(pCmd,compItem);

		m_analogCount++;*/

	}

	for (int i = 0 ;i < pEqu->sample_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->sample_Item[i];

		t_bool bRet= false;
		for (int j = 0 ; j < scl_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &scl_item[j];

		   if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

			/*	if (strstr(pItem1->ref,"ang") != NULL)
				{
					sprintf(pItem1->desc,"%s(角度)",pItem1->desc);
				}

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_analogDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最大值是%s",pItem1->ref,pItem1->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最大值是%s",pItem->ref,pItem->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_analogMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的最小值是%s",pItem1->ref,pItem1->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s的最小值是%s",pItem->ref,pItem->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_analogMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s的步长值是%s",pItem1->ref,pItem1->step_str);
					sprintf(compItem.dync_desc,"ref:%s的步长值是%s",pItem->ref,pItem->step_str);
					
					resultCmd(pCmd,compItem);

					m_analogStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的单位是%s",pItem1->ref,pItem1->unit);
					sprintf(compItem.dync_desc,"ref:%s的单位是%s",pItem->ref,pItem->unit);
					
					resultCmd(pCmd,compItem);

					m_analogUnitCount++;

				}*/

				break;
			}

		}

		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"没有对应的ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_analogRefCount++;

		}

	}


}

void ClientCmdThread::compYb(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	SETTING_ITEM1 *pDync_item=NULL;
	t_ushort      *pDync_item_nums = &scl_item_nums;

	t_short cmdCode;
	pCmd->cmdCode = CMD_INIT_YB;
	//召唤定值点表
	//if (pEqu->bInit_Yb == false)
	{
		cmdCode = pCmd->cmdCode;  
		
		sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		get_dyn_pt(pCmd->serverName,ITI_YB,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		//pEqu->bInit_Yb = true;
		pCmd->cmdCode = cmdCode;


	}

	////装载scl的点表
	//QHash<QString, int> hash;
	//scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_YB_NUMS,hash,NULL);
	//sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_RESULT_FAILD;
	//resultCmd(pCmd,desc);
	//hash.clear();
	
	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if ((*pDync_item_nums) != pEqu->yb_Count)
	{
	
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"压板的总个数%d",pEqu->yb_Count);
		sprintf(compItem.dync_desc,"压板的总个数%d",*pDync_item_nums);
		
		resultCmd(pCmd,compItem);

		m_ybCount++;

	}

	for (int i = 0 ; i < pEqu->yb_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->yb_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < (*pDync_item_nums); j++)
		{
			SETTING_ITEM1 *pItem1 = &pDync_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_ybDescCount++;
				}
				break;

			}
		}


		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_ybRefCount++;

		}
	}

}


void ClientCmdThread::compYb1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if (scl_item_nums != pEqu->yb_Count)
	{
	
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"压板的总个数%d",scl_item_nums);
		sprintf(compItem.dync_desc,"压板的总个数%d",pEqu->yb_Count);
		
		resultCmd(pCmd,compItem);

		m_ybCount++;*/

	}

	for (int i = 0 ;i < pEqu->yb_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->yb_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < scl_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &scl_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				/*if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_ybDescCount++;
				}*/
		    	break;
			}
		}

		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_ybRefCount++;

		}

	}	
}

void   ClientCmdThread::compSwi(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_INIT_SWITCH;
	SETTING_ITEM1 *pDync_item=NULL;
	t_ushort      *pDync_item_nums = &scl_item_nums;

	t_short cmdCode;
	//召唤定值点表
	//if (pEqu->bInitSwitch == false)
	{
		cmdCode = pCmd->cmdCode;  
		

		sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		
		get_dyn_pt(pCmd->serverName,ITI_SWI,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		 resultCmd(pCmd,desc);
		//pEqu->bInitSwitch = true;
		pCmd->cmdCode = cmdCode;


	}


	////装载scl的点表
	//QHash<QString, int> hash;
	//scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_SWI_NUMS,hash,NULL);
	//sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_RESULT_FAILD;
	//resultCmd(pCmd,desc);
	//hash.clear();
	
	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if ((*pDync_item_nums) != pEqu->swi_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"遥信的总个数%d",pEqu->swi_Count);
		sprintf(compItem.dync_desc,"遥信的总个数%d",*pDync_item_nums);
		
		resultCmd(pCmd,compItem);

		m_swiCount++;

	}

	for (int i = 0 ; i < pEqu->swi_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->swi_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < (*pDync_item_nums); j++)
		{
			SETTING_ITEM1 *pItem1 = &pDync_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_swiDescCount++;
				}

				break;

			}
		}


		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_swiRefCount++;

		}
	}

}

void  ClientCmdThread::compSwi1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{
	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;
	
	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if (scl_item_nums != pEqu->swi_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"遥信的总个数%d",scl_item_nums);
		sprintf(compItem.dync_desc,"遥信的总个数%d",pEqu->swi_Count);
		
		resultCmd(pCmd,compItem);

		m_swiCount++;*/

	}

	for (int i = 0; i < pEqu->swi_Count; i++)
	{

		SETTING_ITEM1 *pItem = &pEqu->swi_Item[i];

		t_bool bRet= false;

		for (int j = 0 ;j < scl_item_nums; j++)
		{
			SETTING_ITEM1 *pItem1 = &scl_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				/*if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_swiDescCount++;



				}*/

				break;
			}


		}

		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);

			m_swiRefCount++;

		}
	}

}


void   ClientCmdThread::compEvent(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_INIT_EVET;
	t_short cmdCode;

	SETTING_ITEM1 *pDync_item=NULL;
	t_ushort      *pDync_item_nums = &scl_item_nums;

	//召唤定值点表
	//if (pEqu->bInit_Event == false)
	{
		cmdCode = pCmd->cmdCode;  
		
		sprintf(desc,"正在动态调用%s点表,请稍侯...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		get_dyn_pt(pCmd->serverName,ITI_EVENT,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"动态召唤%s点%d条,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		//pEqu->bInit_Event = true;
		pCmd->cmdCode = cmdCode;


	}

	//装载scl的点表
   /* QHash<QString, int> hash;
	scd.read_event(pEqu->ptFileName,ATTRIB_EVENT,false,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_EVENT_NUMS,hash,NULL);
	sprintf(desc,"装载%s点%d条,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	pCmd->result = CMD_RESULT_RESULT_FAILD;
	resultCmd(pCmd,desc);
	hash.clear();
	*/

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if ((*pDync_item_nums) != pEqu->event_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"事件的总个数%d",pEqu->event_Count);
		sprintf(compItem.dync_desc,"事件的总个数%d",*pDync_item_nums);
		
		resultCmd(pCmd,compItem);

		m_eventCount++;

	}

	for (int i = 0 ; i < pEqu->event_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->event_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < (*pDync_item_nums); j++)
		{
			SETTING_ITEM1 *pItem1 = &pDync_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s的描述是%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s的描述是%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_eventDescCount++;

				}
				break;

			}
		}


		if (bRet == false)
		{
		
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"没有对应的ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);


			m_eventRefCount++;

		}
	}

}


void   ClientCmdThread::compEvent1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums )
{

	t_char desc[MAX_NAMELEN];

	PEQU_PARAM pEqu = getEquipWithAppend(pCmd->serverName);
	if (!pEqu)
		return ;

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//开始比较
	if (scl_item_nums != pEqu->event_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

	}

	for (int i = 0 ; i < pEqu->event_Count; i++)
	{
		SETTING_ITEM1 *pItem = &pEqu->event_Item[i];

		t_bool bRet= false;

		for (int j = 0 ; j < scl_item_nums; j++)
		{

			SETTING_ITEM1 *pItem1 = &scl_item[j];

			if (strcmp(pItem->ref,pItem1->ref) == 0)
			{
				bRet = true;

				break;
			}
		}

		if (bRet == false)
		{
		
			sprintf(compItem.file1_desc,"没有对应的ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"ref:%s",pItem->ref);
	
			resultCmd(pCmd,compItem);


			m_eventRefCount++;

		}
	}

	
}


void  ClientCmdThread::resultCmd(PCMD_GEN pCmd,COMP_ITEM1 &item)
{

	CMD_GEN *pCmd1 = new CMD_GEN();

	memcpy(pCmd1,pCmd,sizeof(CMD_GEN));
	memcpy(pCmd1->data,&item,sizeof(item));

	//发送返回信号
	emit resultData_signals(pCmd1);

}

void  ClientCmdThread::resultCmd(PCMD_GEN pCmd,COMP_RESULT1 &item)
{

	
	CMD_GEN *pCmd1 = new CMD_GEN();

	memcpy(pCmd1,pCmd,sizeof(CMD_GEN));
	memcpy(pCmd1->data,&item,sizeof(item));

	//发送返回信号
	emit resultData_signals(pCmd1);
}
