
#ifndef _CLIENTSCDPHASE_H
#define _CLIENTSCDPHASE_H

#include "clientdatadefine.h"
#include "clientxmlimpl.h"
#include "clientscdheader.h"


//�Զ����da�ṹ����ʱ�ã���Ҫ����û��DA����¸���do�������������Da
typedef struct _CUSTOM_DA
{
	QString daName;       //DA��
	QString daDataType;   //DA��������

	QList <_CUSTOM_DA*> custDaList;

}CUSTOM_DA,*PCUSTOM_DA;

//��Ϣ��
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

	QString  maxVal;       //���ֵ
	QString  minVal;       //��Сֵ
	QString  stepVal;       //���� 
	QString  unitVal;      //��λ
	
	GSDATA_TYPE  dataType;					    //��������

}FCDA,*PFCDA;


//���ݼ�
typedef struct _DATASET
{
	QString desc;
	QString name;

	QList<PFCDA> fcdaList;
	
}DATASET,*PDATASET;



//goose����smv���ƿ�
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
	QString sgAppid;    //goose����smv��id
	QString ldInst;     //�߼��豸ʵ�� ��inst ��ֵӦ����һ����
	QString cbName;
	QString desc;
	QString macAddr;
	QString appId;
	QString inst;		//<LDevice inst="GOLD">
	QString datSet;
	QString confRev;

	QList<PDATASET> dataSetList;

	REPORTCONTROL reportCtrl;                    //�������

}SMVGSE,*PSMVGSE;

//goose����smv���ƿ�
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
	QString sgAppid;    //goose����smv��id
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



//<DO name="Mod" desc="ģʽ" type="PL2205B_CN_INC_Mod" transient="false" />
typedef struct _DO
{
	t_char name[MAX_CHARLEN];
	t_char desc[MAX_CHARLEN];
	t_char type[MAX_CHARLEN];
	t_char cdc[MAX_CHARLEN];

}DO,*PDO;



//<LNodeType id="PL2205B_SAC_IED_GGIO_Evt" desc="��������" lnClass="GGIO">
typedef struct _LNODE_TYPE
{
	t_char id[MAX_CHARLEN];
	t_char desc[MAX_CHARLEN];
	t_char lnClass[MAX_CHARLEN];


	//QList<PDO> doList;
	QMap<QString, PDO> doTypeListMap;  //2014-04-14

}LNODE_TYPE,*PLNODE_TYPE;

//<Communication>
//ͨѶ���ied
typedef struct _INTERNAL_IED
{

	QString iedName;
	QString desc;
	QString apName;
	QString manufacturer;

	QList<PSMVGSE1>			smvGseList;
	std::vector<MMSINFO*>   mmsList;

}INTERNAL_IED,*PINTERNAL_IED;


//<IED name="PL1121" desc="110KV��ѹ��·1����" type="PCS941" manufacturer="����̱�" configVersion="1.00">
//װ�ò��ied
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


//	<DOType id="PL2205B_CN_ACD_3P" desc="���򱣻�������Ϣ(����)" cdc="ACD">
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


//������
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


		 //���ûص�����������
	 void set_routine(CMD_TYPE_DEF::LPRECVROUTINE r, void* param)
	  {
		  m_routine = r; 
		  m_param   = param;
	  }

	//��scd�ļ�      
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
		FL_SCD    = 1,//scd�ļ�
		FL_XML    = 2,//�����ڲ���xml�ļ���ʽ
		FL_CID    = 3,//cid�ļ�

		FL_LINES  = 5000,   //��������
	};  

	
	FILETYPE_FLAG   m_fileFlag;     //�������ļ����ͱ�ʶ
	t_uint8			m_decodeflag;           //������־���ǽ���ͨѶ�㻹�ǵ��

	
   CMD_TYPE_DEF::LPRECVROUTINE	   m_routine;              //�ص�����
   void*	m_param;									   //�ص������Ĳ���

   CMD_TYPE_DEF::FILE_PROGRESS  m_fileProgress;              //��scd����ָʾ
   int            m_fileStep;                                //����
   int            m_equipTotal;                              //װ�õ�����  


//==========================����scd���õı���=========================================================
	STATION          m_sta;          //�ڲ������õ�վ��Ϣ����Ϣ�Ƚ�ȫ
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
	SETTINGCONTROL   m_settingCtrl;  //��ֵ��������

	PFCDA           m_pMulteiFcda;


	QMap<QString, QString> m_cdcMap;  //2014-04-14 cdc���õ���������ӳ��
	QMap<QString, QStringList> m_getDaListMap;  //2014-04-14 ����cdc�������ͻ��da�б�
	QMap<QString, int> m_getDataTypeMap;  //2014-04-14 �����ַ����������ͻ���Զ��������ֵ

	QMap<QString ,int> m_qNameMap;  //xml�ڵ�ӳ��
   
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

   

	
//==========================����scd���õı���=========================================================

//==========================����xml���õı���=========================================================

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


//==========================����xml���õı���=========================================================
	//����scd�ļ�
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
	void   makeScdInf2();   //��DO�°�����ST�ĵ���Ϊһ��ͨ��
	void   makeScdInf();

	void   createXMlNode(QDomDocument &doc);

    PINTERNAL_IED1 		findIED(QString iedName);					//����װ�����ƣ�����װ��
	PSMVGSE		findSMVGse(QString name,quint8 ctrl,QString inst);							//����goose��smv���Ʋ���goose��smv
	PDATASET	findDataSet(QString name);							//�������ݼ������ҵ���Ӧ�����ݼ�
	void		setChinName(FCDA fcda,QString desc,QString doName,QString daName);   //���õ����Ӧ��������
	bool        setSDIChinName(FCDA fcda,QString desc,QString doName);                                  //����SDI����Ӧ��������
	void		setlnClassLLN0ChinName(FCDA fcda,QString desc);  //����fcda�� lnClass="LLN0"����������
	PLNODE_TYPE findLnType(	QString id,QString lnClass);            //�����߼��ڵ�����
	PDO_TYPE    findDoType(QString id,QString cdc);                 //����do����
	//bool        findDo(QString id);
	PDA         findDaType(QString name,QString bType,QString type,QString fc);//����DA����
	PFCDA       findFcda(QString prefix,QString lnClass,QString inst,QString doName,QString daName);         //����fcda
	//bool        findDa(QString id);

	
	QString     getDataType(QString lnType,QString lnClass, QString inst,QString doName,QString fc,QString &daName,bool &bRet);
	bool        getDataType(PFCDA fcda,QList<PCUSTOM_DA> &daList);
	bool        getDaType(PFCDA fcda,QList<PCUSTOM_DA> &daList,PDO_TYPE pDoType);
	bool        makeDaName(PCUSTOM_DA pCustDa,QString daTypeNameForFind);
    void        getDaName(QList<PCUSTOM_DA> &daList,QList<QString> &strList);
	void        freeDaName(QList<PCUSTOM_DA> &daList);
	//bool        getDoType(QString sdoType,QString daName,QString &retType,QString &retbType);
	//bool        getDoCdc(QString id,QString &cdc);  //����DOType ��id �ҵ�����cdc 
	bool        getDaType(QString daType,QString &retBdaType);                  //
	bool        getBDAType(QString daType,QString bdaName,QString &retBdaType); 
	//bool        getBDARef(QString daType,QString &bdaRef,QString &retBdaType,QString &retType);   //����daֱ�ӻ��������ĵ�һ��BDA�����ƺ�����

	t_uint8     getDataTypeValue(QString typeString,t_char *strDefVal);//�����������ֵ
	t_uint8     getDataSizeValue(t_uint8 dataType);  //����������͵Ŀ��
	PSMVGSE     getSmvgse(PINTERNAL_IED1 pIEd1,QString cbName,QString ldInst);
	void        setFcdaLnType(QString prefix,QString lnClass,QString inst,QString lnType);


	//���ͨѶ���ied 
	PINTERNAL_IED  getCommLayerIed(QString iedName);
	void           copyFrom(MMSINFO &dest,MMSINFO &src);

	//�������ݼ����ƣ��������id
	t_int8      getContrlAttrib(QString dataSetName,t_int8 &attribId);
	//�������ֵ ��Сֵ ���� ��λ
	void        setMaxMinUnitStepVal(FCDA  fcda,t_uint type,QString strVal);  //1 ���ֵ 2��Сֵ 3 ���� 4 ��λ
	QString     getDaNameFromCdc(QString strCdc);  //���ݹ���������cdc���ҵ�����ֵ��daName

	

	void initScdVars();
	void initXmlVars();
	void clearScdVars();
	void clearDataSetList();
	void clearStation();
};





//����scd�ļ�,�������Զ����xml�ļ�
class  ClientDSE_ScdManager
{
public:
	ClientDSE_ScdManager();
	~ClientDSE_ScdManager();

	void copyFrom( ClientDSE_ScdManager &from);
	 
	//��scd�ļ�
	t_int  readScd(const t_char *fileName,	
		      CMD_TYPE_DEF::LPRECVROUTINE  callbackfun = NULL,			//������Ϣ�Ļص�������
			  void*                        callbackfunparam = NULL,	//�ص�����������
			  t_uint8  flag = DECODE_PT,                                          //1ֻ����ͨѶ�㣬2�������
			  t_char *err=NULL);

	t_int  read(const t_char *fileName = GSMVCFG,t_char *err=NULL);      //�����ɵ�xml�ļ�
	t_int  write(const t_char *fileName = GSMVCFG,t_char *err=NULL);     //����xml�ļ�

	t_int read_event(const t_char *fileName,t_int attribId,t_bool isSv,t_char *iedName,SETTING_ITEM1 *pEvent,t_uint32 *event_nums,t_uint32 max_nums,QHash<QString, int> &hash,t_char *err=NULL);

	//���cid��iedName,apName
	t_int  getIedAndApName(t_char *iedName,t_char *apName);

	t_int  makeSingeEqTable(char *cidfileName,char *tableName,t_char *err);   //��������װ�õĵ��

	t_int  makeSingeEqTable1(char *cidfileName,char *tableName,t_char *err);   //��������װ�õĵ��protectdb
	

public:

 
	std::vector<PSUBNET>     m_subNetList;      //��������
	SUBSTA                   m_subSta;           //��վ��

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
	t_bool  connDb(QString driver,QString host,QString database,QString user,QString pwd); //�������ݿ�
	void    closeDb();
	void    deleteTable();  //ɾ���������

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

	QString getInfPrecision(t_int dataType);  //��þ���
	t_uint8 getInfValueType(t_int attribId);  //�����Ϣֵ������


};



#endif // _61850CLIENT_H