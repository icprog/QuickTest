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
		 QString str = "���������߳�ʧ��";
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
	QString str = "�߳��˳�";
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
		
		//�Ӷ�����ɾ��
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
		case CMD_CONNECT:                       //����
			connectServer(pCmd);
			break;
		case CMD_DISTCNNECT:                    //�Ͽ�
			destConnect(pCmd);
			break;
		
		case CMD_BECKON_SETTING:                //�ٻ���ֵ
		case CMD_BECKON_YB:
		case CMD_EQU_PARAM:
		case CMD_BECKON_SWITCH:
		case CMD_BECKON_SAMPLE:
			{
				getRealay(pCmd);
			}
		break;
		case CMD_MODIFY_SETTING:                    //�޸Ķ�ֵ
			{
				modifySetting(pCmd);
			}
			break;
		case CMD_MODIFY_YB:                 //�޸�ѹ��
		{
			modifyYb(pCmd);
		}
		break;
		case CMD_MODIFY_PARAM:               //�޸�װ�ò���
		{
		   modifyParam(pCmd);
		}
		break;
		case CMD_DESCODE_PT_SCL:
		case CMD_DESCODE_IP_SCL:                   //����scl�ļ�
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
		sprintf(desc,"�Ͽ��ɹ�");
		pEqu->bConnect = false;
		pCmd->result = CMD_RESULT_SUCCESS;

	}
	else
	{
		sprintf(desc,"�Ѿ����ڶϿ�״̬");
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

	sprintf(desc,"�������� servername:%s,���Ժ�...",pCmd->serverName);
	pCmd->result = CMD_RESULT_SUCCESS;
    resultCmd(pCmd,desc);

	t_int  ret = auto_test_connect_equip(pCmd->serverName, desc);
 
	if (ret == 0 )
	{		
		sprintf(desc,"���ӳɹ� servername:%s...",pCmd->serverName);
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


	   //ע����ϱ���ص�����
	   auto_test_set_fault_report_callback_fun(rpt_callback_fun,this);

	   m_sclFlag = false;

	   	if  ( (QString(pCmd->sclPath.scl_file_name) != "") && (QString(pCmd->sclPath.ied_name) != "") && (QString(pCmd->sclPath.access_point_name) != "") /*&& (pCmd->sclPath.enabled==true)*/ )
		{
			sprintf(desc,"%s����װ��SCL�ļ�:%s,���Ժ�...",pCmd->equpName,pCmd->sclPath.scl_file_name);
			pCmd->result = CMD_RESULT_HINT;
			resultCmd(pCmd,desc);

			strcpy(pEqu->sclPath.scl_file_name,pCmd->sclPath.scl_file_name);
			strcpy(pEqu->sclPath.ied_name,pCmd->sclPath.ied_name);
			strcpy(pEqu->sclPath.access_point_name,pCmd->sclPath.access_point_name);
			pEqu->sclPath.enabled = pCmd->sclPath.enabled;

			ret = auto_test_local_scl_model(pCmd->serverName, pCmd->sclPath.scl_file_name, pCmd->sclPath.ied_name, pCmd->sclPath.access_point_name, desc);
			if (ret != 0)
			{
				sprintf(desc,"װ��SCL�ļ�ʧ��:%s,Զ�̵�ȡģ��...",pCmd->sclPath.scl_file_name);
				pEqu->bConnect = false;
				pCmd->result = CMD_RESULT_SUCCESS;
				resultCmd(pCmd,desc);

				 pCmd->cmdCode = CMD_SCD_CHECK;
				 sprintf(desc,"%sװ��װ��SCL�ļ�:%sʧ��,��ȷ���ý�ģ�ļ��Ƿ�ͨ��һ���Լ��",pCmd->equpName,pCmd->sclPath.scl_file_name);
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
				sprintf(desc,"%sװ��װ��SCL�ļ�:%s�ɹ�,�������ļ������ȷ",pCmd->equpName,pCmd->sclPath.scl_file_name);
				pCmd->result = CMD_RESULT_SUCCESS;
				resultCmd(pCmd,desc);

				pCmd->cmdCode = CMD_SCD_CHECK;
				sprintf(desc,"%sװ��װ��SCL�ļ�:%s�ɹ�,�������ļ������ȷ",pCmd->equpName,pCmd->sclPath.scl_file_name);
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

					//װ���¼����
					sprintf(desc,"��ʼװ���¼���Ϣ,���Ժ�...,%s",pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);
					pEqu->event_Count = 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_EVENT,false,pCmd->sclPath.ied_name,pEqu->event_Item,(t_uint32 *)&pEqu->event_Count,MAX_EVENT_NUMS,hash,NULL);
					sprintf(desc,"װ���¼���%d��,%s",pEqu->event_Count,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);



					//װ��ѹ����
					/*sprintf(desc,"��ʼװ��ѹ����Ϣ,���Ժ�...,%s",pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);
					pEqu->yb_Count = 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pCmd->sclPath.ied_name,pEqu->yb_Item,(t_uint32 *)&pEqu->yb_Count,MAX_YB_NUMS,hash,NULL);
					sprintf(desc,"װ�ظ�ѹ���%d��,%s",pEqu->yb_Count,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_SUCCESS;
				    resultCmd(pCmd,desc);	
					pEqu->bInitModel = true;*/
				}
			}

		}

		if (m_sclFlag == false) 
		{
			 //  ��ȡģ��
		   pCmd->cmdCode = CMD_INIT_MODEL;
		   initModel(pCmd,false);
		   pCmd->result = CMD_RESULT_SUCCESS;

		}


	}
	else
	{
		sprintf(desc,"����ʧ�� servername:%s...",pCmd->serverName);
		pEqu->bConnect = false;
		pCmd->cmdCode = CMD_CONNECT;
		pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		
	}

	if (pEqu->bInitModel)
	{
		pCmd->cmdCode=CMD_INIT_MODEL;
		pCmd->result = CMD_RESULT_SUCCESS;
		sprintf(desc,"��ʼ�����ݳɹ�,���Խ���װ�õĳ������:%s...",pCmd->serverName);
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

	 sprintf(desc,"%s,�޸�ѹ�忪ʼ",pCmd->serverName);
	 pCmd->result = CMD_RESULT_HINT;
	 resultCmd(pCmd,desc);


	ITEM_PARAM    *yb_Item = new ITEM_PARAM[pEqu->edit_yb_count];
    t_ushort      yb_Count = pEqu->edit_yb_count;


	//������վ�޸ĵ�ǰ����ֵ����ֵ�޸�Ŀ��ֵ����
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
			sprintf(desc,"%s,�޸�ѹ��%sʧ��",pCmd->serverName,pItem1->ref);
			pCmd->result = CMD_RESULT_HINT;
			resultCmd(pCmd,desc);
			bRet = false;
		}

		mySleep(1000);

	}


	delete[] yb_Item;


	 if (bRet )
	{
		sprintf(desc,"%s,�޸�ѹ��ɹ�",pCmd->serverName);
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);
	}
	else
	{
		sprintf(desc,"%s,�޸�ѹ��ʧ��",pCmd->serverName);
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

	 sprintf(desc,"%s,�޸�װ�ò�����ʼ",pCmd->serverName);
	 pCmd->result = CMD_RESULT_HINT;
	 resultCmd(pCmd,desc);


	ITEM_PARAM    *param_Item = new ITEM_PARAM[pEqu->edit_param_count];
    t_ushort      param_Count = pEqu->edit_param_count;


	//������վ�޸ĵ�ǰ����ֵ����ֵ�޸�Ŀ��ֵ����
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
			sprintf(desc,"%s,�޸�װ�ò���%sʧ��",pCmd->serverName,pItem1->ref);
			pCmd->result = CMD_RESULT_HINT;
			resultCmd(pCmd,desc);
			bRet = false;
		}

		mySleep(1000);

	}


	delete[] param_Item;


	 if (bRet )
	{
		sprintf(desc,"%s,�޸�װ�ò����ɹ�",pCmd->serverName);
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);
	}
	else
	{
		sprintf(desc,"%s,�޸�װ�ò���ʧ��",pCmd->serverName);
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

	 sprintf(desc,"%s,�޸Ķ�ֵ��ʼ",pCmd->serverName);
	 pCmd->result = CMD_RESULT_HINT;
	 resultCmd(pCmd,desc);


	ITEM_PARAM    *setting_Item = new ITEM_PARAM[pEqu->edit_setting_count];
    t_ushort      setting_Count = pEqu->edit_setting_count;


	//������վ�޸ĵ�ǰ����ֵ����ֵ�޸�Ŀ��ֵ����
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
		sprintf(desc,"%s,�޸Ķ�ֵ�ɹ�",pCmd->serverName);
		pCmd->result = CMD_RESULT_SUCCESS;
		resultCmd(pCmd,desc);
	}
	else
	{
		sprintf(desc,"%s,�޸Ķ�ֵʧ��",pCmd->serverName);
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
					sprintf(desc,"��ֵ");
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
					sprintf(desc,"װ�ò���");
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
				sprintf(desc,"ң��ֵ");
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
				sprintf(desc,"ѹ��");
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
				sprintf(desc,"ң��");
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
				sprintf(desc,"�¼�");
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
				sprintf(desc,"�澯");
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
				sprintf(desc,"¼��");
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
				sprintf(desc,"����");
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
	
	//���attribId
	getAttribId(pCmd->cmdCode,bSv,attribId);
	//���scd��װ��
	pIed = getScdIED(pEqu->equpName);
	if (pIed == NULL)
	{
		bSclEqu = false;

	}

	switch(pCmd->cmdCode)
	{

	case CMD_BECKON_SETTING:
		{
			cmdName = QString("��ֵ");
			//�ٻ���ֵ���
			if (pEqu->bInitSetting == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != "") )
				{
					sprintf(desc," ���ڴ������ļ�����%s���,���Ժ�...%s",cmdName.toAscii().data(),pCmd->serverName);
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
					//װ�ض�ֵ���
					*pItemCount= 0;
					scd.read_event(pEqu->ptFileName,ATTRIB_SETTING,true,pEqu->sclPath.ied_name,pItem,(t_uint32*)pItemCount,MAX_SETTING_NUMS,hash,NULL);
					sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitSetting = true;

					//����
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
					sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
					
					pCmd->cmdCode = CMD_INIT_SETTING;
					pEqu->setting_Count = 0;
					get_dyn_pt(pCmd->serverName,ITI_SETTING,&pEqu->setting_Item,&pEqu->setting_Count, NULL);
					pItem = pEqu->setting_Item;
				    pItemCount = &pEqu->setting_Count;
				

					sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
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
			cmdName = QString("װ�ò���");
			//�ٻ���ֵ���
			if (pEqu->bInitParam == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != "") )
				{
					sprintf(desc," ���ڴ������ļ�����%s���,���Ժ�...%s",cmdName.toAscii().data(),pCmd->serverName);
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
					sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitParam = true;

					//����
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
					sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
					
					pCmd->cmdCode = CMD_INIT_PARAM;
					pEqu->paramCount= 0;
					get_dyn_pt(pCmd->serverName,ITI_EQUP_PARAM,&pEqu->param_Item,&pEqu->paramCount, NULL);
					pItem = pEqu->param_Item;
					pItemCount = &pEqu->paramCount;
				

					sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
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
			cmdName = QString("��ѹ��");

			if (pEqu->bInit_Yb == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != ""))
				{
					sprintf(desc," ���ڴ������ļ�����%s���,���Ժ�...%s",cmdName.toAscii().data(),pCmd->serverName);
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
					sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInit_Yb = true;

					//����
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
					sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
				
					pCmd->cmdCode = CMD_INIT_YB;
					pEqu->yb_Count= 0;
					get_dyn_pt(pCmd->serverName,ITI_YB,&pEqu->yb_Item,&pEqu->yb_Count, NULL);
					pItem = pEqu->yb_Item;
					pItemCount = &pEqu->yb_Count;
					

					sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
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
			cmdName = QString("ң��ֵ");

			if (pEqu->bInitSample == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != ""))
				{
					sprintf(desc," ���ڴ������ļ�����%s���,���Ժ�...%s",cmdName.toAscii().data(),pCmd->serverName);
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
					sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitSample = true;

					//����
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
					sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
				
					pCmd->cmdCode = CMD_INIT_SAMPLE;
					pEqu->sample_Count= 0;
					get_dyn_pt(pCmd->serverName,ITI_TELEMETRY,&pEqu->sample_Item,&pEqu->sample_Count, NULL);
					pItem = pEqu->sample_Item;
					pItemCount = &pEqu->sample_Count;
					
					sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
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
			cmdName = QString("ң��");
			if (pEqu->bInitSwitch == false)
			{
				if  ( (QString(pEqu->sclPath.scl_file_name) != "") && (QString(pEqu->sclPath.ied_name) != "") && (QString(pEqu->sclPath.access_point_name) != ""))
				{
					sprintf(desc," ���ڴ������ļ�����%s���,���Ժ�...%s",cmdName.toAscii().data(),pCmd->serverName);
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
					sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);
					pEqu->bInitSwitch = true;

					//����
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
					sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
			    	pCmd->result = CMD_RESULT_RESULT_FAILD;
				    resultCmd(pCmd,desc);

					t_uint8 tmpCmdCode = pCmd->cmdCode;
				
					pCmd->cmdCode = CMD_INIT_SWITCH;
					pEqu->swi_Count= 0;
					get_dyn_pt(pCmd->serverName,ITI_SWI,&pEqu->swi_Item,&pEqu->swi_Count, NULL);
					pItem = pEqu->swi_Item;
					pItemCount = &pEqu->swi_Count;
					

					sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pItemCount,pCmd->serverName);
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


	QString str = QString("���ڻ�ȡ%1,���Ժ�...").arg(hint);
	pCmd->result = CMD_RESULT_RESULT_FAILD;
	resultCmd(pCmd,str.toAscii().data());

	t_short scdErrNums = 0;


	//�����ַ�������
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


	//ȡԶ�������ݼ����ֵ
	if (auto_test_read_dataSet_value(pCmd->serverName,pdataSet,dataSetNums,strRef,*pItemCount,strVal,NULL) != 0)
		{	 
			 sprintf(desc,"%s,��ȡ%s��Ϣʧ��",pCmd->serverName,pdataSet[0]);
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
			 sprintf(desc,"%s,��ȡ%s��Ϣʧ��",pCmd->serverName,item.ref);
			 pCmd->result = CMD_RESULT_RESULT_FAILD;
			 resultCmd(pCmd,desc);
		}

	}*/

	//�ͷ��ַ�������
	for (int i = 0; i <  *pItemCount; i++)
    {
        free(strRef[i]);
		free(strVal[i]);
    }
	free(strRef);
	free(strVal);
	
	pCmd->result = CMD_RESULT_ENDBEGINPROCESS;
	resultCmd(pCmd,"");
	

	str = QString("������ȡ%1...").arg(hint);
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
	sprintf(desc,"��ʼ��ȡ��������Ŀ¼��Ϣ�������ڱ����ڴ����ݿ�,���Ժ�....server%s",pCmd->serverName);
	pCmd->result = CMD_RESULT_BEGINPROCESS;
	resultCmd(pCmd,desc);

	if (auto_test_dyn_scl_model(pCmd->serverName,NULL) == 0)
	{
		sprintf(desc,"��ȡԶ��װ��ģ�ͳɹ�,server:%s",pCmd->serverName);
		pCmd->result = CMD_RESULT_BEGINPROCESS;
		resultCmd(pCmd,desc);

	}

	pEqu->bInitModel = true;

	if (m_winFlag)
	{
		t_char err[256];

/*
		sprintf(desc,"��ʼ��̬�ٻ��澯������Ϣ,���Ժ�...,%s",pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
	
		if (get_dyn_pt(pCmd->serverName,ITI_ALARM,&pEqu->alarm_Item,(unsigned short*)&pEqu->alarm_Count,err) != 0)
		{
			sprintf(desc,"��̬�ٻ��澯������Ϣʧ��,%s",pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);
		}
		else
		{
			sprintf(desc,"��̬�ٻ����澯������Ϣ%d��,%s",pEqu->alarm_Count,pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);	
		}
*/
		sprintf(desc,"��ʼ��̬�ٻ��¼�������Ϣ,���Ժ�...,%s",pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->event_Count = 0;
		if (get_dyn_pt(pCmd->serverName,ITI_EVENT,&pEqu->event_Item,(unsigned short*)&pEqu->event_Count,err) != 0)
		{
			sprintf(desc,"��̬�ٻ��¼�������Ϣʧ��,%s",pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);
		}
		else
		{
			sprintf(desc,"��̬�ٻ����¼�������Ϣ%d��,%s",pEqu->event_Count,pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);	
		}

		sprintf(desc,"��ʼ��̬�ٻ�ѹ��������Ϣ,���Ժ�...,%s",pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->yb_Count = 0;
		if (get_dyn_pt(pCmd->serverName,ITI_YB,&pEqu->yb_Item,(unsigned short*)&pEqu->yb_Count,err) != 0)
		{
			printf(desc,"��̬�ٻ�ѹ��������Ϣʧ��,%s",pCmd->serverName);
	    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		    resultCmd(pCmd,desc);
		}
		else
		{
			sprintf(desc,"��̬�ٻ���ѹ��������Ϣ%d��,%s",pEqu->yb_Count,pCmd->serverName);
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

	sprintf(desc,"��ʼ����%s�ļ�...",fileName);
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
		sprintf(desc,"����%s�ļ��ɹ�...",fileName);
		pCmd->result = CMD_RESULT_SUCCESS;
	}
	else
	{
		sprintf(desc,"����%s�ļ�ʧ��...",fileName);
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

	//���ͷ����ź�
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
					sprintf(state,"Ͷ��");
				else
					sprintf(state,"�˳�");
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
					sprintf(state,"��");
				else
					sprintf(state,"��");
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
					sprintf(state,"����");
				else
					sprintf(state,"�˳�");
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


	//���ͷ����ź�
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

   m_settingCount = 0;                 //��ֵ���������
   m_settingRefCount = 0;
   m_settingDescCount= 0;             //��ֵ�����������
   m_settingMaxCount= 0;              //��ֵ�����ֵ�����
   m_settingMinCount= 0;              //��ֵ����Сֵ�����
   m_settingStepCount= 0;             //��ֵ���������
   m_settingUnitCount= 0;             //��ֵ��λ�����

   m_equipParamCount= 0;               //װ�ò������������
   m_equipParamRefCount = 0;
   m_equipParamDescCount= 0;           //װ�ò��������������
   m_equipParamMaxCount= 0;            //װ�ò������ֵ�����
   m_equipParamMinCount= 0;            //װ�ò�����Сֵ�����
   m_equipParamStepCount= 0;           //װ�ò������������
   m_equipParamUnitCount= 0;           //װ�ò��������

   m_analogCount= 0;                    //ң��ֵ���������
   m_analogRefCount = 0;
   m_analogDescCount= 0;                //ң��ֵ�����������
   m_analogMaxCount= 0;                 //ң��ֵ���ֵ�����
   m_analogMinCount= 0;                 //ң��ֵ��Сֵ�����
   m_analogStepCount= 0;                //ң��ֵ���������
   m_analogUnitCount= 0;                //ң��ֵ�����

   m_ybCount= 0;                    //ң��ֵ���������
   m_ybRefCount = 0;
   m_ybDescCount= 0;                //ң��ֵ�����������

   m_swiCount= 0;                    //ң��ֵ���������
   m_swiRefCount = 0;
   m_swiDescCount= 0;                //ң��ֵ�����������

   m_alarmCount= 0;                    //ң��ֵ���������
   m_alarmRefCount = 0;
   m_alarmDescCount= 0;                //ң��ֵ�����������

   m_eventCount= 0;                    //ң��ֵ���������
   m_eventRefCount = 0;
   m_eventDescCount= 0;                //ң��ֵ�����������

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

	////����
	//if (connectServer(pCmd) == false)
	//	return ;

	pCmd->cmdCode = CMD_SCD_CHECK;

	ClientDSE_ScdManager scd;

	if (strlen(pEqu->ptFileName) == 0)
	{
		if (QString(pCmd->sclPath.scl_file_name) == "")
		{
			sprintf(desc,"û�бȽϵ�SCL�ļ�");
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
		sprintf(desc,"װ��%s�ļ�ʧ��,%s",pEqu->ptFileName,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		return ;

	}

	pCmd->cmdCode = cmdCode1;


	initModelCheckVars();

	//����ļ����
	if (pEqu->bInitSetting == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("��ֵ");
		if  (pEqu->setting_Item == NULL)
			pEqu->setting_Item = new SETTING_ITEM1[MAX_SETTING_NUMS];

		scd.read_event(pEqu->ptFileName,ATTRIB_SETTING,true,pEqu->sclPath.ied_name,pEqu->setting_Item,(t_uint32*)&pEqu->setting_Count,MAX_SETTING_NUMS,hash,NULL);
		sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),pEqu->setting_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitSetting = true;
		hash.clear();

	}

	if (pEqu->bInitParam == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("װ�ò���");
		if  (pEqu->param_Item == NULL)
			pEqu->param_Item = new SETTING_ITEM1[MAX_PARAM_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_EQU_PARAM,true,pEqu->sclPath.ied_name,pEqu->param_Item,(t_uint32*)&pEqu->paramCount,MAX_PARAM_NUMS,hash,NULL);
		sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),pEqu->paramCount,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitParam = true;
		hash.clear();

	}

	if (pEqu->bInit_Yb == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("ѹ��");
		if  (pEqu->yb_Item == NULL)
			pEqu->yb_Item = new SETTING_ITEM1[MAX_YB_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pEqu->sclPath.ied_name,pEqu->yb_Item,(t_uint32*)&pEqu->yb_Count,MAX_YB_NUMS,hash,NULL);
		sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),pEqu->yb_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInit_Yb = true;
		hash.clear();

	}

	if (pEqu->bInitSample == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("ң��ֵ");
		if  (pEqu->sample_Item == NULL)
			pEqu->sample_Item = new SETTING_ITEM1[MAX_SAMPLE_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_SAMPLE,true,pEqu->sclPath.ied_name,pEqu->sample_Item,(t_uint32*)&pEqu->sample_Count,MAX_SAMPLE_NUMS,hash,NULL);
		sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),pEqu->sample_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitSample = true;
		hash.clear();

	}

	if (pEqu->bInitSwitch == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("ң��");
		if (pEqu->swi_Item == NULL)
			pEqu->swi_Item = new SETTING_ITEM1[MAX_SWI_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,pEqu->swi_Item,(t_uint32*)&pEqu->swi_Count,MAX_SWI_NUMS,hash,NULL);
		sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),pEqu->swi_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInitSwitch = true;
		hash.clear();

	}

	if (pEqu->bInit_Event == false)
	{
		QHash<QString, int> hash;
		cmdName = QString("�¼�");
		if (pEqu->event_Item == NULL)
			pEqu->event_Item = new SETTING_ITEM1[MAX_EVENT_NUMS];
		scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,pEqu->event_Item,(t_uint32*)&pEqu->event_Count,MAX_EVENT_NUMS,hash,NULL);
		sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),pEqu->event_Count,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);
		pEqu->bInit_Event = true;
		hash.clear();

	}



	//�������ĳ��˶�̬���
    SETTING_ITEM1 *scl_item_setting = new SETTING_ITEM1[MAX_SETTING_NUMS];
	t_ushort      scl_item_nums_setting = 0;
	cmdName = QString("��ֵ");
	compSetting(pCmd,scd,cmdName,scl_item_setting,scl_item_nums_setting);
	delete[] scl_item_setting;


	SETTING_ITEM1 *scl_item_param   = new SETTING_ITEM1[MAX_PARAM_NUMS];
	t_ushort      scl_item_nums_param = 0; 
	cmdName = QString("װ�ò���");
	compEquipParam(pCmd,scd,cmdName,scl_item_param,scl_item_nums_param);
	delete[] scl_item_param;


   SETTING_ITEM1 *scl_item_analog   = new SETTING_ITEM1[MAX_SAMPLE_NUMS];
	t_ushort      scl_item_nums_analog = 0; 
	cmdName = QString("ң��ֵ");
	compAnalog(pCmd,scd,cmdName,scl_item_analog,scl_item_nums_analog);
	delete[] scl_item_analog;

	//
    SETTING_ITEM1 *scl_item_yb   = new SETTING_ITEM1[MAX_YB_NUMS];
	t_ushort      scl_item_nums_yb = 0; 
	cmdName = QString("ѹ��");
	compYb(pCmd,scd,cmdName,scl_item_yb,scl_item_nums_yb);	
	delete[] scl_item_yb;

    SETTING_ITEM1 *scl_item_swi   = new SETTING_ITEM1[MAX_SWI_NUMS];
	t_ushort      scl_item_nums_swi = 0; 
	cmdName = QString("ң��");
	compSwi(pCmd,scd,cmdName,scl_item_swi,scl_item_nums_swi);	
	delete[] scl_item_swi;


    SETTING_ITEM1 *scl_item_event   = new SETTING_ITEM1[MAX_EVENT_NUMS];
	t_ushort      scl_item_nums_event = 0; 
	cmdName = QString("�¼�");
	compEvent(pCmd,scd,cmdName,scl_item_event,scl_item_nums_event);
	delete[] scl_item_event;
	


	//����ͳ��
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

	resultCmd(pCmd,"ģ�ͼ�����");

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
			sprintf(compResult.desc,"�ļ�->װ��:��ֵ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵ���������");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);
	}

	if (m_settingRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:��ֵ���ò����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵ���ò����");
		}
		compResult.nums = m_settingRefCount;
		resultCmd(pCmd,compResult);
	}

	if (m_settingDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:��ֵ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵ���������");
		}
		compResult.nums = m_settingDescCount;
		resultCmd(pCmd,compResult);
	}

	if (m_settingMaxCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:��ֵ���ֵ�����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵ���ֵ�����");
		}
		compResult.nums = m_settingMaxCount;
		resultCmd(pCmd,compResult);
	}

	if (m_settingMinCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:��ֵСֵ�����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵСֵ�����");
		}
		compResult.nums = m_settingMinCount;
		resultCmd(pCmd,compResult);

	}

	if (m_settingStepCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:��ֵ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵ���������");
		}
		compResult.nums = m_settingStepCount;
		resultCmd(pCmd,compResult);

	}

	if (m_settingUnitCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:��ֵ��λ�����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:��ֵ��λ�����");
		}
		compResult.nums = m_settingUnitCount;
		resultCmd(pCmd,compResult);
	}

	if (m_equipParamCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:װ�ò������������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:װ�ò������������");
		}
		
		compResult.nums = 0;
		resultCmd(pCmd,compResult);
	}

	if (m_equipParamRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:װ�ò������ò����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:װ�ò������ò����");
		}
		compResult.nums = m_equipParamRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:װ�ò������������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:װ�ò������������");
		}
		compResult.nums = m_equipParamDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamMaxCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:װ�ò������ֵ�����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:װ�ò������ֵ�����");
		}
		
		compResult.nums = m_equipParamMaxCount;
		resultCmd(pCmd,compResult);
	}

	if (m_equipParamMinCount >0 )
	{
		if (bFileFlag)
		{
		     sprintf(compResult.desc,"�ļ�->װ��:װ�ò�����Сֵ�����");
		}
		else
		{
			 sprintf(compResult.desc,"װ��->�ļ�:װ�ò�����Сֵ�����");
		}
		compResult.nums = m_equipParamMinCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamStepCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:װ�ò������������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:װ�ò������������");
		}
		compResult.nums = m_equipParamStepCount;
		resultCmd(pCmd,compResult);

	}

	if (m_equipParamUnitCount >0 )
	{
		if (bFileFlag)
		{
		  sprintf(compResult.desc,"�ļ�->װ��:װ�ò�����λ�����");
		}
		else
		{	
			 sprintf(compResult.desc,"װ��->�ļ�:װ�ò�����λ�����");
		}
		compResult.nums = m_equipParamUnitCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ���������");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_analogRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ���ò����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ���ò����");
		}
		compResult.nums = m_analogRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ���������");
		}
		compResult.nums = m_analogDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogMaxCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ���ֵ�����");
		}
		else
		{	
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ���ֵ�����");
		}
		compResult.nums = m_analogMaxCount;
		resultCmd(pCmd,compResult);
	}

	if (m_analogMinCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ��Сֵ�����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ��Сֵ�����");
		}
		compResult.nums = m_analogMinCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogStepCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ���������");
		}
		compResult.nums = m_analogStepCount;
		resultCmd(pCmd,compResult);

	}

	if (m_analogUnitCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң��ֵ��λ�����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң��ֵ��λ�����");
		}
		compResult.nums = m_analogUnitCount;
		resultCmd(pCmd,compResult);

	}

	if (m_ybCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ѹ���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ѹ���������");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);
	}

	if (m_ybRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ѹ�����ò����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ѹ�����ò����");
		}
		compResult.nums = m_ybRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_ybDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ѹ�����������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ѹ�����������");
		}
		compResult.nums = m_ybDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_swiCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң�Ÿ��������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң�Ÿ��������");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_swiRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң�����ò����");
		}
		else
		{
			
			sprintf(compResult.desc,"װ��->�ļ�:ң�����ò����");
		}
		compResult.nums = m_swiRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_swiDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:ң�����������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:ң�����������");
		}
		compResult.nums = m_swiDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_alarmCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:�澯���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:�澯���������");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_alarmRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:�澯���ò����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:�澯���ò����");
		}
		compResult.nums = m_alarmRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_alarmDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:�澯���������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:�澯���������");
		}
		compResult.nums = m_alarmDescCount;
		resultCmd(pCmd,compResult);

	}

	if (m_eventCount >0)
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:�¼����������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:�¼����������");
		}
		compResult.nums = 0;
		resultCmd(pCmd,compResult);

	}

	if (m_eventRefCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:�¼����ò����");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:�¼����ò����");
		}
		
		compResult.nums = m_eventRefCount;
		resultCmd(pCmd,compResult);

	}

	if (m_eventDescCount >0 )
	{
		if (bFileFlag)
		{
			sprintf(compResult.desc,"�ļ�->װ��:�¼����������");
		}
		else
		{
			sprintf(compResult.desc,"װ��->�ļ�:�¼����������");
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

	
	//�ȳ�ʼ����ֵ
	//if (pEqu->bInitSetting == false)
	{
		cmdCode = pCmd->cmdCode;
		
		sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		
		get_dyn_pt(pCmd->serverName,ITI_SETTING,&pDync_item,pDync_item_nums, NULL);

	
		sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		//pEqu->bInitSetting = true;
		pCmd->cmdCode = cmdCode;

	}

	pCmd->cmdCode = CMD_SCD_CHECK;

	////װ��scl�ĵ��
	//QHash<QString, int> hash;
 //
	//scd.read_event(pEqu->ptFileName,ATTRIB_SETTING,true,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_SETTING_NUMS,hash,NULL);
	//sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_HINT;
 //   resultCmd(pCmd,desc);
	//hash.clear();
	

	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//��ʼ�Ƚ�
	if ((*pDync_item_nums) != pEqu->setting_Count)
	{
	
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"��ֵ����:%d",pEqu->setting_Count);
		sprintf(compItem.dync_desc,"��ֵ����:%d",*pDync_item_nums);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_settingDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�����ֵ��%s",pItem->ref,pItem->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s�����ֵ��%s",pItem1->ref,pItem1->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_settingMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s����Сֵ��%s",pItem->ref,pItem->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s����Сֵ��%s",pItem1->ref,pItem1->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_settingMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�Ĳ���ֵ��%s",pItem->ref,pItem->step_str);
					sprintf(compItem.dync_desc,"ref:%s�Ĳ���ֵ��%s",pItem1->ref,pItem1->step_str);
					
					resultCmd(pCmd,compItem);

					m_settingStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s�ĵ�λ��%s",pItem->ref,pItem->unit);
					sprintf(compItem.dync_desc,"ref:%s�ĵ�λ��%s",pItem1->ref,pItem1->unit);
					
					resultCmd(pCmd,compItem);

					m_settingUnitCount++;

				}
				break;
			}
		}


		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
			
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

	//��ʼ�Ƚ�
	if (scl_item_nums != pEqu->setting_Count)
	{

		pCmd->result = CMD_RESULT_RESULT_FAILD;
	
		/*sprintf(compItem.dync_desc,"��ֵ����:%d",pEqu->setting_Count);
		sprintf(compItem.file1_desc,"��ֵ����:%d",scl_item_nums);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_settingDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�����ֵ��%s",pItem1->ref,pItem1->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s�����ֵ��%s",pItem->ref,pItem->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_settingMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s����Сֵ��%s",pItem1->ref,pItem1->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s����Сֵ��%s",pItem->ref,pItem->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_settingMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�Ĳ���ֵ��%s",pItem1->ref,pItem1->step_str);
					sprintf(compItem.dync_desc,"ref:%s�Ĳ���ֵ��%s",pItem->ref,pItem->step_str);
					
					resultCmd(pCmd,compItem);

					m_settingStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s�ĵ�λ��%s",pItem1->ref,pItem1->unit);
					sprintf(compItem.dync_desc,"ref:%s�ĵ�λ��%s",pItem->ref,pItem->unit);
					
					resultCmd(pCmd,compItem);

					m_settingUnitCount++;

				}*/

				break;
			}


		}


		
		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
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
	//�ٻ���ֵ���
	//if (pEqu->bInitParam == false)
	{
		cmdCode = pCmd->cmdCode;  
		sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

	
		get_dyn_pt(pCmd->serverName,ITI_EQUP_PARAM,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		//pEqu->bInitParam = true;
		pCmd->cmdCode = cmdCode;


	}

	//װ��scl�ĵ��
	/*QHash<QString, int> hash;
	scd.read_event(pEqu->ptFileName,ATTRIB_EQU_PARAM,true,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_PARAM_NUMS,hash,NULL);
	sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	pCmd->result = CMD_RESULT_RESULT_FAILD;
	resultCmd(pCmd,desc);
	hash.clear();*/


	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;


	//��ʼ�Ƚ�
	if ((*pDync_item_nums) != pEqu->paramCount)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"װ�ò������ܸ���%d",pEqu->paramCount);
		sprintf(compItem.dync_desc,"װ�ò������ܸ���%d",*pDync_item_nums);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_equipParamDescCount++;

				}


				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�����ֵ��%s",pItem->ref,pItem->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s�����ֵ��%s",pItem1->ref,pItem1->max_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{

					sprintf(compItem.file1_desc,"ref:%s����Сֵ��%s",pItem->ref,pItem->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s����Сֵ��%s",pItem1->ref,pItem1->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�Ĳ���ֵ��%s",pItem->ref,pItem->step_str);
					sprintf(compItem.dync_desc,"ref:%s�Ĳ���ֵ��%s",pItem1->ref,pItem1->step_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s�ĵ�λ��%s",pItem->ref,pItem->unit);
					sprintf(compItem.dync_desc,"ref:%s�ĵ�λ��%s",pItem1->ref,pItem1->unit);
					
					resultCmd(pCmd,compItem);

					m_equipParamUnitCount++;

				}
				break;
			}
		}


		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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


	//��ʼ�Ƚ�
	if (scl_item_nums != pEqu->paramCount)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"װ�ò������ܸ���%d",scl_item_nums);
		sprintf(compItem.dync_desc,"װ�ò������ܸ���%d",pEqu->paramCount);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_equipParamDescCount++;

				}


				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�����ֵ��%s",pItem1->ref,pItem1->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s�����ֵ��%s",pItem->ref,pItem->max_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{

					sprintf(compItem.file1_desc,"ref:%s����Сֵ��%s",pItem1->ref,pItem1->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s����Сֵ��%s",pItem->ref,pItem->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�Ĳ���ֵ��%s",pItem1->ref,pItem1->step_str);
					sprintf(compItem.dync_desc,"ref:%s�Ĳ���ֵ��%s",pItem->ref,pItem->step_str);
					
					resultCmd(pCmd,compItem);

					m_equipParamStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s�ĵ�λ��%s",pItem1->ref,pItem1->unit);
					sprintf(compItem.dync_desc,"ref:%s�ĵ�λ��%s",pItem->ref,pItem->unit);
					
					resultCmd(pCmd,compItem);

					m_equipParamUnitCount++;

				}*/

				break;
			}
		}


		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
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
	//�ٻ���ֵ���
	//if (pEqu->bInitSample == false)
	{
		cmdCode = pCmd->cmdCode;  
		
		sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		
		get_dyn_pt(pCmd->serverName,ITI_TELEMETRY,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		 resultCmd(pCmd,desc);
		//pEqu->bInitSample = true;
		pCmd->cmdCode = cmdCode;


	}

	//װ��scl�ĵ��

	//QHash<QString, int> hash;
	//scd.read_event(pEqu->ptFileName,ATTRIB_SAMPLE,true,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_SAMPLE_NUMS,hash,NULL);
	//sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_RESULT_FAILD;
	//resultCmd(pCmd,desc);
	//hash.clear();
	

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//��ʼ�Ƚ�
	if ((*pDync_item_nums) != pEqu->sample_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"ң��ֵ���ܸ���%d",pEqu->sample_Count);
		sprintf(compItem.dync_desc,"ң��ֵ���ܸ���%d",*pDync_item_nums);
		
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
					sprintf(pItem1->desc,"%s(�Ƕ�)",pItem1->desc);
				}

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_analogDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�����ֵ��%s",pItem->ref,pItem->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s�����ֵ��%s",pItem1->ref,pItem1->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_analogMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s����Сֵ��%s",pItem->ref,pItem->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s����Сֵ��%s",pItem1->ref,pItem1->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_analogMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�Ĳ���ֵ��%s",pItem->ref,pItem->step_str);
					sprintf(compItem.dync_desc,"ref:%s�Ĳ���ֵ��%s",pItem1->ref,pItem1->step_str);
					
					resultCmd(pCmd,compItem);

					m_analogStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s�ĵ�λ��%s",pItem->ref,pItem->unit);
					sprintf(compItem.dync_desc,"ref:%s�ĵ�λ��%s",pItem1->ref,pItem1->unit);
					
					resultCmd(pCmd,compItem);

					m_analogUnitCount++;

				}

				break;
			}
		}


		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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

	//��ʼ�Ƚ�
	if (scl_item_nums != pEqu->sample_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"ң��ֵ���ܸ���%d",scl_item_nums);
		sprintf(compItem.dync_desc,"ң��ֵ���ܸ���%d",pEqu->sample_Count);
		
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
					sprintf(pItem1->desc,"%s(�Ƕ�)",pItem1->desc);
				}

				if (strcmp(pItem->desc,pItem1->desc) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_analogDescCount++;

				}

				if (atof(pItem->max_val_str) != atof(pItem1->max_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�����ֵ��%s",pItem1->ref,pItem1->max_val_str);
					sprintf(compItem.dync_desc,"ref:%s�����ֵ��%s",pItem->ref,pItem->max_val_str);
					
					resultCmd(pCmd,compItem);


					m_analogMaxCount++;

				}

				if (atof(pItem->min_val_str) != atof(pItem1->min_val_str))
				{
					sprintf(compItem.file1_desc,"ref:%s����Сֵ��%s",pItem1->ref,pItem1->min_val_str);
					sprintf(compItem.dync_desc,"ref:%s����Сֵ��%s",pItem->ref,pItem->min_val_str);
					
					resultCmd(pCmd,compItem);

					m_analogMinCount++;

				}

				if (atof(pItem->step_str) != atof(pItem1->step_str))
				{
					sprintf(compItem.file1_desc,"ref:%s�Ĳ���ֵ��%s",pItem1->ref,pItem1->step_str);
					sprintf(compItem.dync_desc,"ref:%s�Ĳ���ֵ��%s",pItem->ref,pItem->step_str);
					
					resultCmd(pCmd,compItem);

					m_analogStepCount++;

				}

				if (strcmp(pItem->unit,pItem1->unit) != 0)
				{
					sprintf(compItem.file1_desc,"ref:%s�ĵ�λ��%s",pItem1->ref,pItem1->unit);
					sprintf(compItem.dync_desc,"ref:%s�ĵ�λ��%s",pItem->ref,pItem->unit);
					
					resultCmd(pCmd,compItem);

					m_analogUnitCount++;

				}*/

				break;
			}

		}

		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
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
	//�ٻ���ֵ���
	//if (pEqu->bInit_Yb == false)
	{
		cmdCode = pCmd->cmdCode;  
		
		sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		get_dyn_pt(pCmd->serverName,ITI_YB,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		//pEqu->bInit_Yb = true;
		pCmd->cmdCode = cmdCode;


	}

	////װ��scl�ĵ��
	//QHash<QString, int> hash;
	//scd.read_event(pEqu->ptFileName,ATTRIB_YB,false,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_YB_NUMS,hash,NULL);
	//sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_RESULT_FAILD;
	//resultCmd(pCmd,desc);
	//hash.clear();
	
	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//��ʼ�Ƚ�
	if ((*pDync_item_nums) != pEqu->yb_Count)
	{
	
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"ѹ����ܸ���%d",pEqu->yb_Count);
		sprintf(compItem.dync_desc,"ѹ����ܸ���%d",*pDync_item_nums);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_ybDescCount++;
				}
				break;

			}
		}


		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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

	//��ʼ�Ƚ�
	if (scl_item_nums != pEqu->yb_Count)
	{
	
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"ѹ����ܸ���%d",scl_item_nums);
		sprintf(compItem.dync_desc,"ѹ����ܸ���%d",pEqu->yb_Count);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					
					resultCmd(pCmd,compItem);

					m_ybDescCount++;
				}*/
		    	break;
			}
		}

		if (bRet == false)
		{
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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
	//�ٻ���ֵ���
	//if (pEqu->bInitSwitch == false)
	{
		cmdCode = pCmd->cmdCode;  
		

		sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		
		get_dyn_pt(pCmd->serverName,ITI_SWI,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		 resultCmd(pCmd,desc);
		//pEqu->bInitSwitch = true;
		pCmd->cmdCode = cmdCode;


	}


	////װ��scl�ĵ��
	//QHash<QString, int> hash;
	//scd.read_event(pEqu->ptFileName,ATTRIB_SWI,false,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_SWI_NUMS,hash,NULL);
	//sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	//pCmd->result = CMD_RESULT_RESULT_FAILD;
	//resultCmd(pCmd,desc);
	//hash.clear();
	
	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//��ʼ�Ƚ�
	if ((*pDync_item_nums) != pEqu->swi_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"ң�ŵ��ܸ���%d",pEqu->swi_Count);
		sprintf(compItem.dync_desc,"ң�ŵ��ܸ���%d",*pDync_item_nums);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_swiDescCount++;
				}

				break;

			}
		}


		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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

	//��ʼ�Ƚ�
	if (scl_item_nums != pEqu->swi_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		/*sprintf(compItem.file1_desc,"ң�ŵ��ܸ���%d",scl_item_nums);
		sprintf(compItem.dync_desc,"ң�ŵ��ܸ���%d",pEqu->swi_Count);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_swiDescCount++;



				}*/

				break;
			}


		}

		if (bRet == false)
		{

			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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

	//�ٻ���ֵ���
	//if (pEqu->bInit_Event == false)
	{
		cmdCode = pCmd->cmdCode;  
		
		sprintf(desc,"���ڶ�̬����%s���,���Ժ�...%s ",cmdName.toAscii().data(),pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
	    resultCmd(pCmd,desc);

		get_dyn_pt(pCmd->serverName,ITI_EVENT,&pDync_item,pDync_item_nums, NULL);
	

		sprintf(desc,"��̬�ٻ�%s��%d��,%s ",cmdName.toAscii().data(),*pDync_item_nums,pCmd->serverName);
    	pCmd->result = CMD_RESULT_RESULT_FAILD;
		resultCmd(pCmd,desc);
		//pEqu->bInit_Event = true;
		pCmd->cmdCode = cmdCode;


	}

	//װ��scl�ĵ��
   /* QHash<QString, int> hash;
	scd.read_event(pEqu->ptFileName,ATTRIB_EVENT,false,pEqu->sclPath.ied_name,scl_item,(t_uint32*)&scl_item_nums,MAX_EVENT_NUMS,hash,NULL);
	sprintf(desc,"װ��%s��%d��,%s",cmdName.toAscii().data(),scl_item_nums,pCmd->serverName);
	pCmd->result = CMD_RESULT_RESULT_FAILD;
	resultCmd(pCmd,desc);
	hash.clear();
	*/

	pCmd->cmdCode = CMD_SCD_CHECK;
	COMP_ITEM1  compItem;
	pCmd->pos = COMP_ITEM;

	//��ʼ�Ƚ�
	if ((*pDync_item_nums) != pEqu->event_Count)
	{
		
		pCmd->result = CMD_RESULT_RESULT_FAILD;

		sprintf(compItem.file1_desc,"�¼����ܸ���%d",pEqu->event_Count);
		sprintf(compItem.dync_desc,"�¼����ܸ���%d",*pDync_item_nums);
		
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
					sprintf(compItem.file1_desc,"ref:%s��������%s",pItem->ref,pItem->desc);
					sprintf(compItem.dync_desc,"ref:%s��������%s",pItem1->ref,pItem1->desc);
					
					resultCmd(pCmd,compItem);

					m_eventDescCount++;

				}
				break;

			}
		}


		if (bRet == false)
		{
		
			sprintf(compItem.file1_desc,"ref:%s",pItem->ref);
			sprintf(compItem.dync_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
	
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

	//��ʼ�Ƚ�
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
		
			sprintf(compItem.file1_desc,"û�ж�Ӧ��ref:%s",pItem->ref);
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

	//���ͷ����ź�
	emit resultData_signals(pCmd1);

}

void  ClientCmdThread::resultCmd(PCMD_GEN pCmd,COMP_RESULT1 &item)
{

	
	CMD_GEN *pCmd1 = new CMD_GEN();

	memcpy(pCmd1,pCmd,sizeof(CMD_GEN));
	memcpy(pCmd1->data,&item,sizeof(item));

	//���ͷ����ź�
	emit resultData_signals(pCmd1);
}
