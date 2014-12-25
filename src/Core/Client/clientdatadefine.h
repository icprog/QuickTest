#ifndef CLEINTDATADEFINE_H
#define CLEINTDATADEFINE_H

#define MAX_NAMELEN				255/*65 */       //名字长度
#define MAX_STRLEN				30        //名字长度
#define MAX_STRLEN1				50        //名字长度
#define MAX_STRLEN2				100        //名字长度
#define MAX_CHARLEN				100         //字符串长度
#define MAX_IPADDRLEN			16         //IP地址长度
#define MAX_MACADDRLEN			18         //mac地址长度
#define MAX_IPNUM               5          //网口的最大数目
#define MAX_SETTING_NUMS        300      //定值的最大个数
#define MAX_PARAM_NUMS          300      //J装置参数
#define MAX_SAMPLE_NUMS         300      //遥测值的最大个数
#define MAX_YB_NUMS             300       //压板的最大个数 
#define MAX_SWI_NUMS            300      //遥信的最大个数 
#define MAX_ALARM_NUMS          1000       //告警最大个数
#define MAX_EVENT_NUMS          1000      //事件最大个数
#define MAX_RCD_NUMS            10         //录波最大个数
#define MAX_CHARLEN1			30         //字符串长度
#define GSMV_TYPE_GOOSE		    1				 //配置信息为GOOSE
#define GSMV_TYPE_SMV		    2				//配置信息为SMV
#define MAX_DATASET_NUMS        120        //数据集的最大个数
#define MAX_ITEM_DATASET_NUMS   30          //每个项的数据集最大个数 ，如定值，遥测 

#include <stdio.h>
#include <vector>
#include "autotest_global.h"

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtGui/QStandardItem>

#define _DEBUG_	       1		//调试版
#define _RELEASE_	   2		//发行版 
#define _DEBUG_MSG     3        //报印报文 
#define _DEBUG_STR     4        //打印字符 


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



//基本数据类型定义
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


#define  TY_SYS                1            //系统层，即区域
#define  TY_STA                2            //站层，即站
#define  TY_EQP                3            //装置层，即装置

#define  EQU_PROTECT           1            //保护
#define  EQU_RECORD            2            //录波器
#define  DATA_LEN              1024

#define  DS_SETTING           "Setting"        //保护定值数据集

#define  DS_EQU_PARAM         "dsParameter"    //装置参数 

#define  DS_RELAY_AIN         "dsRelayAin"     //模拟量  //dsAin
#define  DS_AIN               "dsAin"          //模拟量  //dsAin
#define  DS_URCB_RELAY_AIN    "urcbRelayAin"
#define  DS_UBRCB_AIN         "urcbAin"        //遥测

#define  DS_RELAY_DIN         "dsRelayDin"     //保护遥信数据集 dsdin
#define  DS_DIN               "dsDin"     //保护遥信数据集 dsdin
#define  DS_BRCB_DIN           "brcbDin"
#define  DS_BRCB_RELAY_DIN     "brcbRelayDin"

#define  DS_TRIP_INFO         "dsTripInfo"     //保护事件数据集
#define  DS_BRCB_TRIP_INFO    "brcbTripInfo"   

#define  DS_RELAY_ENA         "dsRelayEna"     //保护压板数据集
#define  DS_BRCB_REALY_ENA    "brcbRelayEna"
#define  DS_ENA               "dsEna"     //保护压板数据集

#define  DS_WARNING           "dsWarning"      //告警信号
#define  DS_ALARM             "dsAlarm"        //告警
#define  DS_BRCB_ALARM        "brcbAlarm"
#define  DS_BRCB_WARNING       "brcbWarning"

#define  DS_RELAYREC          "dsRelayRec"        //故障简报
#define  DS_BRCB_REALY_RECV    "brcbRelayRecv"




//所谓两层结构服务器，即服务器模型中的变量为IEC61850规约中的逻辑节点；
//而对于三层结构服务器来说，其服务器模型中的变量为IEC61850规约中的功能约束。
//目前大部分的服务器都为两层结构服务器，三层服务器仅为支持老版本的服务器。
//本接口函数对两种层次的服务器都提供支持，在与服务器连接时需指定所连接服务器的层数，否则不能正常工作。
#define  TWO_LAYER_SERVER      2            //两层结构服务器
#define  THREE_LAYER_SERVER    3            //三层结构服务器



#define REGCFG_NAME          ("reg.dll")
#define REGCFG1_NAME         ("reg1.dll")


#define SYSCFG       ("syscfg.xml") 
#define GSMVCFG      ("gsmvcfg.xml")
#define EQPIPCFG     ("eqpipcfg.xml")
#define EQPPTCFG     ("eqpptcfg.xml")



//环境变量设置
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
 
	char   cfgDir[MAX_NAMELEN];               //配置文件目录
	char   dataDir[MAX_NAMELEN];              //数据文件目录
	char   proDir[MAX_NAMELEN];               //程序文件目录
	char   sclPath[MAX_NAMELEN];
	char   waveFileName[MAX_NAMELEN] ;        //波形文件名
	int    eventCount;                        //显示事件的条目数
	int    circleInterval;					  //巡检时间间隔
	bool   bBeckonWave;                       //主动召唤波形
	bool   bSaveToFile;                       //保存到文件



}ENVIRNODE,*PENVIRNODE;


	//单一用户
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

//用户设置
typedef struct _USERNODE
{
	_USERNODE()
	{
		
	}
	~_USERNODE()
	{
	
	}
	std::vector<PSINGLEUSER> userList;   //用户列表
	std::vector<PUSERGRP>   userGrpList; //用户组列表
	SINGLEUSER   currebtUser;                //当前用户
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

//装置节点
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


//站节点
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


//区域
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


//系统
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




//命令码
enum CMDCODE
{
	 CMD_CONNECT            = 1,              //连接
	 CMD_DISTCNNECT         = 2,              //断开
	 CMD_BECKON_FILE_LIST   = 3,		      //召唤文件列表
	 CMD_BECKON_FILE        = 4,		      //召唤文件
     CMD_INIT_MODEL         = 5,		      //初始化模型
	 CMD_INIT_SETTING       = 6,              //初始化定值
	 CMD_INIT_SAMPLE        = 7,              //初始化模拟量
	 CMD_INIT_YB            = 8,              //初始化压板
     CMD_INIT_SWITCH        = 9,              //初始化开关量
	 CMD_INIT_EVET          = 10,              //初始化事件
	 CMD_INIT_ALARM         = 11,              //初始化告警
	 CMD_INIT_WARING        = 12,              //初始化告警
	 CMD_INIT_RCD           = 13,              //初始录波
	 CMD_INIT_OTHER         = 14,              //初始化其他
	 CMD_BECKON_SETTING     = 15,              //召唤定值	
	 CMD_BECKON_SAMPLE      = 16,              //模拟量
	 CMD_BECKON_YB          = 17,              //压板
	 CMD_BECKON_SWITCH      = 18,             //开关量
	 CMD_BECKON_EVENT       = 19,			  //事件 
	 CMD_REMOTE_WAVE        = 20,             //遥控录波
	 CMD_REMOTE_RESET       = 21,             //远方复归
	 CMD_REPORT             = 22,             //故障报告
	 CMD_EQU_PARAM          = 23,              //装置参数
	 CMD_INIT_PARAM         = 24,              //初始化装置参数
	 CMD_UPDATE_PARAM       = 25,              //更新参数

	 CMD_SET_RCD_LDO        = 50,             //设置FH6000LD0,FH6000Rcd
	 CMD_UPDATE_SETTING     = 51,             //从本地定值文件中更新数据库
	 CMD_UPDATE_FILELIST    = 52,             //从本地文件列表中更新数据库
	 CMD_UPDATE_SAMPLE      = 53,
	 CMD_UPDATE_YB          = 54,
	 CMD_UPDATE_SWITCH      = 55,
	 CMD_UPDATE_EVENT       = 56,
     CMD_UPDATE_OTHER       = 57,

	 CMD_DESCODE_IP_SCL        = 70,            //解析scl
	 CMD_PROGRESS_IP_HINT      = 71,           //进度指示
	 CMD_DESCODE_PT_SCL        = 72,            //解析scl
	 CMD_PROGRESS_PT_HINT      = 73,           //进度指示
	 
	 CMD_SCD_CHECK             = 80,     

	 CMD_MODIFY_SETTING        = 81,  //定值修改
	 CMD_MODIFY_YB             = 82,  //定值修改
	 CMD_MODIFY_PARAM          = 83,  //装置参数修改

	 CMD_UPDOWN_PARAM          = 84,
	 CMD_UPDOWN_SETTING        = 85,
	 CMD_UPDOWN_YB             = 86,

	 CMD_FILE_ATTRIB           = 87,  //文件属性

	 CMD_SCD_CHECK_FINISH      = 88,  


};


//命令结果
enum CMDRESLT
{
	 CMD_RESULT_SUCCESS         = 0,		      //命令返回成功
     CMD_RESULT_RESULT_FAILD    = 1,		      //命令返回失败
	 CMD_RESULT_BEGINPROCESS    = 2,		      //命令开始处理
	 CMD_RESULT_PROCESSING      = 3,		      //命令正在处理
	 CMD_RESULT_ENDBEGINPROCESS = 4,		      //命令结束处理
	 CMD_RESULT_HINT            = 5,		      //命令提示信息

};


//模型节点类型
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




//系统
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


	t_short cmdCode;                      //命令码
	t_char  serverName[MAX_NAMELEN];      //服务器名称
	t_char  equpName[MAX_NAMELEN];
	SCL_PATH sclPath;                     //scl路径
	t_char  destStr[MAX_NAMELEN];         //描述字符

	 t_uint8 pos;                            //1代表比较项，2代表比较结果

	
	t_uint8 result;                       //命令结果
	t_char  resultDesc[MAX_NAMELEN];      //命令返回描述
	t_char  data[DATA_LEN];                       //数据指针

}CMD_GEN,*PCMD_GEN;




typedef struct _SETTING_ITEM1
{
    _SETTING_ITEM1(){ memset(this, 0, sizeof(_SETTING_ITEM1)); }
    char		ref[65];       //数据参引
    char		desc[65];      //定值描述：dU
    int			data_type;      //数据类型
    int			data_len;       //长度
    int			data_size;      //数据大小
	char        val_Str[20];    //设置值
	char        down_val_Str[20];    //下载值
	char        max_val_str[20];//最大值
	char        min_val_str[20];//最小
	char        step_str[20];//最小
    char		unit[10];       //单位

}SETTING_ITEM1, *PSETTING_ITEM1;

typedef struct _MODIFY_ITEM
{

	SETTING_ITEM1 item;
	QStandardItem *pStanItem;

}MODIFY_ITEM,*PMODIFY_ITEM;



//系统
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

	t_char  serverName[MAX_NAMELEN];      //服务器名称
	t_char  equpName[MAX_NAMELEN];
	t_bool  bConnect;                     //连接断开
	t_bool  bResetConnect;
	t_bool  bInitSetting;                 //初始化定值项
	t_bool  bInitParam;                    //初始化装置参数
	t_bool  bInitSample;                  //初始化模拟量
	t_bool  bInitSwitch;                  //初始化开关量
	t_bool  bInit_Yb ;                    //初始化压板
	t_bool  bInit_Event;                  //初始化事件
	t_bool  bInitModel;                   //初始化模型

	SCL_PATH sclPath;                     //scl路径
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
	t_char   t[64];           //时间
    t_char   desc[50];       //描述
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
