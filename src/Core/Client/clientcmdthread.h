#ifndef CMDTHREAD_H
#define CMDTHREAD_H

#include <QObject>
#include "clientdatadefine.h"
#include "clientxmlimpl.h" 
#include "autotest_global.h"
#include "clientscdheader.h"
#include "clientscdphase.h"



//�Ƚ���
typedef struct _COMP_ITEM1
{
	_COMP_ITEM1()
	{
		memset(this,0,sizeof(_COMP_ITEM1));
	}
	char  file1_desc[256];
	char  dync_desc[256];

}COMP_ITEM1,*PCOMP_ITEM1;

//�ȽϽ��
typedef struct _COMP_RESULT1
{
	_COMP_RESULT1()
	{
		memset(this,0,sizeof(_COMP_RESULT1));
	}

	char desc[256];
	int  nums;

}COMP_RESULT1,*PCOMP_RESULT1;


class ClientCmdThread : public QObject
{
	Q_OBJECT

public:
	ClientCmdThread(QObject *parent);
	~ClientCmdThread();

	bool start();

	void appendCmd(CMD_GEN &cmd)
	{

		m_cmdLock.lock();

		PCMD_GEN pCmd = new CMD_GEN();
		memcpy(pCmd,&cmd,sizeof(cmd));	
		m_cmdList.append(pCmd);

		m_cmdLock.unlock();

	}

	void setDir(t_char *tmpcfgDir,t_char *tmpDataDir)
	{
		strcpy(m_cfgDir,tmpcfgDir);
		strcpy(m_dataDir,tmpDataDir);
	}

	void setWinFlag(t_bool flag)
	{
		m_winFlag = flag;
	}

   PEQU_PARAM getEquipParam(t_char *serverName); 
   void       resultCmd(PCMD_GEN pCmd,t_char *desc);
   void       resultCmd(PCMD_GEN pCmd,COMP_ITEM1 &item);
   void       resultCmd(PCMD_GEN pCmd,COMP_RESULT1 &item);

   t_uint8    getCurDecodeSclType(){return m_curDecodeSclType;};

   void    readScdCfg();

   PEQU_PARAM getEquipWithAppend(t_char *serverName);

   QList< PCMD_GEN >     m_cmdList;              //������б�

   bool m_isShowDownDlg;


signals:

	void resultData_signals(CMD_GEN *pCmd);


private:
	

	void	  closeThread();

static	void  circleThread(void *pVoid);	               //Ѳ���߳�		  
	void      circleFun();

static  int  rpt_callback_fun(void* ptr, FAULT_REPORT *fault_report);

static t_int     progress_Hint(void* ptr, CMD_TYPE_DEF::CMD cmd);


    ClientSysLock  	  m_cmdLock;              //�����б���
	QList<PEQU_PARAM> m_equParamList;         //װ��ͨ�Ų����б�
	ClientDSE_ScdManager    m_scd;

	char      m_cfgDir[MAX_NAMELEN];               //�����ļ�Ŀ¼
	char      m_dataDir[MAX_NAMELEN];              //�����ļ�Ŀ¼

	t_bool    m_bReadScd;

	bool      m_exit;
   _HANDLE    m_handle;					   //Ѳ���̵߳ľ��
   bool       m_trheadExt;
   t_bool     m_winFlag;
   t_bool     m_sclFlag ;

   DSE_XmlSaxHandler  xml;

   t_uint8    m_curDecodeSclType;
   
   t_char     dataSetList[MAX_DATASET_NUMS][MAX_NAMELEN];   //���50������
   t_int      m_dataSetNums ;

   void       initVars( );

   void       dealCmd();
   void       sendCmd(PCMD_GEN pCmd);


   //���ӷ�����
   t_bool       connectServer(PCMD_GEN pCmd);
   void         destConnect(PCMD_GEN pCmd);

   //��̬��õ��
   unsigned int  get_dyn_pt(t_char *serverName,unsigned char info_type,SETTING_ITEM1 **pt,unsigned short *pt_nums, char* err);

   void      modifySetting(PCMD_GEN pCmd);
   void      modifyYb(PCMD_GEN pCmd);
   void      modifyParam(PCMD_GEN pCmd);



   //��ñ�����Ϣ
   void       getRealay(PCMD_GEN pCmd);
   //��ʼ��ģ��
   void      initModel(PCMD_GEN pCmd,t_bool bFlag = true);

   //����scl
   void      decodeScl(PCMD_GEN pCmd);


   //�ַ������Ƿ���ָ�������ݼ�����
   t_bool  haveDataSetType(PCMD_GEN pCmd,t_char *str,t_char *desc);
   //�����������ͻ���ļ���
   void    getFileName(PCMD_GEN pCmd,t_char *fileName);
   //����װ�õ�ʵʼ����־
   void    setEquipInitFlag(PCMD_GEN pCmd,PEQU_PARAM pEqu,t_bool flag);
   //�����ٻ��������������
   t_short getUpdateCode(PCMD_GEN pCmd);


   t_bool getPointDesc(t_char *serverName,t_char *ref,t_bool bBoolFlag,t_char *desc,t_char *state);

  

   //scd���ü��
    t_bool  getAttribId(t_uint8 cmdCode,t_bool &bSv,t_uint8 &attribId);     //����������attribid��bSv
	PIED   getScdIED(QString equName);									    //����װ�������scd����Ӧ��װ��ָ�� 
	t_uint16 getChnNum(PIED pIed,t_bool bSv,t_uint8 attribId);                        //����attribId���ͨ������



	 //ģ�ͼ��  
    t_short   m_settingCount;                 //��ֵ���������
	t_short   m_settingRefCount;
	t_short   m_settingDescCount;             //��ֵ�����������
	t_short   m_settingMaxCount;              //��ֵ�����ֵ�����
	t_short   m_settingMinCount;              //��ֵ����Сֵ�����
	t_short   m_settingStepCount;             //��ֵ���������
	t_short   m_settingUnitCount;             //��ֵ��λ�����

	t_short   m_equipParamCount;               //װ�ò������������
	t_short   m_equipParamRefCount;
	t_short   m_equipParamDescCount;           //װ�ò��������������
	t_short   m_equipParamMaxCount;            //װ�ò������ֵ�����
	t_short   m_equipParamMinCount;            //װ�ò�����Сֵ�����
	t_short   m_equipParamStepCount;           //װ�ò������������
	t_short   m_equipParamUnitCount;           //װ�ò��������

	t_short   m_analogCount;                    //ң��ֵ���������
	t_short   m_analogRefCount;
	t_short   m_analogDescCount;                //ң��ֵ�����������
	t_short   m_analogMaxCount;                 //ң��ֵ���ֵ�����
	t_short   m_analogMinCount;                 //ң��ֵ��Сֵ�����
	t_short   m_analogStepCount;                //ң��ֵ���������
	t_short   m_analogUnitCount;                //ң��ֵ�����

	t_short   m_ybCount;                    //ң��ֵ���������
	t_short   m_ybRefCount;
	t_short   m_ybDescCount;                //ң��ֵ�����������

	t_short   m_swiCount;                    //ң��ֵ���������
	t_short   m_swiRefCount;
	t_short   m_swiDescCount;                //ң��ֵ�����������

	t_short   m_alarmCount;                    //ң��ֵ���������
	t_short   m_alarmRefCount;
	t_short   m_alarmDescCount;                //ң��ֵ�����������

	t_short   m_eventCount;                    //ң��ֵ���������
	t_short   m_eventRefCount;
	t_short   m_eventDescCount;                //ң��ֵ�����������
    void      initModelCheckVars();
    void      modelCheck(PCMD_GEN pCmd);
    void      compSetting(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compSetting1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compEquipParam(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compEquipParam1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compAnalog(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compAnalog1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compYb(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compYb1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compSwi(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compSwi1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compAlarm(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compAlarm1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compEvent(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );
    void      compEvent1(PCMD_GEN pCmd,ClientDSE_ScdManager &scd,QString cmdName,SETTING_ITEM1 *scl_item,t_ushort  &scl_item_nums );

    void      compErrCount(PCMD_GEN pCmd,t_bool bFileFlag = true);

	
};


#endif // CMDTHREAD_H
