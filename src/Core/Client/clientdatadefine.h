#ifndef CLEINTDATADEFINE_H
#define CLEINTDATADEFINE_H

#define MAX_NAMELEN				255/*65 */       //���ֳ���
#define MAX_STRLEN				30        //���ֳ���
#define MAX_STRLEN1				50        //���ֳ���
#define MAX_STRLEN2				100        //���ֳ���
#define MAX_CHARLEN				100         //�ַ�������
#define MAX_IPADDRLEN			16         //IP��ַ����
#define MAX_MACADDRLEN			18         //mac��ַ����
#define MAX_IPNUM               5          //���ڵ������Ŀ
#define MAX_SETTING_NUMS        300      //��ֵ��������
#define MAX_PARAM_NUMS          300      //Jװ�ò���
#define MAX_SAMPLE_NUMS         300      //ң��ֵ��������
#define MAX_YB_NUMS             300       //ѹ��������� 
#define MAX_SWI_NUMS            300      //ң�ŵ������� 
#define MAX_ALARM_NUMS          1000       //�澯������
#define MAX_EVENT_NUMS          1000      //�¼�������
#define MAX_RCD_NUMS            10         //¼��������
#define MAX_CHARLEN1			30         //�ַ�������
#define GSMV_TYPE_GOOSE		    1				 //������ϢΪGOOSE
#define GSMV_TYPE_SMV		    2				//������ϢΪSMV
#define MAX_DATASET_NUMS        120        //���ݼ���������
#define MAX_ITEM_DATASET_NUMS   30          //ÿ��������ݼ������� ���綨ֵ��ң�� 

#include <stdio.h>
#include <vector>
#include "autotest_global.h"

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtGui/QStandardItem>

#define _DEBUG_	       1		//���԰�
#define _RELEASE_	   2		//���а� 
#define _DEBUG_MSG     3        //��ӡ���� 
#define _DEBUG_STR     4        //��ӡ�ַ� 


#define DATETIMEFORMAT			"yyyy-MM-dd HH:mm:ss"
#define DATETIMEFORMAT1		    "dd/MM/yyyy,HH:mm:ss"
#define DATETIMEFORMAT2		    "yyyy-MM-dd_HH-mm-ss"
#define DATETIMEFORMAT_ZZZ		"yyyy-MM-dd HH:mm:ss.zzz"

#if defined(WIN32)  
	#include <windows.h>
	#include <algorithm>
    #include <process.h>    // _beginthread, _endthread 
	
	typedef  HANDLE _HANDLE;
	typedef  void (*ptrFun)( void*);
	#define mySleep(msec) ::Sleep(msec)
#else  
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <error.h>
	#include <errno.h>
	#include <limits.h>
	#include <stdlib.h>
	#include <fcntl.h>
	#include <linux/watchdog.h>
	#include <net/if.h>
	#include <linux/sockios.h>
	#include <sys/ioctl.h>
	#include <string.h>

	typedef  pthread_t _HANDLE; 
	typedef  void* (*ptrFun)( void*);  
    #define mySleep(msec) ::usleep(1000*msec)
#endif //WIN32



//�����������Ͷ���
typedef bool					t_bool;
typedef char					t_int8;
typedef char					t_char;
typedef unsigned char			t_uint8;
typedef unsigned char			t_uchar;
typedef short					t_int16;
typedef short					t_short;
typedef short					t_wchar;
typedef unsigned short			t_uint16;
typedef unsigned short			t_ushort;
typedef long					t_int32;
typedef int 					t_int;
typedef unsigned long			t_uint32;
typedef unsigned int			t_uint;
typedef long					t_long;
typedef unsigned long			t_ulong;
#if defined(WIN32)  
typedef __int64					t_int64; 
typedef unsigned __int64	    t_uint64;  
#else
typedef long long		        t_int64;
typedef unsigned long long	    t_uint64;
#endif// WIN32

typedef float			        t_float;
typedef double			        t_double;

typedef void*			        t_handle;


#define  TY_SYS                1            //ϵͳ�㣬������
#define  TY_STA                2            //վ�㣬��վ
#define  TY_EQP                3            //װ�ò㣬��װ��

#define  EQU_PROTECT           1            //����
#define  EQU_RECORD            2            //¼����
#define  DATA_LEN              1024

#define  DS_SETTING           "Setting"        //������ֵ���ݼ�

#define  DS_EQU_PARAM         "dsParameter"    //װ�ò��� 

#define  DS_RELAY_AIN         "dsRelayAin"     //ģ����  //dsAin
#define  DS_AIN               "dsAin"          //ģ����  //dsAin
#define  DS_URCB_RELAY_AIN    "urcbRelayAin"
#define  DS_UBRCB_AIN         "urcbAin"        //ң��

#define  DS_RELAY_DIN         "dsRelayDin"     //����ң�����ݼ� dsdin
#define  DS_DIN               "dsDin"     //����ң�����ݼ� dsdin
#define  DS_BRCB_DIN           "brcbDin"
#define  DS_BRCB_RELAY_DIN     "brcbRelayDin"

#define  DS_TRIP_INFO         "dsTripInfo"     //�����¼����ݼ�
#define  DS_BRCB_TRIP_INFO    "brcbTripInfo"   

#define  DS_RELAY_ENA         "dsRelayEna"     //����ѹ�����ݼ�
#define  DS_BRCB_REALY_ENA    "brcbRelayEna"
#define  DS_ENA               "dsEna"     //����ѹ�����ݼ�

#define  DS_WARNING           "dsWarning"      //�澯�ź�
#define  DS_ALARM             "dsAlarm"        //�澯
#define  DS_BRCB_ALARM        "brcbAlarm"
#define  DS_BRCB_WARNING       "brcbWarning"

#define  DS_RELAYREC          "dsRelayRec"        //���ϼ�
#define  DS_BRCB_REALY_RECV    "brcbRelayRecv"




//��ν����ṹ����������������ģ���еı���ΪIEC61850��Լ�е��߼��ڵ㣻
//����������ṹ��������˵���������ģ���еı���ΪIEC61850��Լ�еĹ���Լ����
//Ŀǰ�󲿷ֵķ�������Ϊ����ṹ�������������������Ϊ֧���ϰ汾�ķ�������
//���ӿں��������ֲ�εķ��������ṩ֧�֣��������������ʱ��ָ�������ӷ������Ĳ���������������������
#define  TWO_LAYER_SERVER      2            //����ṹ������
#define  THREE_LAYER_SERVER    3            //����ṹ������



#define REGCFG_NAME          ("reg.dll")
#define REGCFG1_NAME         ("reg1.dll")


#define SYSCFG       ("syscfg.xml") 
#define GSMVCFG      ("gsmvcfg.xml")
#define EQPIPCFG     ("eqpipcfg.xml")
#define EQPPTCFG     ("eqpptcfg.xml")



//������������
typedef struct _ENVIRNODE
{
	_ENVIRNODE()
	{
		memset(cfgDir,0x00,MAX_NAMELEN);
		memset(dataDir,0x00,MAX_NAMELEN);
		memset(proDir,0x00,MAX_NAMELEN);
		memset(sclPath,0x00,MAX_NAMELEN);
		memset(waveFileName,0x00,MAX_NAMELEN);
		eventCount = 0;
		circleInterval = 0;
		bBeckonWave = false;
		bSaveToFile = false;

	}
	~_ENVIRNODE()
	{
	
	}
 
	char   cfgDir[MAX_NAMELEN];               //�����ļ�Ŀ¼
	char   dataDir[MAX_NAMELEN];              //�����ļ�Ŀ¼
	char   proDir[MAX_NAMELEN];               //�����ļ�Ŀ¼
	char   sclPath[MAX_NAMELEN];
	char   waveFileName[MAX_NAMELEN] ;        //�����ļ���
	int    eventCount;                        //��ʾ�¼�����Ŀ��
	int    circleInterval;					  //Ѳ��ʱ����
	bool   bBeckonWave;                       //�����ٻ�����
	bool   bSaveToFile;                       //���浽�ļ�



}ENVIRNODE,*PENVIRNODE;


	//��һ�û�
typedef struct _SINGLEUSER
{

	_SINGLEUSER()
	{
		memset(name,0x00,MAX_NAMELEN);
		memset(password,0x00,MAX_STRLEN);
		memset(grp,0x00,MAX_NAMELEN);
	}

	~_SINGLEUSER()
	{

	}
	char name[MAX_NAMELEN];
	char password[MAX_STRLEN] ;
	char grp[MAX_NAMELEN];

}SINGLEUSER,*PSINGLEUSER;

typedef struct _USERGRP
{
	_USERGRP()
	{
		memset(grpName,0x00,MAX_NAMELEN);
	}

	char grpName[MAX_NAMELEN];

}USERGRP,*PUSERGRP;

//�û�����
typedef struct _USERNODE
{
	_USERNODE()
	{
		
	}
	~_USERNODE()
	{
	
	}
	std::vector<PSINGLEUSER> userList;   //�û��б�
	std::vector<PUSERGRP>   userGrpList; //�û����б�
	SINGLEUSER   currebtUser;                //��ǰ�û�
}USERNODE,*PUSERNODE;

#define COMP_ITEM    1
#define COMP_RESULT  2


typedef struct _SCL_PATH
{
	_SCL_PATH()
	{
		memset(scl_file_name,0x00,MAX_CHARLEN);
		memset(ied_name,0x00,MAX_STRLEN1);
		memset(access_point_name,0x00,MAX_STRLEN1);
		enabled= 0;
	}

   t_char scl_file_name[MAX_CHARLEN];
   t_char ied_name[MAX_STRLEN1];
   t_char access_point_name[MAX_STRLEN1];

   t_bool enabled;

}SCL_PATH,*PSCL_PATH;

//װ�ýڵ�
typedef struct _EQUIPNODE
{
	_EQUIPNODE()
	{
		memset(name,0x00,MAX_NAMELEN);
		memset(ip,0x00,MAX_IPADDRLEN);
		port = 102;
		id = 0;
		model = EQU_PROTECT;
	}
	~_EQUIPNODE()
	{
	
	}

	char   name[MAX_NAMELEN];
	char   ip[MAX_IPADDRLEN];
	int    port;
	int    id;
	int    type;
	int    model;

	SCL_PATH scl;

}EQUIPNODE,*PEQUIPNODE;


//վ�ڵ�
typedef struct _STANODE
{
	_STANODE()
	{
		memset(name,0x00,MAX_NAMELEN);
	}
	~_STANODE()
	{
	
	}
	std::vector<PEQUIPNODE> iedList;
	char   name[MAX_NAMELEN];
	char   defaultEqu[MAX_NAMELEN];

}STANODE,*PSTANODE;


//����
typedef struct _DOMAINNODE
{
	_DOMAINNODE()
	{
		memset(name,0x00,MAX_NAMELEN);
	}
	~_DOMAINNODE()
	{
	
	}
	std::vector<PSTANODE> staList;
	char   name[MAX_NAMELEN];

}DOMAINNODE,*PDOMAINNODE;


//ϵͳ
typedef struct _SYSNODE
{
	_SYSNODE()
	{
		memset(name,0x00,MAX_NAMELEN);
		memset(defaultSta,0x00,MAX_NAMELEN);
		memset(defaultEqu,0x00,MAX_NAMELEN);
		memset(scl_fileName_modelCheck,0x00,MAX_NAMELEN);
	}
	~_SYSNODE()
	{
	
	}
	std::vector<PDOMAINNODE> domainList;
	char   name[MAX_NAMELEN];
	char   defaultSta[MAX_NAMELEN];
	char   defaultEqu[MAX_NAMELEN];
	t_char scl_fileName_modelCheck[MAX_NAMELEN];

}SYSNODE,*PSYSNODE;




//������
enum CMDCODE
{
	 CMD_CONNECT            = 1,              //����
	 CMD_DISTCNNECT         = 2,              //�Ͽ�
	 CMD_BECKON_FILE_LIST   = 3,		      //�ٻ��ļ��б�
	 CMD_BECKON_FILE        = 4,		      //�ٻ��ļ�
     CMD_INIT_MODEL         = 5,		      //��ʼ��ģ��
	 CMD_INIT_SETTING       = 6,              //��ʼ����ֵ
	 CMD_INIT_SAMPLE        = 7,              //��ʼ��ģ����
	 CMD_INIT_YB            = 8,              //��ʼ��ѹ��
     CMD_INIT_SWITCH        = 9,              //��ʼ��������
	 CMD_INIT_EVET          = 10,              //��ʼ���¼�
	 CMD_INIT_ALARM         = 11,              //��ʼ���澯
	 CMD_INIT_WARING        = 12,              //��ʼ���澯
	 CMD_INIT_RCD           = 13,              //��ʼ¼��
	 CMD_INIT_OTHER         = 14,              //��ʼ������
	 CMD_BECKON_SETTING     = 15,              //�ٻ���ֵ	
	 CMD_BECKON_SAMPLE      = 16,              //ģ����
	 CMD_BECKON_YB          = 17,              //ѹ��
	 CMD_BECKON_SWITCH      = 18,             //������
	 CMD_BECKON_EVENT       = 19,			  //�¼� 
	 CMD_REMOTE_WAVE        = 20,             //ң��¼��
	 CMD_REMOTE_RESET       = 21,             //Զ������
	 CMD_REPORT             = 22,             //���ϱ���
	 CMD_EQU_PARAM          = 23,              //װ�ò���
	 CMD_INIT_PARAM         = 24,              //��ʼ��װ�ò���
	 CMD_UPDATE_PARAM       = 25,              //���²���

	 CMD_SET_RCD_LDO        = 50,             //����FH6000LD0,FH6000Rcd
	 CMD_UPDATE_SETTING     = 51,             //�ӱ��ض�ֵ�ļ��и������ݿ�
	 CMD_UPDATE_FILELIST    = 52,             //�ӱ����ļ��б��и������ݿ�
	 CMD_UPDATE_SAMPLE      = 53,
	 CMD_UPDATE_YB          = 54,
	 CMD_UPDATE_SWITCH      = 55,
	 CMD_UPDATE_EVENT       = 56,
     CMD_UPDATE_OTHER       = 57,

	 CMD_DESCODE_IP_SCL        = 70,            //����scl
	 CMD_PROGRESS_IP_HINT      = 71,           //����ָʾ
	 CMD_DESCODE_PT_SCL        = 72,            //����scl
	 CMD_PROGRESS_PT_HINT      = 73,           //����ָʾ
	 
	 CMD_SCD_CHECK             = 80,     

	 CMD_MODIFY_SETTING        = 81,  //��ֵ�޸�
	 CMD_MODIFY_YB             = 82,  //��ֵ�޸�
	 CMD_MODIFY_PARAM          = 83,  //װ�ò����޸�

	 CMD_UPDOWN_PARAM          = 84,
	 CMD_UPDOWN_SETTING        = 85,
	 CMD_UPDOWN_YB             = 86,

	 CMD_FILE_ATTRIB           = 87,  //�ļ�����

	 CMD_SCD_CHECK_FINISH      = 88,  


};


//������
enum CMDRESLT
{
	 CMD_RESULT_SUCCESS         = 0,		      //����سɹ�
     CMD_RESULT_RESULT_FAILD    = 1,		      //�����ʧ��
	 CMD_RESULT_BEGINPROCESS    = 2,		      //���ʼ����
	 CMD_RESULT_PROCESSING      = 3,		      //�������ڴ���
	 CMD_RESULT_ENDBEGINPROCESS = 4,		      //�����������
	 CMD_RESULT_HINT            = 5,		      //������ʾ��Ϣ

};


//ģ�ͽڵ�����
enum MODEL_NODE_TYPE
{ 
        MNT_ROOT			= 1,
        MNT_IED				= 2,
        MNT_DATASET_ROOT    = 3,
        MNT_RPT_ROOT	    = 4,
        MNT_LD			    = 10,
        MNT_LN				= 11,
        MNT_DATA			= 12,
        MNT_LEAF			= 13,
        MNT_DATASET			= 20,
        MNT_DATASET_DATA	= 21,
        MNT_RPT				= 30,
		MNT_RPT1			= 31,
        MNT_RPT_DATA		= 32,
        MNT_ERROR			= 100
};




//ϵͳ
typedef struct _CMD_GEN
{
	_CMD_GEN()
	{
		cmdCode = 0;
		memset(destStr,0x00,sizeof(MAX_NAMELEN));
		memset(serverName,0x00,sizeof(MAX_NAMELEN));
		memset(data,0x00,DATA_LEN);
		memset(resultDesc,0x00,sizeof(resultDesc));
		memset(equpName,0x00,sizeof(equpName));
		
	}
	~_CMD_GEN()
	{
	
	}


	t_short cmdCode;                      //������
	t_char  serverName[MAX_NAMELEN];      //����������
	t_char  equpName[MAX_NAMELEN];
	SCL_PATH sclPath;                     //scl·��
	t_char  destStr[MAX_NAMELEN];         //�����ַ�

	 t_uint8 pos;                            //1����Ƚ��2����ȽϽ��

	
	t_uint8 result;                       //������
	t_char  resultDesc[MAX_NAMELEN];      //���������
	t_char  data[DATA_LEN];                       //����ָ��

}CMD_GEN,*PCMD_GEN;




typedef struct _SETTING_ITEM1
{
    _SETTING_ITEM1(){ memset(this, 0, sizeof(_SETTING_ITEM1)); }
    char		ref[65];       //���ݲ���
    char		desc[65];      //��ֵ������dU
    int			data_type;      //��������
    int			data_len;       //����
    int			data_size;      //���ݴ�С
	char        val_Str[20];    //����ֵ
	char        down_val_Str[20];    //����ֵ
	char        max_val_str[20];//���ֵ
	char        min_val_str[20];//��С
	char        step_str[20];//��С
    char		unit[10];       //��λ

}SETTING_ITEM1, *PSETTING_ITEM1;

typedef struct _MODIFY_ITEM
{

	SETTING_ITEM1 item;
	QStandardItem *pStanItem;

}MODIFY_ITEM,*PMODIFY_ITEM;



//ϵͳ
typedef struct _EQU_PARAM
{
	_EQU_PARAM()
	{
		memset(serverName,0x00,sizeof(MAX_NAMELEN));
		memset(equpName,0x00,sizeof(equpName));
		memset(ptFileName,0x00,sizeof(ptFileName));
		bConnect = false;
		bInitSetting = false;
		bInitParam = false;
		bInitSwitch = false;
		bInit_Yb = false;
		bInitSample = false;
		bInit_Event = false;
		bInitModel  = false;
		bResetConnect= false;

		setting_Item = 0;
		setting_Count = 0;

		param_Item = NULL;
		paramCount = 0;
		
		sample_Item = NULL;
		sample_Count = 0;

		yb_Item = NULL;
		yb_Count = 0;

	
		swi_Item = NULL;
		swi_Count = 0;

		event_Item = NULL;
		event_Count = 0;

		edit_setting_item = NULL;
		edit_setting_count = 0;
		edit_yb_item = NULL;
		edit_yb_count = 0;

		edit_param_item = NULL;
		edit_param_count = 0;

		down_setting_Item = NULL;
		down_setting_Count = 0;
		down_param_Item = NULL;
		down_paramCount = 0;
		down_ybItem = NULL;
		down_ybCount = 0;

		isBeckonSetting = false;
		isBeckonParam = false;
		isBeckonYb = false;

		setting_dataSet = NULL;
		setting_dataSet_nums = 0;

		param_dataSet = NULL;
		param_dataSet_nums = 0;

		sampledataSet = NULL;
		sample_dataSet_nums= 0;

		ybdataSet = NULL;
		yb_dataSet_nums = 0;
		swidataSet = NULL;
		swi_dataSet_nums = 0;
	}
	~_EQU_PARAM()
	{
	
		delete[]setting_Item;
		delete[]param_Item;
		delete[]sample_Item;
		delete[]yb_Item;
		delete[]swi_Item;
		
		delete[]event_Item;
		

		if (edit_setting_item)
			delete[] edit_setting_item;
        
		if (edit_yb_item)
			delete[] edit_yb_item;
		if (edit_param_item)
			delete[] edit_param_item;

		if (down_setting_Item)
			delete[] down_setting_Item;
		if (down_param_Item)
			delete[] down_param_Item;
		if (down_ybItem)
			delete[] down_ybItem;

		if (setting_dataSet)
		{
			for (int i = 0; i < setting_dataSet_nums; i++)
			{
				free(setting_dataSet[i]);
			}
			free(setting_dataSet);

		}

		if (param_dataSet)
		{
			for (int i = 0; i < param_dataSet_nums; i++)
			{
				free(param_dataSet[i]);
			}
			free(param_dataSet);

		}

		if (sampledataSet)
		{
			for (int i = 0; i < sample_dataSet_nums; i++)
			{
				free(sampledataSet[i]);
			}
			free(sampledataSet);

		}

		if (ybdataSet)
		{
			for (int i = 0; i < sample_dataSet_nums; i++)
			{
				free(ybdataSet[i]);
			}
			free(ybdataSet);

		}

		if (swidataSet)
		{
			for (int i = 0; i < swi_dataSet_nums; i++)
			{
				free(swidataSet[i]);
			}
			free(swidataSet);

		}

	


	}

	t_char  serverName[MAX_NAMELEN];      //����������
	t_char  equpName[MAX_NAMELEN];
	t_bool  bConnect;                     //���ӶϿ�
	t_bool  bResetConnect;
	t_bool  bInitSetting;                 //��ʼ����ֵ��
	t_bool  bInitParam;                    //��ʼ��װ�ò���
	t_bool  bInitSample;                  //��ʼ��ģ����
	t_bool  bInitSwitch;                  //��ʼ��������
	t_bool  bInit_Yb ;                    //��ʼ��ѹ��
	t_bool  bInit_Event;                  //��ʼ���¼�
	t_bool  bInitModel;                   //��ʼ��ģ��

	SCL_PATH sclPath;                     //scl·��
	t_char   ptFileName[256];

	
	SETTING_ITEM1 *setting_Item;
    t_ushort         setting_Count;
	t_bool         isBeckonSetting;
	t_char          **setting_dataSet;
	int             setting_dataSet_nums;

	ITEM_PARAM *down_setting_Item;
	t_ushort       down_setting_Count;



	
	SETTING_ITEM1 *param_Item;
    t_ushort       paramCount;
	t_char         **param_dataSet;
	t_int          param_dataSet_nums;
	t_bool         isBeckonParam;
	ITEM_PARAM     *down_param_Item;
	t_ushort       down_paramCount;




	SETTING_ITEM1 *sample_Item;
	t_ushort         sample_Count;
	t_char         **sampledataSet;
	t_int         sample_dataSet_nums;

	SETTING_ITEM1 *yb_Item;
	t_ushort         yb_Count;
	t_bool           isBeckonYb;
	t_char          **ybdataSet;
	t_int           yb_dataSet_nums;
	ITEM_PARAM    *down_ybItem;
	t_ushort        down_ybCount;


	SETTING_ITEM1 *swi_Item;
	t_ushort         swi_Count;
	t_char          **swidataSet;
	t_int           swi_dataSet_nums;



	SETTING_ITEM1 *event_Item;
	t_ushort         event_Count;


	SETTING_ITEM1  *edit_setting_item;
	t_ushort        edit_setting_count;

	SETTING_ITEM1  *edit_yb_item;
	t_ushort        edit_yb_count;

	SETTING_ITEM1  *edit_param_item;
	t_ushort        edit_param_count;

	
}EQU_PARAM,*PEQU_PARAM;




typedef struct _C61850_REPORT
{
    _C61850_REPORT()
    {
        strcpy(t, "1970-01-01 00:00:00.000000");
		strcpy(ref,"");
        strcpy(desc, "");
		strcpy(strVal,"");

  
    }
	t_char   ref[65];
	t_char   t[64];           //ʱ��
    t_char   desc[50];       //����
	t_char   strVal[100];

}C61850_REPORT, *PC61850_REPORT;



typedef struct _MODEL_ITEM
{
	_MODEL_ITEM(){ memset(this, 0, sizeof(_MODEL_ITEM)); }
    t_char			name[MAX_NAMELEN];
	t_char			ref[MAX_NAMELEN];		
	int 			type;											
	int				len;						
	int				size;	
}MODEL_ITEM, *PMODEL_ITEM;


class ClientSysLock
{
public:
    
#if defined(WIN32)
	
	ClientSysLock() { ::InitializeCriticalSection(&m_sec);};
	~ClientSysLock(){::DeleteCriticalSection(&m_sec);};
	void lock(){  EnterCriticalSection(&m_sec);};
	void unlock(){ LeaveCriticalSection(&m_sec);}

private:
    CRITICAL_SECTION  m_sec;

#else

	 ClientSysLock()   { m_mutex= (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;pthread_mutex_init(&m_mutex, NULL);};
	~ClientSysLock()   { pthread_mutex_destroy(&m_mutex);};
	void lock()  { pthread_mutex_lock(&m_mutex);};
	void unlock(){ pthread_mutex_unlock(&m_mutex);};

private:
    pthread_mutex_t  m_mutex;

#endif

};

void getTypeStr(int data_type ,t_char *typeStr);
void getUnit(int unitOrd,char *unitStr);
void getMultiplier(int multiOrd,char *multiStr);



#endif // DATADEFINE_H
