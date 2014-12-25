
#ifndef _CLIENTSCDPHASE_H
#define _CLIENTSCDPHASE_H

#include "clientdatadefine.h"
#include "clientxmlimpl.h"
#include "clientscdheader.h"


//自定义的da结构，临时用，主要查找没有DA情况下根据do查找下面的所有Da
typedef struct _CUSTOM_DA
{
	QString daName;       //DA名
	QString daDataType;   //DA数据类型

	QList <_CUSTOM_DA*> custDaList;

}CUSTOM_DA,*PCUSTOM_DA;

//信息点
typedef struct _FCDA
{
	_FCDA()
	{
		lnDesc = "";
		prefix = "";
		doName = "";
		daName = "";
		fc     = "";
		ldInst = "";
		lnClass = "";
		chnName = "";
		lnType  = "";
		maxVal  = "";
		minVal  = "";
		stepVal = "";
		unitVal = "";


	}
	QString lnDesc;
	QString prefix;
	QString doName;
	QString daName;
	QString fc;
	QString ldInst;
	QString lnClass;
	QString lnInst;
	QString chnName;
	QString lnType;

	QString  maxVal;       //最大值
	QString  minVal;       //最小值
	QString  stepVal;       //步长 
	QString  unitVal;      //单位
	
	GSDATA_TYPE  dataType;					    //数据类型

}FCDA,*PFCDA;


//数据集
typedef struct _DATASET
{
	QString desc;
	QString name;

	QList<PFCDA> fcdaList;
	
}DATASET,*PDATASET;



//goose或者smv控制块
typedef struct _SMVGSE
{
	/*_SMVGSE()
	{
		control  = "";
		sgAppid  = "";
		ldInst   = "";
		cbName   = "";
		desc     = "";
		macAddr  = "";
		appId    = "";
		inst     = "";
		datSet   = "";
		confRev  = "";
	}*/

	quint8  control;
	QString sgAppid;    //goose或者smv的id
	QString ldInst;     //逻辑设备实例 和inst 的值应该是一样的
	QString cbName;
	QString desc;
	QString macAddr;
	QString appId;
	QString inst;		//<LDevice inst="GOLD">
	QString datSet;
	QString confRev;

	QList<PDATASET> dataSetList;

	REPORTCONTROL reportCtrl;                    //报告控制

}SMVGSE,*PSMVGSE;

//goose或者smv控制块
typedef struct _SMVGSE1
{
	/*_SMVGSE1()
	{
		control  = "";
		sgAppid  = "";
		ldInst   = "";
		cbName   = "";
		desc     = "";
		macAddr  = "";
		appId    = "";
		inst     = "";
		datSet   = "";
		confRev  = "";
		lnType   = "";
		t1       = "";
		t0       = "":
	}*/

	quint8  control;
	QString sgAppid;    //goose或者smv的id
	QString ldInst;
	QString cbName;
	QString desc;
	QString macAddr;
	QString appId;
	QString inst;		//<LDevice inst="GOLD">
	QString datSet;
	QString confRev;
	QString lnType;
	QString t1;   //1s
	QString t0;   //5000s

}SMVGSE1,*PSMVGSE1;



//<DO name="Mod" desc="模式" type="PL2205B_CN_INC_Mod" transient="false" />
typedef struct _DO
{
	t_char name[MAX_CHARLEN];
	t_char desc[MAX_CHARLEN];
	t_char type[MAX_CHARLEN];
	t_char cdc[MAX_CHARLEN];

}DO,*PDO;



//<LNodeType id="PL2205B_SAC_IED_GGIO_Evt" desc="保护动作" lnClass="GGIO">
typedef struct _LNODE_TYPE
{
	t_char id[MAX_CHARLEN];
	t_char desc[MAX_CHARLEN];
	t_char lnClass[MAX_CHARLEN];


	//QList<PDO> doList;
	QMap<QString, PDO> doTypeListMap;  //2014-04-14

}LNODE_TYPE,*PLNODE_TYPE;

//<Communication>
//通讯层的ied
typedef struct _INTERNAL_IED
{

	QString iedName;
	QString desc;
	QString apName;
	QString manufacturer;

	QList<PSMVGSE1>			smvGseList;
	std::vector<MMSINFO*>   mmsList;

}INTERNAL_IED,*PINTERNAL_IED;


//<IED name="PL1121" desc="110KV高压线路1保护" type="PCS941" manufacturer="南瑞继保" configVersion="1.00">
//装置层的ied
typedef struct _INTERNAL_IED1
{

	QString iedName;
	QString desc;
	QString apName;
	QString manufacturer;

	QList<PSMVGSE> smvGseList;

}INTERNAL_IED1,*PINTERNAL_IED1;


//DA name="general" bType="BOOLEAN" dchg="true" qchg="false" dupd="false" fc="ST" />
//<DA name="rangeC" bType="Struct" type="PL2205B_CN_RangeConfig" dchg="false" qchg="false" dupd="false" fc="CF" />
typedef struct _DA
{
	t_char name[MAX_CHARLEN];
	t_char bType[MAX_CHARLEN];
    t_char type[MAX_CHARLEN];
	t_char fc[MAX_CHARLEN];

}DA,*PDA;

typedef struct _SDO
{
	t_char name[MAX_CHARLEN];
    t_char type[MAX_CHARLEN];

}SDO,*PSDO;


//	<DOType id="PL2205B_CN_ACD_3P" desc="方向保护激活信息(三相)" cdc="ACD">
typedef struct _DO_TYPE
{

	t_char id[MAX_CHARLEN];
	t_char cdc[MAX_CHARLEN];
	t_char desc[MAX_CHARLEN];

	QList<PDA> daList;
	//QList<PSDO> sdoList;
	QMap<QString, PSDO> sdoListMap;  //2014-04-14

}DO_TYPE,*PDO_TYPE;

typedef struct _BDA_TYPE
{

    t_char name[MAX_CHARLEN];
	t_char bType[MAX_CHARLEN];
    t_char type[MAX_CHARLEN];

}BDA_TYPE,*PBDA_TYPE;
typedef struct _DA_TYPE
{
	t_char id[MAX_CHARLEN];
	//QList<PBDA_TYPE> bdaTypeList;
	QMap<QString, PBDA_TYPE> bdaTypeListMap;  //2014-04-14

}DA_TYPE,*PDA_TYPE;


//子网络
typedef struct _CUSTOM_SUBNET
{
	_CUSTOM_SUBNET()
	{
		memset(name,0x00,MAX_NAMELEN);
		memset(desc,0x00,MAX_NAMELEN);
		memset(type,0x00,MAX_NAMELEN);
	}
	~_CUSTOM_SUBNET()
	{
	}
	std::vector<PINTERNAL_IED> iedList;

	t_char type[MAX_NAMELEN];
	t_char name[MAX_NAMELEN];
	t_char desc[MAX_NAMELEN];

}CUSTOM_SUBNET,*PCUSTOM_SUBNET;


typedef struct _STATION
{
	QString desc;
	QString name;

	QList<PCUSTOM_SUBNET> subNetList;
	QList<PINTERNAL_IED1> iedList;

	//QList<PLNODE_TYPE>   lnTypeList;    //
	QMap<QString, PLNODE_TYPE> lnTypeListMap;  //2014-04-14

	//QList<PDO_TYPE>      doTypeList;
	QMap<QString, PDO_TYPE> doTypeListMap;  //2014-04-14

	//QList<PDA_TYPE>      daTypeList;
	QMap<QString, PDA_TYPE> daTypeListMap;  //2014-04-14
	
	

}STATION,*PSTATION;


class  ClientDSE_ScdManagerImpl : public DSE_XmlSaxHandler
{
public:

	ClientDSE_ScdManagerImpl() 
	{
		initScdVars();
	    m_routine = NULL;
	    m_param = NULL;
	};
	~ClientDSE_ScdManagerImpl(){clearScdVars();};


		 //设置回调函数及参数
	 void set_routine(CMD_TYPE_DEF::LPRECVROUTINE r, void* param)
	  {
		  m_routine = r; 
		  m_param   = param;
	  }

	//读scd文件      
	t_int readScd(const t_char        *fileName,
				   std::vector<PSUBNET>    &subNetList,
				   SUBSTA               &subSta,
				   t_uint8  flag,
				   t_char* err = NULL );

	t_int write(const t_char          *fileName,
			    std::vector<PSUBNET>    &subNetList,
				SUBSTA               &subSta,
			    t_char* err = NULL );

	t_int read(const t_char *fileName ,
			    std::vector<PSUBNET>    &subNetList,
				SUBSTA               &subSta,
			    t_char* err = NULL );


public:

    std::vector<PSUBNET>     *m_subNetList;
	SUBSTA                   *m_subSta;


private:

	virtual bool analystStartEmelent(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes);
	virtual bool analystEndEmelent(const QString &namespaceURI, const QString &localName,
		const QString &qName);
	virtual bool analystCharacters(const QString &str);
	

private:

	enum FILETYPE_FLAG
	{
		FL_SCD    = 1,//scd文件
		FL_XML    = 2,//生成内部的xml文件格式
		FL_CID    = 3,//cid文件

		FL_LINES  = 5000,   //读的行数
	};  

	
	FILETYPE_FLAG   m_fileFlag;     //解析的文件类型标识
	t_uint8			m_decodeflag;           //解析标志，是解析通讯层还是点表

	
   CMD_TYPE_DEF::LPRECVROUTINE	   m_routine;              //回调函数
   void*	m_param;									   //回调函数的参数

   CMD_TYPE_DEF::FILE_PROGRESS  m_fileProgress;              //读scd进度指示
   int            m_fileStep;                                //步长
   int            m_equipTotal;                              //装置的总数  


//==========================解析scd所用的变量=========================================================
	STATION          m_sta;          //内部解析用的站信息，信息比较全
	PINTERNAL_IED1   m_ied;
	PINTERNAL_IED    m_ied2;
	QString          m_ied2Name;
	QString          m_ied2Desc;
	QString          m_ied2ApName;

	SMVGSE1          *m_msvgse ;

	quint8          u8SubNetwork;
	QList<PDATASET> m_dataSetList;
	PDATASET        m_dataSet;

	QMap<QString, PFCDA> m_doMap;  //2014-05-18
    QMultiMap<QString, PFCDA> m_sdoMap;
	QMultiMap<QString, PFCDA> m_lnTypeMap;

	PSMVGSE         m_smvgse;
	PMMSINFO        m_mmsInfo1;
	FCDA            m_fcda;
	QString         m_inst;
	PLNODE_TYPE     m_pLnType;
	PDO_TYPE        m_pDoType;
	PFCDA           m_pFcda;
	PDA_TYPE        m_pDaType;
	PCUSTOM_SUBNET   m_pCustomSubNet;
	SETTINGCONTROL   m_settingCtrl;  //定值区的描述

	PFCDA           m_pMulteiFcda;


	QMap<QString, QString> m_cdcMap;  //2014-04-14 cdc常用的数据类型映射
	QMap<QString, QStringList> m_getDaListMap;  //2014-04-14 根据cdc数据类型获得da列表
	QMap<QString, int> m_getDataTypeMap;  //2014-04-14 根据字符的数据类型获得自定义的类型值

	QMap<QString ,int> m_qNameMap;  //xml节点映射
   
	bool        bScl;
	bool        m_bComLayer;
	
	bool		bWrite ;
	bool		bSubstation;
	QString		strSubstation ;
	bool		bCommunication ;
	QString		strCommunication ;
	bool		bSubNetwork ;
	QString		strSubNetwork ;
	bool		bConnectedAP ;
	QString		strConnectedAP ;
	bool		bGSE ;
	QString		strGSE ;
	bool		bSMV ;
	QString		strSMV ;
	bool		bP ;
	QString		strP;
	bool		bMinTime ;
	QString		strMinTime;
	bool		bMaxTime ;
	QString		strMaxTime;
	QString		strMacAddr ;
	bool		bMacAddr ;
	QString		strAppId ;
	bool		bAppId ;
	QString     strIp;
	bool        bIp;
	QString     strIPSUBNET;
	bool        bIPSUBNET;
	QString		strIed ;
	bool		bIed ;
	QString		strAccessPoint ;
	bool		bAccessPoint ;
	QString		strService ;
	bool	    bService ;
	QString		strLDevice ;
	bool		bLdevice ;
	QString		strLN0 ;
	bool		bLn0 ;
	QString		strDataSet ;
	bool		bDataSet ;
	QString		strFcda ;
	bool		bFcda;
	bool        bInputs;
	QString		strGSEControl ;
	bool		bGSEControl ;
	QString		strSampledValueControl ;
	bool		bSampledValueControl ;
		QString         strRptEnabled;
	QString			strReportControl;
	QString			strSettingControl;
	QString		strLN ;
	bool		bLn ;
	QString		strDoi ;
	bool		bDoi ;

	QString     m_strDoi;
	QString     strSDI; //<SDI name="units">
	bool        bSDI;
	QString     strDAI;

	bool        bMax;        
	bool        bMin;
	bool        bUnit;
	bool        bMultiplier;
	bool        bStep;

	QString     m_strMultiplier;

   

	
//==========================解析scd所用的变量=========================================================

//==========================解析xml所用的变量=========================================================

//	PNETCONFIG m_netCfg;
	quint8     m_ipCount;
	PSUBNET    m_subNet;
	PIED       m_ied1;
	PGOOSEINFO m_gooseInfo;
	PSVINFO    m_svInfo;
	PMMSINFO   m_mmsInfo;

	bool       m_bNetWork;
	bool       m_bNetCfg;
	bool       m_bEthCfg;
	bool       m_bSubNet;
	bool       m_bIed;
	bool       m_bSmv;
	bool       m_curInfo;
	bool       m_bChnInfo;
	bool       m_bGoose;
	bool       m_bMms;


//==========================解析xml所用的变量=========================================================
	//分析scd文件
	void   startParseScdFile(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes);
    void   endParseScdFile(const QString &namespaceURI, const QString &localName,
                       const QString &qName);

	void   startParseXmlFile(const QString &namespaceURI, const QString &localName,
                       const QString &qName, const QXmlAttributes &attributes);
	void   endParseXmlFile(const QString &namespaceURI, const QString &localName,
                       const QString &qName);

	void   makeScdIp();
	void   makeScdInf1();
	void   makeScdInf2();   //在DO下把所有ST的点做为一个通道
	void   makeScdInf();

	void   createXMlNode(QDomDocument &doc);

    PINTERNAL_IED1 		findIED(QString iedName);					//根据装置名称，查找装置
	PSMVGSE		findSMVGse(QString name,quint8 ctrl,QString inst);							//根据goose或smv名称查找goose和smv
	PDATASET	findDataSet(QString name);							//根据数据集名称找到相应的数据集
	void		setChinName(FCDA fcda,QString desc,QString doName,QString daName);   //设置点的相应中文名称
	bool        setSDIChinName(FCDA fcda,QString desc,QString doName);                                  //计算SDI的相应中文名称
	void		setlnClassLLN0ChinName(FCDA fcda,QString desc);  //设置fcda的 lnClass="LLN0"的中文名称
	PLNODE_TYPE findLnType(	QString id,QString lnClass);            //查找逻辑节点类型
	PDO_TYPE    findDoType(QString id,QString cdc);                 //查找do类型
	//bool        findDo(QString id);
	PDA         findDaType(QString name,QString bType,QString type,QString fc);//查找DA类型
	PFCDA       findFcda(QString prefix,QString lnClass,QString inst,QString doName,QString daName);         //查找fcda
	//bool        findDa(QString id);

	
	QString     getDataType(QString lnType,QString lnClass, QString inst,QString doName,QString fc,QString &daName,bool &bRet);
	bool        getDataType(PFCDA fcda,QList<PCUSTOM_DA> &daList);
	bool        getDaType(PFCDA fcda,QList<PCUSTOM_DA> &daList,PDO_TYPE pDoType);
	bool        makeDaName(PCUSTOM_DA pCustDa,QString daTypeNameForFind);
    void        getDaName(QList<PCUSTOM_DA> &daList,QList<QString> &strList);
	void        freeDaName(QList<PCUSTOM_DA> &daList);
	//bool        getDoType(QString sdoType,QString daName,QString &retType,QString &retbType);
	//bool        getDoCdc(QString id,QString &cdc);  //根据DOType 的id 找到它的cdc 
	bool        getDaType(QString daType,QString &retBdaType);                  //
	bool        getBDAType(QString daType,QString bdaName,QString &retBdaType); 
	//bool        getBDARef(QString daType,QString &bdaRef,QString &retBdaType,QString &retType);   //根据da直接获得他下面的第一个BDA的名称和类型

	t_uint8     getDataTypeValue(QString typeString,t_char *strDefVal);//获得数据类型值
	t_uint8     getDataSizeValue(t_uint8 dataType);  //获得数据类型的宽度
	PSMVGSE     getSmvgse(PINTERNAL_IED1 pIEd1,QString cbName,QString ldInst);
	void        setFcdaLnType(QString prefix,QString lnClass,QString inst,QString lnType);


	//获得通讯层的ied 
	PINTERNAL_IED  getCommLayerIed(QString iedName);
	void           copyFrom(MMSINFO &dest,MMSINFO &src);

	//根据数据集名称，获得属性id
	t_int8      getContrlAttrib(QString dataSetName,t_int8 &attribId);
	//设置最大值 最小值 步长 单位
	void        setMaxMinUnitStepVal(FCDA  fcda,t_uint type,QString strVal);  //1 最大值 2最小值 3 步长 4 单位
	QString     getDaNameFromCdc(QString strCdc);  //根据公共数据类cdc，找到代表值的daName

	

	void initScdVars();
	void initXmlVars();
	void clearScdVars();
	void clearDataSetList();
	void clearStation();
};





//解析scd文件,并生成自定义的xml文件
class  ClientDSE_ScdManager
{
public:
	ClientDSE_ScdManager();
	~ClientDSE_ScdManager();

	void copyFrom( ClientDSE_ScdManager &from);
	 
	//读scd文件
	t_int  readScd(const t_char *fileName,	
		      CMD_TYPE_DEF::LPRECVROUTINE  callbackfun = NULL,			//接收消息的回调函数。
			  void*                        callbackfunparam = NULL,	//回调函数参数。
			  t_uint8  flag = DECODE_PT,                                          //1只解析通讯层，2解析点表
			  t_char *err=NULL);

	t_int  read(const t_char *fileName = GSMVCFG,t_char *err=NULL);      //读生成的xml文件
	t_int  write(const t_char *fileName = GSMVCFG,t_char *err=NULL);     //生成xml文件

	t_int read_event(const t_char *fileName,t_int attribId,t_bool isSv,t_char *iedName,SETTING_ITEM1 *pEvent,t_uint32 *event_nums,t_uint32 max_nums,QHash<QString, int> &hash,t_char *err=NULL);

	//获得cid的iedName,apName
	t_int  getIedAndApName(t_char *iedName,t_char *apName);

	t_int  makeSingeEqTable(char *cidfileName,char *tableName,t_char *err);   //制作单个装置的点表

	t_int  makeSingeEqTable1(char *cidfileName,char *tableName,t_char *err);   //制作单个装置的点表protectdb
	

public:

 
	std::vector<PSUBNET>     m_subNetList;      //子网配置
	SUBSTA                   m_subSta;           //子站层

public:

	void clear();
	void clear(std::vector<PSUBNET> &subNetList);
	
	void copyFrom(SUBNET &dest,SUBNET &src);
	void copyFrom(IED &dest,IED &src);
	void copyFrom(GOOSEINFO &dest,GOOSEINFO &src);
	void copyFrom(GSCHNINFO &dest,GSCHNINFO &src);
	void copyFrom(SVINFO &dest,SVINFO &src);
	void copyFrom(SVCHNINFO &dest,SVCHNINFO &src);
	void copyFrom(MMSINFO &dest,MMSINFO &src);
	void copyFrom(PTPINFO &dest,PTPINFO &src);

private:

	QString m_connName;
	QString m_tableName;
	t_bool   readConnParam(QString &driver,QString &host,QString &database,QString &user,QString &pwd);
	t_bool   readConnParam1(QString &driver,QString &host,QString &database,QString &user,QString &pwd);
	t_bool  connDb(QString driver,QString host,QString database,QString user,QString pwd); //连接数据库
	void    closeDb();
	void    deleteTable();  //删除表的内容

	t_bool  insertItem(t_uint8 CpuId,
		               t_uint8 InfFun,t_uint8 InfInf,
					   t_uint8 InfGroupId,t_uint8 InfItemId,
					   t_uint8 InfEnabled,t_uint8 InfDataType,
					   t_uint8 InfAttribId,QString InfName,
					   QString InfCode,QString InfUnit,
					   t_uint8 InfValueType,t_float InfMaxValue,
					   t_float InfMinValue,t_float InfStep,
					   QString InfDefaultValue,t_float InfFactor,
					   QString InfPrecision,t_int InfKod,
					   t_int InfType,t_int InfSize,
					   t_int InfNumber,t_int InfDisplevelAutoSend,
					   QString InfReference,t_int AssociateInfFun ,
					   QString Iec61850EntryID ,QString Iec61850RptID
					   );

		t_bool  insertItem1(t_uint8 CpuId,
		               t_uint8 InfFun,t_uint8 InfInf,
					   t_uint8 InfEnabled,t_uint8 InfDataType,
					   t_uint8 InfAttribId,QString InfName,
					   QString InfCode,QString InfUnit,
					   t_uint8 InfValueType,t_float InfMaxValue,
					   t_float InfMinValue,t_float InfStep,
					   QString InfDefaultValue,t_float InfFactor,
					   QString InfPrecision,t_int InfKod,
					   t_int InfType,t_int InfSize,
					   t_int InfNumber,t_int InfDisplevelAutoSend,
					   QString InfReference,t_int AssociateInfFun ,
					   QString Iec61850EntryID 
					   );

	QString getInfPrecision(t_int dataType);  //获得精度
	t_uint8 getInfValueType(t_int attribId);  //获得信息值的类型


};



#endif // _61850CLIENT_H