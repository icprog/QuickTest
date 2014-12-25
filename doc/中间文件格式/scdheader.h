#ifndef SCDHEADER_H
#define SCDHEADER_H

#include <stdio.h>
#include <vector>


#ifndef Q_DECL_EXPORT
#  ifdef WIN32
#    define Q_DECL_EXPORT __declspec(dllexport)
#  elif defined(QT_VISIBILITY_AVAILABLE)
#    define Q_DECL_EXPORT __attribute__((visibility("default")))
#  endif
#  ifndef Q_DECL_EXPORT
#    define Q_DECL_EXPORT
#  endif
#endif
#ifndef Q_DECL_IMPORT
#  if defined(WIN32)
#    define Q_DECL_IMPORT __declspec(dllimport)
#  else
#    define Q_DECL_IMPORT
#  endif  
#endif 

#ifdef SCD_LIB    
# define SCD_EXPORT Q_DECL_EXPORT 
#else    
# define SCD_EXPORT Q_DECL_IMPORT 
#endif  


 

	//常用文件定义
  #if defined(WIN32) 

		#define SYSCFGFILE					   ("config/syscfg.xml")
		#define STARTCFGFILE				   ("config/startcfg.xml")
		#define GSMVCFG						   ("config/gsmvcfg.xml")

	#else

		#define SYSCFGFILE						("config/syscfg.xml")
		#define STARTCFGFILE					("config/startcfg.xml")
		#define GSMVCFG							("config/smvgs.xml")
		

	#endif //WIN32

namespace SCD
{

  #define MAX_REFLEN_65         65       //REF长度
  #define MAX_MACADDRLEN_18	    18         //mac地址长度
  #define MAX_IPADDRLEN_16	    16         //IP地址长度
  #define MAX_STR_LEN_128       128
  #define MAX_STR_LEN_256       256
  #define MAX_STR_LEN_65       65

  #define GSMV_TYPE_GOOSE	    1				 //配置信息为GOOSE
  #define GSMV_TYPE_SMV		    2				//配置信息为SMV


	enum MSG_TYPE
	{
		MT_GOOSE        = 1,         //goose
		MT_SMV9_1       = 2,         //9-1
		MT_SMV9_2       = 3,         //9-2
		MT_FT3          = 4,         //ft3
		MT_FT3_EX       = 5,         //ft3扩展
		MT_MMS          = 6,         //mms
		MT_PTP          = 7,         //ptp
		MT_OTHER        = 8,         //其他   
	};

	enum ANG_TYPE
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

		enum ANG_FLAG
		{
			AF_NONE = 0,//未定义通道标识
			AF_A    = 1,//A相通道
			AF_B    = 2,//B相通道
			AF_C    = 3,//C相通道
			AF_N    = 4, //N相通道
			AF_L    = 5  //L通道
		};


	//调度命令
	typedef struct _DSP_CMD
	{
		enum DSP_PLACE_CODE
		{
			DPC_PUBLIC = 0,    
			DPC_FIRST  = 1,
			DPC_SECOND = 2,
		};
		unsigned int     code;						   //命令码。    为DPC_PUBLIC描述放在公共地方 DPC_FIRST描述放在第一个边框 DPC_SECOND放在第二边框
		unsigned int     result;					   //消息处理标志。0成功，1失败
		char             data[1024];

	}DSP_CMD,*PDSP_CMD;

	 //命令码
	enum DSIP_CMD_CODE
	{
		DCC_FILE_PROCESS    = 1,         //文件进度
		DCC_SAVE_FILE_NAME  = 2,         //保存文件名

	};

	//进度指示
	typedef struct _PROGRESS
	{
		char	 destName[MAX_STR_LEN_256];		//目标文件名
		unsigned int fileTotalLen ;				//文件总长度
		unsigned int fileLengthed;				//已经接收的长度

	}PROGRESS, *PPROGRESS;

	//关联文件名
	typedef struct _REAL_FILENAME
	{
		char	 scdName[MAX_STR_LEN_256];		//scd的文件名
		char	 xmlName[MAX_STR_LEN_256];		//生成xml的文件名
		
	}REAL_FILENAME, *PREAL_FILENAME;

    typedef int (*LPCALLFUN)(void* ptr, _DSP_CMD cmd);


	
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
			GTE_VAL_Q_T =  0x08, 
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
			GV_INT16           =  16      //INT16
			
		 };

		
	   bool          bStruct;                            //是否是一个结构
	   unsigned char fromatValue;                        //开关量格式,是strVal,stVal、q ,strVal、q、t 			 
	   unsigned char type;						       //数据类型

   }GSDATA_TYPE,*PGSDATA_TYPE;

   
   	enum _GOOSE_FORMAT   //还应增加int8 int32 等
	{
		GF_DO_StVal_Q_T = 1,
		GF_DPI_Bstr2    = 2,
		GF_DA_StVal_Q_T = 3,
		GF_DA_StVal_Q   = 4,
		GF_DA_StVal     = 5,
		GF_DA_StVal_T   = 6,
		
	};

	

	
	enum MAP_VOL_CUR
	{
		MVC_Ua1       = 1,         //
		MVC_Ub1       = 2,         //
		MVC_Uc1       = 3,         //
		MVC_Un1       = 4,         //

		MVC_Ua2       = 5,         //
		MVC_Ub2       = 6,         //
		MVC_Uc2       = 7,         //
		MVC_Un2       = 8,         //
		
		MVC_Ia1       = 9,         //
		MVC_Ib1       = 10,         //
		MVC_Ic1       = 11,         //
		MVC_In1       = 12,         //

		MVC_Ia2       = 13,         //
		MVC_Ib2       = 14,         //
		MVC_Ic2       = 15,         //
		MVC_In2       = 16,         //

	};

	enum MAP_SWI_IN_OUT
	{
		MSIO_Out1       = 1,       
		MSIO_Out2       = 2,         //
		MSIO_Out3       = 3,         //
		MSIO_Out4       = 4,         //
		MSIO_Out5       = 5,         //
		MSIO_Out6       = 6,         //
		MSIO_Out7       = 7,         //
		MSIO_Out8       = 8,         //

		MSIO_In1        = 9,        
		MSIO_In2        = 10,         //
		MSIO_In3        = 11,         //
		MSIO_In4        = 12,         //
		MSIO_In5        = 13,         //
		MSIO_In6        = 14,         //
		MSIO_In7        = 15,         //
		MSIO_In8        = 16,         //

	};


 
   //输出对应的外部输入通道名称
   typedef struct _DSE_EXTREF
   {
	   _DSE_EXTREF()
	   {
		    memset(iedName, 0, sizeof(iedName));
			memset(extAddr, 0, sizeof(extAddr));
			memset(chnName, 0, sizeof(chnName));
			pInIed = 0;
			pInGooseInf = 0;
			pInSvInf = 0;
			pInSvChnInf = 0;
			pInGsChnInf = 0;
			inIedId = 0;
			inGsId =0;
			inSvId = 0;
			inSvChnId =0;
			inGsChnId = 0;
	   };

	  char   iedName[MAX_REFLEN_65]; //外部装置名
	  char   extAddr[MAX_REFLEN_65];  //外部地址
	  char   chnName[MAX_REFLEN_65];  //外部通道名称


	  struct _IED        *pInIed;
	  unsigned int           inIedId;

	  struct _GOOSEINFO  *pInGooseInf;
	  unsigned int            inGsId;

	  struct _SVINFO     *pInSvInf;
	  unsigned int            inSvId;

	  struct _SVCHNINFO  *pInSvChnInf;
	  unsigned int            inSvChnId;

	  struct _GSCHNINFO  *pInGsChnInf;
	  unsigned int            inGsChnId;

	
   }DSE_EXTREF;

	//goose通道信息
	typedef struct _GSCHNINFO 
	{
		_GSCHNINFO() 
		{ 
			index = 0;
			type = 0;
			flag = 0;
			memset(chnName,0x00,sizeof(chnName));
			memset(strVal,0x00,sizeof(strVal));
			valType1 = 0;
			memset(strVal1,0x00,sizeof(strVal1));
			valType2 = 0;
			memset(strVal2,0x00,sizeof(strVal2));
			mapVal = 0;
			pGsInfo = NULL;
		
		}
		~_GSCHNINFO()
		{
			for (int i = 0 ;i < extRefList.size() ;i++)
			{
				DSE_EXTREF *pExtRef = extRefList[i];
				if (pExtRef) delete pExtRef;

			}
			extRefList.clear();

		}
		unsigned short    index;							//序号（1~N）
		char	    chnName[MAX_REFLEN_65];			//通道名称
		char        datSetName[MAX_REFLEN_65];		//数据对象名
		GSDATA_TYPE  dataType;					    //数据类型
		unsigned char     type;							//通道类型（保护动作/断路器位置、、、、）
		unsigned char     flag;							//通道标识（断A、、、）
		unsigned char     mapVal;                       //MAP_SWI_IN_OUT

		char      strVal[MAX_REFLEN_65];

		//如果dataType.bStruct是结构,第一个定的类型是dataType.type，第二、三个值是下面两个
		unsigned char     valType1;                       //_GSDATA_VALUE_ENUM
		char      strVal1[MAX_REFLEN_65];
		unsigned char     valType2;                       //_GSDATA_VALUE_ENUM
		char      strVal2[MAX_REFLEN_65];


		//输出对应的输入通道
		std::vector<DSE_EXTREF *> extRefList;

		struct _GOOSEINFO *pGsInfo;     //本通道所属控制块

		unsigned int  id;      //通道编号


	}GSCHNINFO ,*PGSCHNINFO ;

	//PT CT 变比
	typedef struct _PT_CT
	{
		_PT_CT()
		{
			priRated = 1.0;
			secRated = 1.0;
		}

		float		   priRated;						 //一次额定值 (v)
		float	       secRated;						 //二次额定值

	}PT_CT,*PPT_CT;

	//smv通道信息
	typedef struct _SVCHNINFO //双AD在SV块中标识
	{
		_SVCHNINFO()
		{ 
			index = 0;
			remoteIndex = 0;
			type = 0;
			flag = 0;
			memset(chnName,0x00,sizeof(chnName));
			memset(datSetName,0x00,sizeof(datSetName));
			fVal = 0;
			freq =0;
			mapVal = 0;
			fPhase = 0;
			pSvInfo = NULL;
		 }
		~_SVCHNINFO()
		{
			for (int i = 0 ;i < extRefList.size() ;i++)
			{
				DSE_EXTREF *pExtRef = extRefList[i];

			}
			extRefList.clear();
		}

		unsigned short       index;							 //序号（1~N）
		unsigned short       remoteIndex;                    //对端序号 ,主要用于双AD 0代表没有双AD
		char                 chnName[MAX_REFLEN_65];			     //通道名称
		char                 datSetName[MAX_REFLEN_65];		         //数据对象名
		unsigned char        type;							 //通道类型（交流电流/交流电压/、、）
		unsigned char		 flag;						 //通道标识（A、B、C、N、、）
		unsigned char        mapVal;                            //MAP_VOL_CUR
		PT_CT                ptCt; 

		float                fVal;
		float                fPhase;
		float                freq;
		

		

		//输出对应的输入通道
		std::vector<DSE_EXTREF *> extRefList;

		struct _SVINFO *pSvInfo;

		unsigned int  id;      //通道编号


	}SVCHNINFO,*PSVCHNINFO;



		//输入通道信息
	typedef struct _INPUT_CHNINFO
	{
		_INPUT_CHNINFO()
		{
			id = 0;
			memset(intAddr,0x00,sizeof(intAddr));
			memset(intName,0x00,sizeof(intName));
			extId = 0;
			memset(extAddr,0x00,sizeof(extAddr));
			memset(extName,0x00,sizeof(extName));
			pOutIed = NULL;
			pOutGooseInf= NULL;
			pOutSvInf = NULL;
			pOutGsChnInf = NULL;
			pOutSvChnInf = NULL;
			outIedId = 0;
			outSvId = 0;
			outGsId = 0;
			outGsChnId = 0;
			outSvChnId = 0;
		}
		unsigned short id;				  //序号 
		char   intAddr[MAX_REFLEN_65];    //内部地址
		char   intName[MAX_REFLEN_65];	  //内部名称 
		unsigned short extId;			  //外部序号
		char   extAddr[MAX_REFLEN_65];    //外部地址
		char   extName[MAX_REFLEN_65];	  //外部名称 


		struct _IED        *pOutIed;
		unsigned int             outIedId;

		struct _GOOSEINFO  *pOutGooseInf;
		unsigned int             outGsId;

		struct _SVINFO     *pOutSvInf;
		unsigned int             outSvId;

		struct _GSCHNINFO  *pOutGsChnInf;
		unsigned int             outGsChnId;

		struct _SVCHNINFO  *pOutSvChnInf;
		unsigned int             outSvChnId;

	}INPUT_CHNINFO,*PINPUT_CHNINFO;

	//goose控制块
	typedef struct _GOOSEINFO//双网考虑在什么地方标识
	{
		_GOOSEINFO()
		{
		
			appid = 0;						  //APPID 0x0000
			memset(prefix,0x00,sizeof(prefix));
			memset(mac,0x00,sizeof(mac));
		    confRev = 0;						  //1~N 数据集改变次数
			memset(gcRef,0x00,sizeof(gcRef));
			memset(datSet,0x00,sizeof(datSet));
			memset(goId,0x00,sizeof(goId));
			datCount = 0;						  //数据对象数目
			vlan = 0;
			priority = 0;

			t0 = 0;
			t1= 0;
			q = 0;
			
			chnTotalNums = 0;
			gooseFromat = GF_DO_StVal_Q_T;
			pIed = 0;
			pRemoteGsInfo = 0;
			remoteId = 0;

		 }
		~_GOOSEINFO()
		{
			for (unsigned int i = 0 ;i < chnList.size() ;i++)
			{	
			  PGSCHNINFO pGsChnInfo = chnList[i];
			  delete pGsChnInfo;
			}
			chnList.clear();

			for (unsigned int i = 0 ;i < inputChnList.size() ; i++)
			{

				INPUT_CHNINFO *pChnInfo = inputChnList[i];

				delete pChnInfo;
			}

			inputChnList.clear();
		}

	
		unsigned int    appid;						  //APPID 0x0000
		char            prefix[MAX_STR_LEN_256];			  //前缀
		char	        mac[MAX_MACADDRLEN_18];				  //MAC地址（01-0C-CD-04-01-01）
		unsigned char	confRev;						  //1~N 数据集改变次数
		char	        gcRef[MAX_REFLEN_65];			  //
		char	        datSet[MAX_REFLEN_65];			  //
		unsigned short  datCount;						  //数据对象数目
		unsigned short  vlan;                               //vlan
		unsigned short  priority;                            //优先级
		char            goId[MAX_REFLEN_65];
		int             t0;                               //稳定条件下重传时间
		int             t1;                               //最短传输时间
		unsigned short  q;                              //故障品质
	
		unsigned char   gooseFromat;                   //开关量的格式,_GOOSE_FORMAT

		std::vector<GSCHNINFO*> chnList;  //输出通道列表 

		//为输入信息
		//bool isInputs;							   //这个控制块是否是输入
		unsigned short chnTotalNums;						   //对应输出控制块的总通道数目
		std::vector<INPUT_CHNINFO*> inputChnList;      //输入通道列表 

		struct _IED  *pIed;  //本控制块所属ied
		unsigned int  iedId; //ied所属的id
		unsigned int  id;      //控制块编号

		_GOOSEINFO   *pRemoteGsInfo;//本控制块作为输入时，这个appid本身所属的控制块
		unsigned int  remoteId;      //控制块编号
		

	}GOOSEINFO,*PGOOSEINFO;

	//smv控制块
	typedef struct _SVINFO
	{
		_SVINFO()
		{
			appid = 0;						         //APPID 0x0000
			memset(prefix,0x00,sizeof(prefix));
			memset(mac,0x00,sizeof(mac));
			memset(svId,0x00,sizeof(svId));
			memset(datSet,0x00,sizeof(datSet));
			memset(datsetDesc,0x00,sizeof(datsetDesc));
				
			chnTotalNums = 0;
			datCount = 0;
			pIed = 0;
			precipValue = 0;
			twoADDiff = 0;
			vlan = 0;
			priority = 0;

		 }
		~_SVINFO()
		{
			for (unsigned int i = 0 ;i < chnList.size() ;i++)
			{	
			  PSVCHNINFO psvChnInfo = chnList[i];
			  delete psvChnInfo;
			}
			chnList.clear();

			for (unsigned int i = 0 ;i < inputChnList.size() ; i++)
			{
				INPUT_CHNINFO *pIntInfo = inputChnList[i];
				delete pIntInfo;
			}
			inputChnList.clear();
		}
		unsigned int   appid;						         //APPID 0x0000
		char	       prefix[MAX_STR_LEN_256];		         //前缀
		char	       mac[MAX_MACADDRLEN_18];		         //MAC地址（01-0C-CD-04-01-01）
		char	       svId[MAX_REFLEN_65];				     //SMVID
		unsigned char  confRev;						         //1~N 数据集改变次数
		unsigned short vlan;                                 //vlan
		unsigned short  priority;                            //优先级
		char	       datSet[MAX_REFLEN_65];				 //数据集字段
		char           datsetDesc[MAX_REFLEN_65];			 //数据集描述
		unsigned short datCount;						     //数据对象数目
	

		float          precipValue;                          //数据异常变大，门槛值
		float	       twoADDiff;						      //双AD偏差（10%）

		std::vector<SVCHNINFO*> chnList;			         //输出通道列表

		//为输入信息
		//bool isInputs;							          //这个控制块是否是输入
		unsigned short chnTotalNums;						  //对应输出控制块的总通道数目
		std::vector<INPUT_CHNINFO*> inputChnList;             //输入通道列表 

		struct _IED  *pIed;  //本控制块所属ied
		unsigned int  iedId; //ied所属的id
		unsigned int  id;      //控制块编号

		_SVINFO      *pRemoteGsInfo;//本控制块作为输入时，这个appid本身所属的控制块
		unsigned int remoteId;   //控制块编号

	}SVINFO,*PSVINFO;


	//2014-2-12添加mms解析部分 begin

	//MMS控制块通道
	typedef struct _MMSCHNINFO
	{

		_MMSCHNINFO()
		{
			maxVal = 0;
			minVal = 0;
			stepVal = 0;
			memset(unitStr,0x00,sizeof(unitStr));
			memset(name,0x00,sizeof(name));
			memset(ref,0x00,sizeof(ref));
			pCtlInfo = NULL;

		}

		char	       name[MAX_REFLEN_65];			//通道名称
		char           ref[MAX_REFLEN_65];		    //数据对象名
		unsigned char  type;						//数据类型 _GSDATA_VALUE_ENUM

        //只有 定值,遥测,装置参数 有这几个参数
		float          maxVal;                             //最大值
		float          minVal;                             //最小值
		float          stepVal;                            //步长
		char           unitStr[MAX_REFLEN_65];               //单位

		struct _MMSCTRLINFO *pCtlInfo ;


	}MMSCHNINFO,*PMMSCHNINFO;

	//MMS控制块
	typedef struct _MMSCTRLINFO
	{

		_MMSCTRLINFO()
		{
			memset(datsetName,0x00,sizeof(datsetName));
			memset(datsetDesc,0x00,sizeof(datsetDesc));

			memset(rptName,0x00,sizeof(rptName));
			confRev = 0;
			memset(rptID,0x00,sizeof(rptID));
			maxRptInst = 0;
			dsType = 0;
		}

		~_MMSCTRLINFO()
		{

			for (int i = 0 ;i < chnList.size() ; i++)
			{
				MMSCHNINFO *pChnInfo = chnList[i];
				if (pChnInfo)
				  delete pChnInfo;
				pChnInfo = NULL;
			}

			chnList.clear();

		}

		 enum _DATASET_TYPE_ENUM
		 {
			DTE_NONE      =  0,      //未知
			DTE_SETTING   =  1,      //定值
			DTE_AIN       =  2,      //遥测
			DTE_PARAM     =  3,	     //装置参数
			
			DTE_DIN       =  4,      //遥信
			DTE_ALRAM     =  5,      //故障信号
			DTE_WARNING   =  6,      //告警信号
			DTE_COMMSTATE =  7,      //通信工况
			DTE_TRIPINFO  =  8,      //保护事件
			DTE_ENA       =  9,      //保护压板
			DTE_REC       =  10,     //保护录波
		 };

		//数据集
		char   datsetName[MAX_REFLEN_65];  //数据集名称
		char   datsetDesc[MAX_REFLEN_65];  //数据集描述

		//控制块下的描述
		char           rptName[MAX_REFLEN_65];               //名称
		unsigned char  confRev;						     //1~N 数据集改变次数
		char           rptID[MAX_REFLEN_65];
		unsigned char  maxRptInst;                         //最大的报告实例个数

		unsigned char  dsType;                             //数据集类型
		//通道
		std::vector<MMSCHNINFO*>    chnList;  //通道列表


	}MMSCTRLINFO,*PMMSCTRLINFO;

	//装置的mms信息
	typedef struct _MMSINFO
	{
		_MMSINFO()
		{
			memset(name,0x00,sizeof(name));
			memset(desc,0x00,sizeof(desc));
			memset(ip,0x00,sizeof(ip));
			memset(mac,0x00,sizeof(mac));
			memset(mask,0x00,sizeof(mask));
			memset(destIp,0x00,sizeof(destIp));
			memset(destMask,0x00,sizeof(destMask));

			memset(OSI_AP_Title,0x00,sizeof(OSI_AP_Title));
			memset(OSI_AE_Qualifier,0x00,sizeof(OSI_AE_Qualifier));
			memset(OSI_PSEL,0x00,sizeof(OSI_PSEL));
			memset(OSI_SSEL,0x00,sizeof(OSI_SSEL));
			memset(OSI_TSEL,0x00,sizeof(OSI_TSEL));

			appid = 0;
			ethIndex = 0;
			overtime = 0;

			actSG = 0;
			numOfSGs = 0;

		}
		~_MMSINFO()
		{
			for (int i = 0 ;i < ctlList.size() ; i++)
			{
				MMSCTRLINFO *pCtlInfo = ctlList[i];
				if (pCtlInfo)
				  delete pCtlInfo;
				pCtlInfo = NULL;
			}
			ctlList.clear();
		}
		
		char		name[MAX_STR_LEN_128];
		char		desc[MAX_STR_LEN_256];
		char        ip[MAX_IPADDRLEN_16];		    //ip地址
		char	    mac[MAX_MACADDRLEN_18];		         //MAC地址（01-0C-CD-04-01-01）
		char        mask[MAX_IPADDRLEN_16];		//掩码
		char        destIp[MAX_IPADDRLEN_16];      //目标ip
		char        destMask[MAX_IPADDRLEN_16];      //目标掩码

 		unsigned int      appid;                      //mms本身没有appid，为了查询方便 ，生成appid = MT_MMS << 16+ i
		unsigned char     ethIndex;					//网口编号,从1开始
		int               overtime;                   //超时时间

		char      OSI_AP_Title[MAX_REFLEN_65];
		char      OSI_AE_Qualifier[MAX_REFLEN_65];
		char      OSI_PSEL[MAX_REFLEN_65];
		char      OSI_SSEL[MAX_REFLEN_65];
		char      OSI_TSEL[MAX_REFLEN_65];

		
		//定值参数
		unsigned char actSG;                                //激活定值区
		unsigned char numOfSGs;                             //定值区个数

		std::vector<MMSCTRLINFO*>    ctlList;  //控制块列表


	}MMSINFO,*PMMSINFO;

	//2014-2-12添加mms解析部分 end


	//PTP控制块
	typedef struct _PTPINFO
	{
		_PTPINFO()
		{
			memset(name,0x00,sizeof(name));
			memset(desc,0x00,sizeof(desc));
			appid = 0;
			ethIndex = 0;
		}
		~_PTPINFO()
		{
			
		}
		
		char		     name[MAX_STR_LEN_128];
		char		     desc[MAX_STR_LEN_256];
 		unsigned int     appid;                      //ptp本身没有appid，为了查询方便 ，生成appid = MT_PTP << 16+i;
		unsigned char    ethIndex;					//网口编号,从1开始

	}PTPINFO,*PPTPINFO;

   
	//SNTP控制块
	typedef struct _SNTPINFO
	{
		_SNTPINFO()
		{
			memset(this,0x00,sizeof(_SNTPINFO));
			
		}
		~_SNTPINFO()
		{
			
		}
		
		char		  name[MAX_STR_LEN_256];
		char		  desc[MAX_STR_LEN_256];
 		unsigned int  appid;                      //sntp本身没有appid，为了查询方便 ，生成appid = MT_SNTP << 16+i;
		unsigned char ethIndex;					//网口编号,从1开始

	}SNTPINFO,*PSNTPINFO;


	//IED信息
	typedef struct _IED
	{
		_IED()
		{
			memset(name,0x00,sizeof(name));
			memset(desc,0x00,sizeof(desc));
			memset(type,0x00,sizeof(type));
			memset(manufacturer,0x00,sizeof(manufacturer));
			memset(apName,0x00,sizeof(apName));
			isShow = false;
			isMms = false;
		 }
		~_IED()
		{
			for (unsigned int i = 0 ;i < gsList.size() ;i++)
			{	
			  PGOOSEINFO pGsInfo = gsList[i];
			  delete pGsInfo;
			}
			gsList.clear();

			for (unsigned int i = 0 ;i < svList.size() ;i++)
			{	
			  PSVINFO pSvInfo = svList[i];
			  delete pSvInfo;
			}
			svList.clear();

			for (unsigned int i = 0 ;i < mmsList.size() ;i++)
			{	
			  PMMSINFO pMms = mmsList[i];
			  if (pMms)
			     delete pMms;
			  pMms = NULL;
			}
			mmsList.clear();

			for (unsigned int i = 0 ;i < ptpList.size() ;i++)
			{	
			  PPTPINFO ptp = ptpList[i];
			  delete ptp;
			}
			ptpList.clear();

			for (unsigned int i = 0 ;i < sntpList.size() ;i++)
			{	
			  PSNTPINFO pSntp = sntpList[i];
			  delete pSntp;
			}
			sntpList.clear();
			
			for (unsigned short i = 0 ; i < gsInputList.size();i++)
			{

				GOOSEINFO *pGsInfo = gsInputList[i];
				delete pGsInfo;
			}
			gsInputList.clear();

			for (unsigned short i = 0 ; i < svInputList.size();i++)
			{

				SVINFO *pSvInfo = svInputList[i];
				delete pSvInfo;
			}
			svInputList.clear();

		}

		char name[MAX_STR_LEN_128];
		char desc[MAX_STR_LEN_256];
		char type[MAX_REFLEN_65];
		char apName[MAX_REFLEN_65];
		char manufacturer[MAX_STR_LEN_128];
		std::vector<GOOSEINFO*> gsList;  //输出
		std::vector<SVINFO*>    svList;  //输出
		std::vector<GOOSEINFO*> gsInputList;  //输入
	    std::vector<SVINFO*>    svInputList;  //输入
		std::vector<MMSINFO*>   mmsList;
		std::vector<PTPINFO*>   ptpList;
		std::vector<SNTPINFO*>  sntpList;

		bool isMms;
		bool isShow;
		unsigned int  id;      //装置编号


	}IED,*PIED;

	//子网络
	typedef struct _SUBNET
	{
		_SUBNET()
		{
			memset(name,0x00,sizeof(name));
			memset(desc,0x00,sizeof(desc));
			memset(type,0x00,sizeof(type));
			isShow = false;
		}
		~_SUBNET()
		{
			for (unsigned int i = 0 ;i < iedList.size() ;i++)
			{	
			  
				  PIED pIed = iedList[i];
				  delete pIed;
			}
			iedList.clear();
		}
		std::vector<PIED>	   iedList;
		char type[MAX_STR_LEN_128];
		char name[MAX_STR_LEN_128];
		char desc[MAX_STR_LEN_256];

		bool isShow;

	}SUBNET,*PSUBNET;

	//时间信息
	typedef struct _TIMESYNC
	{
		_TIMESYNC() { memset(this, 0, sizeof(_TIMESYNC)); }
		
		unsigned char       mode;					//同步模式  0是ptp 1是B码 2是Rdu
		int	                delayAmend;		    //延时修正
		unsigned int        lostAlarm;			//丢失同步告警超时
		bool		        bBelievePTP;          //总是信任ptp
		unsigned char		ethIndex;					    //网口序号从1开始
		char                macAddr[MAX_MACADDRLEN_18];      //mac

	}TIMESYNC,*PTIMESYNC;

	//公共信息
	typedef struct _PUPINFO 
	{
		_PUPINFO() { memset(this, 0, sizeof(_PUPINFO)); }

		char          name[MAX_STR_LEN_128];	//设备名称
		int           timeDiff;				//时差(分)
		unsigned int  netOverTime;			//网络超时（秒）
		unsigned char gsOnlyFlag;			//goose匹配依赖
		unsigned char svOnlyFlag;			//sv匹配依赖
		unsigned int  svSampleRate;		    //sv采样频率
		bool		  bJustActAlarm;		//只告警状态变位事件
		char          staName[MAX_STR_LEN_128]; //站名称
		bool          bPriVal;              //true一次值 ，false二次值


		//
		char          fileId[MAX_STR_LEN_65];  //文件标识
		char          version[MAX_STR_LEN_65]; //版本号
		char          reversion[MAX_STR_LEN_65]; //修订版本号
		char          fileTime[MAX_STR_LEN_65];  //文件创建时间
		char          toolID[MAX_STR_LEN_65];    //工具软件
		char          fileImportTime[MAX_STR_LEN_65];  //文件导入时间


	}PUPINFO,*PPUPINFO;


	//网络配置
	typedef struct _NETCONFIG
	{
 		_NETCONFIG() { memset(this, 0, sizeof(_NETCONFIG)); }

		typedef struct _NET_IP
		{
			char ip[MAX_IPADDRLEN_16];			//IP地址
			char mask[MAX_IPADDRLEN_16];			//子网掩码
		}NET_IP,*PNET_IP;

		NET_IP		  ip[5];				//一个网口最多设置5个网段
		char          macAddr[MAX_MACADDRLEN_18];      //mac
		char		  name[MAX_STR_LEN_128];			//网口别名
		unsigned char index;						//网口序号，从1开始
		unsigned char       ethType;                      //网口类型  DSE_TYPE::ET_ETH ,DSE_TYPE::ET_ETN,DSE_TYPE::ET_TNET
	}NETCONFIG,*PNETCONFIG;

}

#endif // SCDANALYST_GLOBAL_H