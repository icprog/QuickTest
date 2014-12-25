#ifndef CMDTHREAD_H
#define CMDTHREAD_H

#include <QObject>
#include "clientdatadefine.h"
#include "clientxmlimpl.h" 
#include "autotest_global.h"
#include "clientscdheader.h"
#include "clientscdphase.h"



//比较项
typedef struct _COMP_ITEM1
{
	_COMP_ITEM1()
	{
		memset(this,0,sizeof(_COMP_ITEM1));
	}
	char  file1_desc[256];
	char  dync_desc[256];

}COMP_ITEM1,*PCOMP_ITEM1;

//比较结果
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

   QList< PCMD_GEN >     m_cmdList;              //命令队列表

   bool m_isShowDownDlg;


signals:

	void resultData_signals(CMD_GEN *pCmd);


private:
	

	void	  closeThread();

static	void  circleThread(void *pVoid);	               //巡检线程		  
	void      circleFun();

static  int  rpt_callback_fun(void* ptr, FAULT_REPORT *fault_report);

static t_int     progress_Hint(void* ptr, CMD_TYPE_DEF::CMD cmd);


    ClientSysLock  	  m_cmdLock;              //命令列表锁
	QList<PEQU_PARAM> m_equParamList;         //装置通信参数列表
	ClientDSE_ScdManager    m_scd;

	char      m_cfgDir[MAX_NAMELEN];               //配置文件目录
	char      m_dataDir[MAX_NAMELEN];              //数据文件目录

	t_bool    m_bReadScd;

	bool      m_exit;
   _HANDLE    m_handle;					   //巡检线程的句柄
   bool       m_trheadExt;
   t_bool     m_winFlag;
   t_bool     m_sclFlag ;

   DSE_XmlSaxHandler  xml;

   t_uint8    m_curDecodeSclType;
   
   t_char     dataSetList[MAX_DATASET_NUMS][MAX_NAMELEN];   //最大50个数集
   t_int      m_dataSetNums ;

   void       initVars( );

   void       dealCmd();
   void       sendCmd(PCMD_GEN pCmd);


   //连接服务器
   t_bool       connectServer(PCMD_GEN pCmd);
   void         destConnect(PCMD_GEN pCmd);

   //动态获得点表
   unsigned int  get_dyn_pt(t_char *serverName,unsigned char info_type,SETTING_ITEM1 **pt,unsigned short *pt_nums, char* err);

   void      modifySetting(PCMD_GEN pCmd);
   void      modifyYb(PCMD_GEN pCmd);
   void      modifyParam(PCMD_GEN pCmd);



   //获得保护信息
   void       getRealay(PCMD_GEN pCmd);
   //初始化模型
   void      initModel(PCMD_GEN pCmd,t_bool bFlag = true);

   //解析scl
   void      decodeScl(PCMD_GEN pCmd);


   //字符串中是否有指定的数据集类型
   t_bool  haveDataSetType(PCMD_GEN pCmd,t_char *str,t_char *desc);
   //根据命令类型获得文件名
   void    getFileName(PCMD_GEN pCmd,t_char *fileName);
   //设置装置的实始化标志
   void    setEquipInitFlag(PCMD_GEN pCmd,PEQU_PARAM pEqu,t_bool flag);
   //根据召唤命令获得命令代码
   t_short getUpdateCode(PCMD_GEN pCmd);


   t_bool getPointDesc(t_char *serverName,t_char *ref,t_bool bBoolFlag,t_char *desc,t_char *state);

  

   //scd配置检查
    t_bool  getAttribId(t_uint8 cmdCode,t_bool &bSv,t_uint8 &attribId);     //根据命令获得attribid和bSv
	PIED   getScdIED(QString equName);									    //根据装置名获得scd中相应的装置指针 
	t_uint16 getChnNum(PIED pIed,t_bool bSv,t_uint8 attribId);                        //根据attribId获得通道总数



	 //模型检查  
    t_short   m_settingCount;                 //定值个数不相等
	t_short   m_settingRefCount;
	t_short   m_settingDescCount;             //定值描述符不相等
	t_short   m_settingMaxCount;              //定值描最大值不相等
	t_short   m_settingMinCount;              //定值描最小值不相等
	t_short   m_settingStepCount;             //定值步长不相等
	t_short   m_settingUnitCount;             //定值单位不相符

	t_short   m_equipParamCount;               //装置参数个数不相等
	t_short   m_equipParamRefCount;
	t_short   m_equipParamDescCount;           //装置参数描述符不相等
	t_short   m_equipParamMaxCount;            //装置参数最大值不相等
	t_short   m_equipParamMinCount;            //装置参数最小值不相等
	t_short   m_equipParamStepCount;           //装置参数步长不相等
	t_short   m_equipParamUnitCount;           //装置参数不相符

	t_short   m_analogCount;                    //遥测值个数不相等
	t_short   m_analogRefCount;
	t_short   m_analogDescCount;                //遥测值描述符不相等
	t_short   m_analogMaxCount;                 //遥测值最大值不相等
	t_short   m_analogMinCount;                 //遥测值最小值不相等
	t_short   m_analogStepCount;                //遥测值步长不相等
	t_short   m_analogUnitCount;                //遥测值不相符

	t_short   m_ybCount;                    //遥测值个数不相等
	t_short   m_ybRefCount;
	t_short   m_ybDescCount;                //遥测值描述符不相等

	t_short   m_swiCount;                    //遥测值个数不相等
	t_short   m_swiRefCount;
	t_short   m_swiDescCount;                //遥测值描述符不相等

	t_short   m_alarmCount;                    //遥测值个数不相等
	t_short   m_alarmRefCount;
	t_short   m_alarmDescCount;                //遥测值描述符不相等

	t_short   m_eventCount;                    //遥测值个数不相等
	t_short   m_eventRefCount;
	t_short   m_eventDescCount;                //遥测值描述符不相等
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
