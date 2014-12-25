#ifndef _SCDHEADER_H
#define _SCDHEADER_H

#include "clientdatadefine.h"

#define ATTRIB_SETTING			 20       //定值信息
#define ATTRIB_EQU_PARAM		 23       //装置参数信息
#define ATTIRB_RUN_SETTING_NONE  21       //运行定值区信息
#define ATTIRB_CUR_SETTING_NONE  22       //当前定值区信息

#define ATTRIB_SAMPLE			 10       //模拟量信息 

#define ATTRIB_SWI				 0        //开关量信息            
#define ATTRIB_YB				 1        //软压板信息
#define ATTRIB_YYB				 2        //硬压板信息

#define ATTRIB_ALARM			 30       //告警信息
#define ATTRIB_SELF_CHECK		 32       //自检信息

#define ATTRIB_EVENT             40       //事件信息

#define ATTRIB_RELAYREC          50       //故障简报

#define ATTRIB_REPORTCTRL        160     //报告控制块
#define ATTRIB_SETTINCTRL        161     //定值控制块

#define DECODE_COMMLAYER         0x01       //解析通讯层
#define DECODE_PT                0x02       //解析点表
#define DECODE_ALL               0x0f       //解析所有



	//goose数据类型
   typedef struct _GSDATA_TYPE
   {
	   _GSDATA_TYPE() { memset(this, 0, sizeof(_GSDATA_TYPE)); }

	   //goos开关量格式
	    enum _GSDATA_TYPE_ENUM
		 {
			GTE_STRVAL  =  0x01,      //strval
			GTE_Q       =  0x02,      //q
			GTE_T       =  0x04,	
		 };

		//值类型
		enum _GSDATA_VALUE_ENUM   //还应增加int8 int32 等
		 {
			GV_NONE            =  0,	  //未知
			GVE_BOOLEAN        =  1,      //BOOLEAN
			GVE_INT8U          =  2,      //INT8U
			GVE_INT32          =  3,      //INT32
			GVE_INT32U         =  4,      //INT32U
			GVE_Timestamp      =  5,      //Timestamp
			GVE_Check          =  6,      //Check
			GVE_FLOAT32        =  7,      //FLOAT32  
			GVE_Struct         =  8,      //Struct
			GVE_Enum           =  9,      //Enum
			GVE_Octet64        =  10,     //Octet64 
			GVE_Unicode255     =  11,     //Unicode255
			GVE_VisString255   =  12,     //VisString255
			GV_Dbpos           =  13,     //Dbpos 
			GV_Quality         =  14,     //Quality
			GV_INT8            =  15,     //INT8 
			GV_INT16           =  16,      //INT16
			GV_Sbpos           =  17       //单点
			
		 };

		
	   t_bool  bStruct;                            //是否是一个结构
	   t_uint8 fromatValue;                        //开关量格式,是strVal,stVal、q ,strVal、q、t 			 
	   t_uint8  type;						       //数据类型

   }GSDATA_TYPE,*PGSDATA_TYPE;


	//goose通道信息
	typedef struct _GSCHNINFO 
	{
		_GSCHNINFO() { memset(this, 0, sizeof(_GSCHNINFO)); }
		t_ushort    index;							//序号（1~N）
		t_char	    chnName[MAX_NAMELEN];			//通道名称
		t_char      datSetName[MAX_NAMELEN];		//数据对象名
		t_uint8     dataType;					    //数据类型
		t_uint8     dataSize;                      //数据宽度
		t_char      dataTypeStr[MAX_STRLEN];
		t_uchar     type;							//通道类型（保护动作/断路器位置、、、、）
		t_uchar     flag;							//通道标识（断A、、、）

		t_ushort    enable;							//0位：是否勾选，1位：是否双节点按单点处理

		//为模拟服务器增加
		t_char   strVal[MAX_STRLEN];
	}GSCHNINFO ,*PGSCHNINFO ;

	
	typedef struct _REPORTCONTROL
	{
		_REPORTCONTROL()
		{
			memset(this,0x00,sizeof(_REPORTCONTROL));

		}
		t_char     ld[MAX_CHARLEN];    //逻辑设备
		t_char     name[MAX_CHARLEN];
		t_char     datSet[MAX_CHARLEN];
		t_char     rptID[MAX_CHARLEN];
		t_uint8    max;        //报告的连接的个数

	}REPORTCONTROL,*PREPORTCONTROL;

	//goose控制块
	typedef struct _GOOSEINFO//双网考虑在什么地方标识
	{
		_GOOSEINFO()
		{
			customId = 0;
			ethIndex = 0;					      //网口编号,从1开始
			appid = 0;						  //APPID 0x0000
			memset(prefix,0x00,MAX_NAMELEN);
			memset(mac,0x00,MAX_CHARLEN);
		    confRev = 0;						  //1~N 数据集改变次数
			memset(gcRef,0x00,MAX_CHARLEN);
			memset(datSet,0x00,MAX_CHARLEN);
			memset(goId,0x00,MAX_CHARLEN);
			datCount = 0;						  //数据对象数目
			vlan = 0;
			enable = 0;						  //0位：是否检查数据对象数目 1位：是否检查confRev字段 2位：检查gcRef字段 3位：检查datSet字段 4位：需要暂态录波,5:是否判AB网
			t0 = 0;
			t1= 0;

		 }
		~_GOOSEINFO()
		{
			for (t_uint i = 0 ;i < chnList.size() ;i++)
			{	
			  PGSCHNINFO pGsChnInfo = chnList[i];
			  delete pGsChnInfo;
			}
			chnList.clear();
		}

		t_ushort  customId;                       //自定义序号 ，唯一    
		t_uchar   ethIndex;					      //网口编号,从1开始
		t_uint32  appid;						  //APPID 0x0000
		t_char    prefix[MAX_NAMELEN];			  //前缀
		t_char	  mac[MAX_CHARLEN];				  //MAC地址（01-0C-CD-04-01-01）
		t_uchar	  confRev;						  //1~N 数据集改变次数
		t_char	  gcRef[MAX_CHARLEN];			  //
		t_char	  datSet[MAX_CHARLEN];			  //
		t_ushort  datCount;						  //数据对象数目
		t_ushort  vlan;                               //vlan
		t_char    goId[MAX_CHARLEN];
		t_int     t0;                               //稳定条件下重传时间
		t_int     t1;                               //最短传输时间
		t_ushort  enable;						   //0位：是否检查数据对象数目 1位：是否检查confRev字段 2位：检查gcRef字段 3位：检查datSet字段 4位：需要暂态录波,5:是否判AB网,6：是是否检查goid
	
		t_uint8  attribId;                           //属性id ，定值，模拟量，开关量、告警，事件等信息
		REPORTCONTROL reportCtrl;                    //报告控制

		std::vector<GSCHNINFO*> chnList;
	}GOOSEINFO,*PGOOSEINFO;

	//smv通道信息
	typedef struct _SVCHNINFO //双AD在SV块中标识
	{
		_SVCHNINFO() { memset(this, 0, sizeof(_SVCHNINFO)); }

		t_ushort       index;							 //序号（1~N）
		t_ushort       remoteIndex;                      //对端序号 ,主要用于双AD 0代表没有双AD
		t_char         chnName[MAX_NAMELEN];			 //通道名称
		t_char         datSetName[MAX_NAMELEN];		     //数据对象名
		t_uchar        type;							 //通道类型（交流电流/交流电压/、、）
		t_uchar		   flag;							 //通道标识（A、B、C、N、、）
		t_float		   priRated;						 //一次额定值
		t_float	       secRated;						 //二次额定值
		t_float        priReferVal;						 //一次参考值（电压0.01 电流0.001）
		t_int		   referToSamp;						 //参考值对应采样值0x2d41 主要用于9-1,是一个基准值 如2041对于1000v 则2041/1000*收到的值
		t_ushort       overLaod;						 //过载倍数（电流：50 电压：2）
		t_float        sampLimit;						 //AD有效采样门槛（额定值5%）
		t_float		   twoADDiff;						 //双AD偏差（10%）
		int            freq;                              //工作频率  //????
		t_float        precipValue;                       //数据异常变大，门槛值

		t_float		   range;
		t_float		   blm;
		t_float		   smpMin;

		t_ushort enable;							   //0位：是否勾选，1位：是否采样无效告警

		//为保信增加
		t_float  maxVal;                             //最大值
		t_float  minVal;                             //最小值
		t_float  stepVal;                            //步长
		t_char   unitStr[MAX_STRLEN];                //单位
		t_uint8  dataType;					    //数据类型
		t_uint8  dataSize;                      //数据宽度
		t_char   dataTypeStr[MAX_STRLEN];

		//为模拟服务器增加
		t_char   strVal[MAX_NAMELEN];

		
	}SVCHNINFO,*PSVCHNINFO;

	//smv控制块
	typedef struct _SVINFO
	{
		_SVINFO()
		{
			customId = 0;
			ethIndex = 0;					         //网口序号从1开始
			appid = 0;						         //APPID 0x0000
			memset(prefix,0x00,MAX_NAMELEN);
			memset(mac,0x00,MAX_MACADDRLEN);
			confRev = 0;						     //1~N 数据集改变次数
			memset(svId,0x00,MAX_NAMELEN);
			memset(datSet,0x00,MAX_NAMELEN);
			memset(ldDevName,0x00,MAX_NAMELEN);
			memset(lnName,0x00,MAX_NAMELEN);
			datCount = 0;									 //数据对象数目
			datSetType = 0;						 //数据集模板
			convertType = 0;						 //数据转换类型
			enable = 0;							 // 0位：是否检查数据对象数目 1位：是否检查confRev字段 2位：检查SMVID字段 3位：检查datSet字段4位：需要暂态录波 5位：是否检查双AD偏差
			attribId = 0;
		 }
		~_SVINFO()
		{
			for (t_uint i = 0 ;i < chnList.size() ;i++)
			{	
			  PSVCHNINFO psvChnInfo = chnList[i];
			  delete psvChnInfo;
			}
			chnList.clear();
		}
		t_ushort customId;                           //自定义序号 ，唯一   
		t_uchar  ethIndex;					         //网口序号从1开始
		t_uint32 appid;						         //APPID 0x0000
		t_char	 prefix[MAX_NAMELEN];		         //前缀
		t_char	 mac[MAX_MACADDRLEN];		         //MAC地址（01-0C-CD-04-01-01）
		t_uchar  confRev;						     //1~N 数据集改变次数
		t_char	 svId[MAX_NAMELEN];				     //SMVID
		t_char	 datSet[MAX_NAMELEN];				 //数据集字段
		t_char   ldDevName[MAX_NAMELEN];             //逻辑设备名
		t_char   lnName[MAX_NAMELEN];                //逻辑设节点
		t_ushort datCount;						     //数据对象数目

		t_uchar  datSetType;						 //数据集模板
		t_uchar  convertType;						 //数据转换类型
		t_ushort vlan;                               //vlan

		t_ushort enable;							 // 0位：是否检查数据对象数目 1位：是否检查confRev字段 2位：检查SMVID字段 3位：检查datSet字段4位：需要暂态录波 5位：是否检查双AD偏差

		//为保信增加
		t_uint8  attribId;                           //属性id ，定值，模拟量，开关量、告警，事件等信息

		typedef struct _AV_INFO
		{
			_AV_INFO()
			{
	          priRated = 0;					//一次额定值
	          secRated= 0.0;					//二次额定值
	          priReferVal= 0.0;			    //一次参考值（电压0.01 电流0.001）
	          referToSamp= 0;				//参考值对应采样值0x1
	          overLaod= 0;					//过载倍数（电流：50 电压：2）
	          sampLimit= 0.0;					//AD有效采样门槛（额定值5%）
	          twoADDiff= 0.0;					//双AD偏差（10%）
			  precipValue = 1;

			}
			t_float          priRated;					//一次额定值
			t_float          secRated;					//二次额定值
			t_float          priReferVal;			    //一次参考值（电压0.01 电流0.001）
			t_int            referToSamp;				//参考值对应采样值0x1
			t_ushort         overLaod;					//过载倍数（电流：50 电压：2）
			t_float          sampLimit;					//AD有效采样门槛（额定值5%）
			t_float          twoADDiff;					//双AD偏差（10%）
			t_float			 precipValue;               //数据异常变大，门槛值
		}AV_INFO;

		AV_INFO curInfo;
		AV_INFO volInfo;

		std::vector<SVCHNINFO*> chnList;			   //通道列表
	}SVINFO,*PSVINFO;


	//MMS控制块
	typedef struct _MMSINFO
	{
		_MMSINFO()
		{
			memset(name,0x00,MAX_NAMELEN);
			memset(desc,0x00,MAX_NAMELEN);
			memset(ip,0x00,MAX_IPADDRLEN);
			memset(mask,0x00,MAX_IPADDRLEN);
			memset(destIp,0x00,MAX_IPADDRLEN);
			memset(destMask,0x00,MAX_IPADDRLEN);
			memset(apName,0x00,MAX_STRLEN);
			appid = 0;
			ethIndex = 0;
			overtime = 0;
		}
		~_MMSINFO()
		{
			
		}
		
		t_char		name[MAX_NAMELEN];
		t_char		desc[MAX_NAMELEN];
		t_char      ip[MAX_IPADDRLEN];		    //ip地址
		t_char      apName[MAX_STRLEN];
		t_char	    mac[MAX_MACADDRLEN];		         //MAC地址（01-0C-CD-04-01-01）
		t_char      mask[MAX_IPADDRLEN];		//掩码
		t_char      destIp[MAX_IPADDRLEN];      //目标ip
		t_char      destMask[MAX_IPADDRLEN];      //目标掩码
 		t_uint32    appid;                      //mms本身没有appid，为了查询方便 ，生成appid = MT_MMS << 16+ i
		t_uchar     ethIndex;					//网口编号,从1开始
		t_int       overtime;                   //超时时间

	}MMSINFO,*PMMSINFO;


	//PTP控制块
	typedef struct _PTPINFO
	{
		_PTPINFO()
		{
			memset(name,0x00,MAX_NAMELEN);
			memset(desc,0x00,MAX_NAMELEN);
			appid = 0;
			ethIndex = 0;
		}
		~_PTPINFO()
		{
			
		}
		
		t_char		name[MAX_NAMELEN];
		t_char		desc[MAX_NAMELEN];
 		t_uint32    appid;                      //ptp本身没有appid，为了查询方便 ，生成appid = MT_PTP << 16+i;
		t_uchar     ethIndex;					//网口编号,从1开始

	}PTPINFO,*PPTPINFO;

	//定值控制块 一个ied应该只有一个
	typedef struct _SETTINGCONTROL
	{
		_SETTINGCONTROL()
		{
			memset(this,0x00,sizeof(_SETTINGCONTROL));

		}

		t_uint8  actSG;
		t_uint8  numOfSGs;

	}SETTINGCONTROL,*PSETTINGCONTROL;

	//IED信息
	typedef struct _IED
	{
		_IED()
		{
			memset(name,0x00,MAX_NAMELEN);
			memset(desc,0x00,MAX_NAMELEN);
			memset(type,0x00,MAX_NAMELEN);
			memset(apName,0x00,MAX_NAMELEN);
			memset(manufacturer,0x00,MAX_NAMELEN);
		 }
		~_IED()
		{
			for (t_uint i = 0 ;i < gsList.size() ;i++)
			{	
			  PGOOSEINFO pGsInfo = gsList[i];
			  delete pGsInfo;
			}
			gsList.clear();

			for (t_uint i = 0 ;i < svList.size() ;i++)
			{	
			  PSVINFO pSvInfo = svList[i];
			  delete pSvInfo;
			}
			svList.clear();

			for (t_uint i = 0 ;i < mmsList.size() ;i++)
			{	
			  PMMSINFO pMms = mmsList[i];
			  delete pMms;
			}
			mmsList.clear();

			for (t_uint i = 0 ;i < ptpList.size() ;i++)
			{	
			  PPTPINFO ptp = ptpList[i];
			  delete ptp;
			}
			ptpList.clear();
		}

		t_char name[MAX_NAMELEN];
		t_char desc[MAX_NAMELEN];
		t_char type[MAX_NAMELEN];
		t_char apName[MAX_NAMELEN];
		t_char manufacturer[MAX_NAMELEN];
	
		std::vector<GOOSEINFO*> gsList;
		std::vector<SVINFO*>    svList;
		std::vector<MMSINFO*>   mmsList;
		std::vector<PTPINFO*>   ptpList;

		SETTINGCONTROL  settingCtrl;     //为模拟服务器增加的定值控制块


	}IED,*PIED;

	//子网络
	typedef struct _SUBNET
	{
		_SUBNET()
		{
			memset(name,0x00,MAX_NAMELEN);
			memset(desc,0x00,MAX_NAMELEN);
			memset(type,0x00,MAX_NAMELEN);
		}
		~_SUBNET()
		{
			for (t_uint i = 0 ;i < iedList.size() ;i++)
			{	
			  PIED pIed = iedList[i];
			  delete pIed;
			}
			iedList.clear();
		}
		std::vector<PIED> iedList;

		t_char type[MAX_NAMELEN];
		t_char name[MAX_NAMELEN];
		t_char desc[MAX_NAMELEN];

	}SUBNET,*PSUBNET;


	typedef struct  _SUBSTA
	{
		_SUBSTA()
		{
			memset(name,0x00,MAX_NAMELEN);
		}

		t_char name[MAX_NAMELEN];

	}SUBSTA,*PSUBSTA;






//命令类型定义
namespace CMD_TYPE_DEF
{
	enum MESSAGE_TYPE
	{
		MT_GOOSE        = 1,         //goose
		MT_SMV          = 2,         //smv
		MT_MMS          = 3,         //mms
		MT_PTP          = 4,         //ptp
		MT_SNTP         = 5,         //sntp
		MT_OTHER        = 6,         //其他     //定义了other.other 这个值可以不用了
		MT_ETH          = 7,         //网口
		MT_OTHER_GOOSE  = 0x0601,      //other.goose
		MT_OTHER_SMV    = 0x0602,      //other.smv
		MT_OTHER_MMS    = 0x0603,      //other.mms
		MT_OTHER_PTP    = 0x0604,      //other.ptp
		MT_OTHER_SNTP   = 0x0605,      //other.sntp
		MT_OTHER_OTHER  = 0x0606,      //other.other

	};
	//事件类型
	enum MES_EVENT_TYPE
	{
		MET_EVENT        = 1,          //事件 
		MET_ALARM        = 2,          //告警
		MET_COMM_STATE   = 3,          //通讯状态
	};


	enum CMDSIZE
	{
        CMDDATA_MAXSIZE       = 1024,		      //(/*命令数据的最大长度*/)
        CMDTIME_MAXSIZE       = 23,				  //(/*命令时间最大长度*/) 
     
    };

	//命令码
	enum CMDCODE
	{
		 CMD_PROCESS_ALIVE         = 1,		      //进程保活消息
		 CMD_KILL_PROCESS          = 2,		      //停止进程
	     CMD_MANUAL_TRIGGER        = 3,		      //手动录波
         CMD_TIME_TRIGGER          = 4,		      //定时录波
		 CMD_ENABLE_FIXVALUE       = 5,		      //向前置机下送定值
		 CMD_REQUEST_FIXVALUE      = 6,		      //申请前置机定值
		 CMD_NEW_TRIGGER_INFO      = 7,		      //上送新事件信息
	     CMD_SET_TIME              = 8,		      //修改前置机时间
		 CMD_FORCE_STOP_RECORD     = 9,		      //强制停止录波
		 CMD_SETTING_CHANGED       = 11,		  //配线信息改变
		 CMD_FIXVALUE_CHANGED      = 12,		  //定值信息改变
		 CMD_REQUEST_VERINFO       = 13,		  //请求硬件版本信息
		 CMD_SYSTEMPARAM_CHANGED   = 14,		  //系统参数改变
		 CMD_COMMPARAM_CHANGED     = 15,		  //通信参数改变
		 CMD_GPSPARAM_CHANGED      = 16,		  //GPS参数改变
		 CMD_REQUEST_TIME          = 17,		  //申请前置机时间
		 CMD_RECORD_COMPLETED      = 18,		  //录波完成（有新录波文件生成）
		 CMD_CLEAR_WARNING         = 19,		  //复归告警信号
		 CMD_REMOTE_RECORD         = 20,		  //远方遥控录波
		 CMD_GPS_SET_TIME          = 21,		  //GPS软件对时
		 CMD_REMOTE_ENABLE_FIX     = 22,		  //远方修改前置机定值
		 CMD_REMOTE_SET_TIME       = 23,		  //远方修改前置机时间，附带TIME_STRUCT数据
		 CMD_GPS_STATE             = 24,		  //请求GPS状态
		 CMD_COLLECTING            = 25,		  //正在采集录波数据，附带t_uint32类型数据指示数据大小（字节）
		 CMD_ANALYZING             = 26,		  //正在进行故障分析，附带t_uint32类型数据指示分析进度
		 CMD_ANALYZFINSH           = 27,		  //分析完成
		 CMD_QUEST_COMMTRADE_FILE  = 28,		  //请求commtrade文件
		 CMD_QUEST_GENERAL_FILE    = 29,		  //请求普通文件
		 CMD_FILEDATA              = 30,		  //普通文件数据
		 CMD_REALFILEDATA          = 31,		  //实时文件数据
		 CMD_READYFILE             = 32,		  //准备发送文件
		 CMD_FILELIST              = 33,		  //请求文件列表
		 CMD_DOWNGENERALFILE       = 34,          //下装文件
		 CMD_FILE_PROGRESSS        = 35,		  //文件进度指示  
		 CMD_FRONT_COMMSTATS       = 36,		  //前置机通信状态 
		 CMD_EVENTLIST             = 37,          //请求事件列表
		 CMD_DELETE_FILE           = 38,          //删除comtrade文件
		 CMD_DELETE_EVENT          = 39,          //删除事件
		 CMD_ROOT_FRONTSYS         = 40,          //启动前置机系统
		 CMD_FRONT_TIME            = 41,          //前置机系统时间
		 CMD_SEND_SYSCMD           = 42,          //发送系统操作命令
		 CMD_SMV_GOOSE_CHANAGE     = 43,          //goose,sv配置改变
		 CMD_MSG_FIX_CHANAGE       = 44,          //报文定值改变


		 CMD_MSG_FILELIST          = 100,         //请求报文的稳态文件列表 
		 CMD_MSG_TRANS_FILELIST    = 101,         //请求报文的暂态文件列表
		 CMD_MSG_EVENTLIST         = 102,         //请求报文的事件列表
		 CMD_MSG_STREAMLIST        = 103,         //请求报文的流量列表
		 CMD_MSG_DELTE_FILE        = 104,         //删除报文的稳态文件 
		 CMD_MSG_DELETE_TRANSFILE  = 105,         //删除报文的暂态文件
		 CMD_MSG_DELTE_EVENTLIST   = 106,         //删除报文的事件列表
		 CMD_MSG_DELTE_STREAMLIST  = 107,         //删除报文的流量列表
		 CMD_MSG_EVENT             = 108,         //事件
		 CMD_MSG_NEW_FILE          = 109,         //报文的稳态文件信息
		 CMD_MSG_NEW_TRANS_FILE    = 110,         //报文的暂态文件信息
		 CMD_MSG_QUEST_TRANS_FILE  = 111,         //请求暂态文件
		
		 CMD_DBGINFO_LOGIN         = 200,         //注册信息
		 CMD_DBGINFO_LOGON         = 201,         //注销信息
		 CMD_DBGINFO_HINT		   = 202,		  //提示信息
		 CMD_DBGINFO_ALARM		   = 203,		  //告警信息
		 CMD_DBGINFO_ERR		   = 204,		  //错误信息
		 CMD_DBGINFO_SPECIAL	   = 205,		  //特殊信息
		 CMD_DBGINFO_IMPORT		   = 206,		  //重要信息
		 CMD_DBGINFO_DATAMSG	   = 207,		  //数据报文
		 CMD_DBGINFO_SYSTEM		   = 208		  //系统信息
    };

	//命令结果
	enum CMDRESLT
	{
		 CMD_RESULT_SUCCESS         = 0,		      //命令返回成功
	     CMD_RESULT_RESULT_FAILD    = 1,		      //命令返回失败
		 CMD_RESULT_BEGINPROCESS    = 2,		      //命令开始处理
		 CMD_RESULT_PROCESSING      = 3,		      //命令正在处理
		 CMD_RESULT_ENDBEGINPROCESS = 4,		      //命令结束处理

	};

	
#pragma pack(push)
#pragma pack(1)
	
		
	
	//文件进度数据
	typedef struct _FILE_PROGRESS
	{
		t_char	 destName[MAX_NAMELEN];		//目标文件名
		t_uint32 fileTotalLen ;   //文件总长度
		t_uint32 fileLengthed;	  //已经接收的长度

	}FILE_PROGRESS, *PFILE_PROGRESS;

	//文件名 源文件名，目标文件名
	typedef struct _SRC_DEST_FILENAME
	{
		t_char	 srcName[MAX_NAMELEN];		//源文件名
		t_char	 destName[MAX_NAMELEN];		//目标文件名

	}SRC_DEST_FILENAME, *PSRC_DEST_FILENAME;

	//命令数据
	typedef struct _CMD_DATA
	{
		t_char str[CMDDATA_MAXSIZE];
	}CMD_DATA, *PCMD_DATA;

	//程序地址
	typedef struct _PROG_ADDR
	{

		t_ushort  id;                        //程序标识。
		t_char    ip[MAX_IPADDRLEN] ;         //程序IP地址

	}PROG_ADDR, *PPROG_ADDR;


	//命令头
	typedef struct _CMDHEAD
	{
		PROG_ADDR    localAddr;						   //程序地址,本程序和远方程序
		PROG_ADDR    remoteAddr;						   //程序地址,本程序和远方程序
		t_uint32     cmd;						   //命令码。
		t_uint32     result;					   //消息处理标志。0未处理，1,正在处理 2处理成功   
	}CMDHEAD,*PCMDHEAD;


	//命令类型
	typedef struct _CMD
	{
		_CMD() { memset(this, 0, sizeof(_CMD)); }
		_CMD(const _CMD& r){  	memcpy(this, &r, sizeof(_CMD));}
		_CMD& operator=(const _CMD& r){if(this == &r) return *this;memcpy(this, &r, sizeof(_CMD));return *this;}
		~_CMD(){};
  
		CMDHEAD		head;                            //命令头       
		CMD_DATA	data;							 //命令附加数据。

	}CMD, *PCMD;



#pragma pack(pop)
    
	//命令的回调函数类型
    typedef t_int (*LPRECVROUTINE)(void* ptr, CMD cmd);


}



namespace DSE_TYPE
{
	enum DEV_TYPE
	{
		DT_BUS     = 1,              //是母线类型
		DT_LINE    = 2,              //是线路类型
		DT_TRANS   = 3,              //是变压器类型
		DT_GEN     = 4,              //发电机信息
		DT_EXCITER = 5               //是励磁机
	};

	enum ANALOG_TYPE
	{
		AT_NONE      = 0,//未定义通道类型
		AT_ACVOL	 = 1,/*交流电压*/
		AT_ACCUR	 = 2,/*交流电流*/
		AT_DCVOL	 = 3,/*直流电压*/
		AT_DCCUR	 = 4,/*直流电流*/
		AT_HF		 = 5,/*高频*/
		AT_TEMP	     = 6,/*温度*/
		AT_TIME      = 7, //时间
		AT_TXCUR     = 8, /*测量电流*/
	};

	enum ANALOG_FLAG
	{
		AF_NONE = 0,//未定义通道标识
		AF_A    = 1,//A相通道
		AF_B    = 2,//B相通道
		AF_C    = 3,//C相通道
		AF_N    = 4, //N相通道
		AF_L    = 5  //L通道
	};

}









#endif // _SCDHEADER_H