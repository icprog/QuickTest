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


 

	//�����ļ�����
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

  #define MAX_REFLEN_65         65       //REF����
  #define MAX_MACADDRLEN_18	    18         //mac��ַ����
  #define MAX_IPADDRLEN_16	    16         //IP��ַ����
  #define MAX_STR_LEN_128       128
  #define MAX_STR_LEN_256       256
  #define MAX_STR_LEN_65       65

  #define GSMV_TYPE_GOOSE	    1				 //������ϢΪGOOSE
  #define GSMV_TYPE_SMV		    2				//������ϢΪSMV


	enum MSG_TYPE
	{
		MT_GOOSE        = 1,         //goose
		MT_SMV9_1       = 2,         //9-1
		MT_SMV9_2       = 3,         //9-2
		MT_FT3          = 4,         //ft3
		MT_FT3_EX       = 5,         //ft3��չ
		MT_MMS          = 6,         //mms
		MT_PTP          = 7,         //ptp
		MT_OTHER        = 8,         //����   
	};

	enum ANG_TYPE
		{
			AT_NONE      = 0,//δ����ͨ������
			AT_ACVOL	 = 1,/*������ѹ*/
			AT_ACCUR	 = 2,/*��������*/
			AT_DCVOL	 = 3,/*ֱ����ѹ*/
			AT_DCCUR	 = 4,/*ֱ������*/
			AT_HF		 = 5,/*��Ƶ*/
			AT_TEMP	     = 6,/*�¶�*/
			AT_TIME      = 7, //ʱ��
			AT_TXCUR     = 8, /*��������*/
		};

		enum ANG_FLAG
		{
			AF_NONE = 0,//δ����ͨ����ʶ
			AF_A    = 1,//A��ͨ��
			AF_B    = 2,//B��ͨ��
			AF_C    = 3,//C��ͨ��
			AF_N    = 4, //N��ͨ��
			AF_L    = 5  //Lͨ��
		};


	//��������
	typedef struct _DSP_CMD
	{
		enum DSP_PLACE_CODE
		{
			DPC_PUBLIC = 0,    
			DPC_FIRST  = 1,
			DPC_SECOND = 2,
		};
		unsigned int     code;						   //�����롣    ΪDPC_PUBLIC�������ڹ����ط� DPC_FIRST�������ڵ�һ���߿� DPC_SECOND���ڵڶ��߿�
		unsigned int     result;					   //��Ϣ�����־��0�ɹ���1ʧ��
		char             data[1024];

	}DSP_CMD,*PDSP_CMD;

	 //������
	enum DSIP_CMD_CODE
	{
		DCC_FILE_PROCESS    = 1,         //�ļ�����
		DCC_SAVE_FILE_NAME  = 2,         //�����ļ���

	};

	//����ָʾ
	typedef struct _PROGRESS
	{
		char	 destName[MAX_STR_LEN_256];		//Ŀ���ļ���
		unsigned int fileTotalLen ;				//�ļ��ܳ���
		unsigned int fileLengthed;				//�Ѿ����յĳ���

	}PROGRESS, *PPROGRESS;

	//�����ļ���
	typedef struct _REAL_FILENAME
	{
		char	 scdName[MAX_STR_LEN_256];		//scd���ļ���
		char	 xmlName[MAX_STR_LEN_256];		//����xml���ļ���
		
	}REAL_FILENAME, *PREAL_FILENAME;

    typedef int (*LPCALLFUN)(void* ptr, _DSP_CMD cmd);


	
	//goose��������
   typedef struct _GSDATA_TYPE
   {
	   _GSDATA_TYPE() { memset(this, 0, sizeof(_GSDATA_TYPE)); }

	   //goos��������ʽ
	    enum _GSDATA_TYPE_ENUM
		 {
			GTE_STRVAL  =  0x01,      //strval
			GTE_Q       =  0x02,      //q
			GTE_T       =  0x04,
			GTE_VAL_Q_T =  0x08, 
		 };

		//ֵ����
		enum _GSDATA_VALUE_ENUM   //��Ӧ����int8 int32 ��
		 {
			GV_NONE            =  0,	  //δ֪
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

		
	   bool          bStruct;                            //�Ƿ���һ���ṹ
	   unsigned char fromatValue;                        //��������ʽ,��strVal,stVal��q ,strVal��q��t 			 
	   unsigned char type;						       //��������

   }GSDATA_TYPE,*PGSDATA_TYPE;

   
   	enum _GOOSE_FORMAT   //��Ӧ����int8 int32 ��
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


 
   //�����Ӧ���ⲿ����ͨ������
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

	  char   iedName[MAX_REFLEN_65]; //�ⲿװ����
	  char   extAddr[MAX_REFLEN_65];  //�ⲿ��ַ
	  char   chnName[MAX_REFLEN_65];  //�ⲿͨ������


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

	//gooseͨ����Ϣ
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
		unsigned short    index;							//��ţ�1~N��
		char	    chnName[MAX_REFLEN_65];			//ͨ������
		char        datSetName[MAX_REFLEN_65];		//���ݶ�����
		GSDATA_TYPE  dataType;					    //��������
		unsigned char     type;							//ͨ�����ͣ���������/��·��λ�á���������
		unsigned char     flag;							//ͨ����ʶ����A��������
		unsigned char     mapVal;                       //MAP_SWI_IN_OUT

		char      strVal[MAX_REFLEN_65];

		//���dataType.bStruct�ǽṹ,��һ������������dataType.type���ڶ�������ֵ����������
		unsigned char     valType1;                       //_GSDATA_VALUE_ENUM
		char      strVal1[MAX_REFLEN_65];
		unsigned char     valType2;                       //_GSDATA_VALUE_ENUM
		char      strVal2[MAX_REFLEN_65];


		//�����Ӧ������ͨ��
		std::vector<DSE_EXTREF *> extRefList;

		struct _GOOSEINFO *pGsInfo;     //��ͨ���������ƿ�

		unsigned int  id;      //ͨ�����


	}GSCHNINFO ,*PGSCHNINFO ;

	//PT CT ���
	typedef struct _PT_CT
	{
		_PT_CT()
		{
			priRated = 1.0;
			secRated = 1.0;
		}

		float		   priRated;						 //һ�ζֵ (v)
		float	       secRated;						 //���ζֵ

	}PT_CT,*PPT_CT;

	//smvͨ����Ϣ
	typedef struct _SVCHNINFO //˫AD��SV���б�ʶ
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

		unsigned short       index;							 //��ţ�1~N��
		unsigned short       remoteIndex;                    //�Զ���� ,��Ҫ����˫AD 0����û��˫AD
		char                 chnName[MAX_REFLEN_65];			     //ͨ������
		char                 datSetName[MAX_REFLEN_65];		         //���ݶ�����
		unsigned char        type;							 //ͨ�����ͣ���������/������ѹ/������
		unsigned char		 flag;						 //ͨ����ʶ��A��B��C��N������
		unsigned char        mapVal;                            //MAP_VOL_CUR
		PT_CT                ptCt; 

		float                fVal;
		float                fPhase;
		float                freq;
		

		

		//�����Ӧ������ͨ��
		std::vector<DSE_EXTREF *> extRefList;

		struct _SVINFO *pSvInfo;

		unsigned int  id;      //ͨ�����


	}SVCHNINFO,*PSVCHNINFO;



		//����ͨ����Ϣ
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
		unsigned short id;				  //��� 
		char   intAddr[MAX_REFLEN_65];    //�ڲ���ַ
		char   intName[MAX_REFLEN_65];	  //�ڲ����� 
		unsigned short extId;			  //�ⲿ���
		char   extAddr[MAX_REFLEN_65];    //�ⲿ��ַ
		char   extName[MAX_REFLEN_65];	  //�ⲿ���� 


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

	//goose���ƿ�
	typedef struct _GOOSEINFO//˫��������ʲô�ط���ʶ
	{
		_GOOSEINFO()
		{
		
			appid = 0;						  //APPID 0x0000
			memset(prefix,0x00,sizeof(prefix));
			memset(mac,0x00,sizeof(mac));
		    confRev = 0;						  //1~N ���ݼ��ı����
			memset(gcRef,0x00,sizeof(gcRef));
			memset(datSet,0x00,sizeof(datSet));
			memset(goId,0x00,sizeof(goId));
			datCount = 0;						  //���ݶ�����Ŀ
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
		char            prefix[MAX_STR_LEN_256];			  //ǰ׺
		char	        mac[MAX_MACADDRLEN_18];				  //MAC��ַ��01-0C-CD-04-01-01��
		unsigned char	confRev;						  //1~N ���ݼ��ı����
		char	        gcRef[MAX_REFLEN_65];			  //
		char	        datSet[MAX_REFLEN_65];			  //
		unsigned short  datCount;						  //���ݶ�����Ŀ
		unsigned short  vlan;                               //vlan
		unsigned short  priority;                            //���ȼ�
		char            goId[MAX_REFLEN_65];
		int             t0;                               //�ȶ��������ش�ʱ��
		int             t1;                               //��̴���ʱ��
		unsigned short  q;                              //����Ʒ��
	
		unsigned char   gooseFromat;                   //�������ĸ�ʽ,_GOOSE_FORMAT

		std::vector<GSCHNINFO*> chnList;  //���ͨ���б� 

		//Ϊ������Ϣ
		//bool isInputs;							   //������ƿ��Ƿ�������
		unsigned short chnTotalNums;						   //��Ӧ������ƿ����ͨ����Ŀ
		std::vector<INPUT_CHNINFO*> inputChnList;      //����ͨ���б� 

		struct _IED  *pIed;  //�����ƿ�����ied
		unsigned int  iedId; //ied������id
		unsigned int  id;      //���ƿ���

		_GOOSEINFO   *pRemoteGsInfo;//�����ƿ���Ϊ����ʱ�����appid���������Ŀ��ƿ�
		unsigned int  remoteId;      //���ƿ���
		

	}GOOSEINFO,*PGOOSEINFO;

	//smv���ƿ�
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
		char	       prefix[MAX_STR_LEN_256];		         //ǰ׺
		char	       mac[MAX_MACADDRLEN_18];		         //MAC��ַ��01-0C-CD-04-01-01��
		char	       svId[MAX_REFLEN_65];				     //SMVID
		unsigned char  confRev;						         //1~N ���ݼ��ı����
		unsigned short vlan;                                 //vlan
		unsigned short  priority;                            //���ȼ�
		char	       datSet[MAX_REFLEN_65];				 //���ݼ��ֶ�
		char           datsetDesc[MAX_REFLEN_65];			 //���ݼ�����
		unsigned short datCount;						     //���ݶ�����Ŀ
	

		float          precipValue;                          //�����쳣����ż�ֵ
		float	       twoADDiff;						      //˫ADƫ�10%��

		std::vector<SVCHNINFO*> chnList;			         //���ͨ���б�

		//Ϊ������Ϣ
		//bool isInputs;							          //������ƿ��Ƿ�������
		unsigned short chnTotalNums;						  //��Ӧ������ƿ����ͨ����Ŀ
		std::vector<INPUT_CHNINFO*> inputChnList;             //����ͨ���б� 

		struct _IED  *pIed;  //�����ƿ�����ied
		unsigned int  iedId; //ied������id
		unsigned int  id;      //���ƿ���

		_SVINFO      *pRemoteGsInfo;//�����ƿ���Ϊ����ʱ�����appid���������Ŀ��ƿ�
		unsigned int remoteId;   //���ƿ���

	}SVINFO,*PSVINFO;


	//2014-2-12���mms�������� begin

	//MMS���ƿ�ͨ��
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

		char	       name[MAX_REFLEN_65];			//ͨ������
		char           ref[MAX_REFLEN_65];		    //���ݶ�����
		unsigned char  type;						//�������� _GSDATA_VALUE_ENUM

        //ֻ�� ��ֵ,ң��,װ�ò��� ���⼸������
		float          maxVal;                             //���ֵ
		float          minVal;                             //��Сֵ
		float          stepVal;                            //����
		char           unitStr[MAX_REFLEN_65];               //��λ

		struct _MMSCTRLINFO *pCtlInfo ;


	}MMSCHNINFO,*PMMSCHNINFO;

	//MMS���ƿ�
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
			DTE_NONE      =  0,      //δ֪
			DTE_SETTING   =  1,      //��ֵ
			DTE_AIN       =  2,      //ң��
			DTE_PARAM     =  3,	     //װ�ò���
			
			DTE_DIN       =  4,      //ң��
			DTE_ALRAM     =  5,      //�����ź�
			DTE_WARNING   =  6,      //�澯�ź�
			DTE_COMMSTATE =  7,      //ͨ�Ź���
			DTE_TRIPINFO  =  8,      //�����¼�
			DTE_ENA       =  9,      //����ѹ��
			DTE_REC       =  10,     //����¼��
		 };

		//���ݼ�
		char   datsetName[MAX_REFLEN_65];  //���ݼ�����
		char   datsetDesc[MAX_REFLEN_65];  //���ݼ�����

		//���ƿ��µ�����
		char           rptName[MAX_REFLEN_65];               //����
		unsigned char  confRev;						     //1~N ���ݼ��ı����
		char           rptID[MAX_REFLEN_65];
		unsigned char  maxRptInst;                         //���ı���ʵ������

		unsigned char  dsType;                             //���ݼ�����
		//ͨ��
		std::vector<MMSCHNINFO*>    chnList;  //ͨ���б�


	}MMSCTRLINFO,*PMMSCTRLINFO;

	//װ�õ�mms��Ϣ
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
		char        ip[MAX_IPADDRLEN_16];		    //ip��ַ
		char	    mac[MAX_MACADDRLEN_18];		         //MAC��ַ��01-0C-CD-04-01-01��
		char        mask[MAX_IPADDRLEN_16];		//����
		char        destIp[MAX_IPADDRLEN_16];      //Ŀ��ip
		char        destMask[MAX_IPADDRLEN_16];      //Ŀ������

 		unsigned int      appid;                      //mms����û��appid��Ϊ�˲�ѯ���� ������appid = MT_MMS << 16+ i
		unsigned char     ethIndex;					//���ڱ��,��1��ʼ
		int               overtime;                   //��ʱʱ��

		char      OSI_AP_Title[MAX_REFLEN_65];
		char      OSI_AE_Qualifier[MAX_REFLEN_65];
		char      OSI_PSEL[MAX_REFLEN_65];
		char      OSI_SSEL[MAX_REFLEN_65];
		char      OSI_TSEL[MAX_REFLEN_65];

		
		//��ֵ����
		unsigned char actSG;                                //���ֵ��
		unsigned char numOfSGs;                             //��ֵ������

		std::vector<MMSCTRLINFO*>    ctlList;  //���ƿ��б�


	}MMSINFO,*PMMSINFO;

	//2014-2-12���mms�������� end


	//PTP���ƿ�
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
 		unsigned int     appid;                      //ptp����û��appid��Ϊ�˲�ѯ���� ������appid = MT_PTP << 16+i;
		unsigned char    ethIndex;					//���ڱ��,��1��ʼ

	}PTPINFO,*PPTPINFO;

   
	//SNTP���ƿ�
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
 		unsigned int  appid;                      //sntp����û��appid��Ϊ�˲�ѯ���� ������appid = MT_SNTP << 16+i;
		unsigned char ethIndex;					//���ڱ��,��1��ʼ

	}SNTPINFO,*PSNTPINFO;


	//IED��Ϣ
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
		std::vector<GOOSEINFO*> gsList;  //���
		std::vector<SVINFO*>    svList;  //���
		std::vector<GOOSEINFO*> gsInputList;  //����
	    std::vector<SVINFO*>    svInputList;  //����
		std::vector<MMSINFO*>   mmsList;
		std::vector<PTPINFO*>   ptpList;
		std::vector<SNTPINFO*>  sntpList;

		bool isMms;
		bool isShow;
		unsigned int  id;      //װ�ñ��


	}IED,*PIED;

	//������
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

	//ʱ����Ϣ
	typedef struct _TIMESYNC
	{
		_TIMESYNC() { memset(this, 0, sizeof(_TIMESYNC)); }
		
		unsigned char       mode;					//ͬ��ģʽ  0��ptp 1��B�� 2��Rdu
		int	                delayAmend;		    //��ʱ����
		unsigned int        lostAlarm;			//��ʧͬ���澯��ʱ
		bool		        bBelievePTP;          //��������ptp
		unsigned char		ethIndex;					    //������Ŵ�1��ʼ
		char                macAddr[MAX_MACADDRLEN_18];      //mac

	}TIMESYNC,*PTIMESYNC;

	//������Ϣ
	typedef struct _PUPINFO 
	{
		_PUPINFO() { memset(this, 0, sizeof(_PUPINFO)); }

		char          name[MAX_STR_LEN_128];	//�豸����
		int           timeDiff;				//ʱ��(��)
		unsigned int  netOverTime;			//���糬ʱ���룩
		unsigned char gsOnlyFlag;			//gooseƥ������
		unsigned char svOnlyFlag;			//svƥ������
		unsigned int  svSampleRate;		    //sv����Ƶ��
		bool		  bJustActAlarm;		//ֻ�澯״̬��λ�¼�
		char          staName[MAX_STR_LEN_128]; //վ����
		bool          bPriVal;              //trueһ��ֵ ��false����ֵ


		//
		char          fileId[MAX_STR_LEN_65];  //�ļ���ʶ
		char          version[MAX_STR_LEN_65]; //�汾��
		char          reversion[MAX_STR_LEN_65]; //�޶��汾��
		char          fileTime[MAX_STR_LEN_65];  //�ļ�����ʱ��
		char          toolID[MAX_STR_LEN_65];    //�������
		char          fileImportTime[MAX_STR_LEN_65];  //�ļ�����ʱ��


	}PUPINFO,*PPUPINFO;


	//��������
	typedef struct _NETCONFIG
	{
 		_NETCONFIG() { memset(this, 0, sizeof(_NETCONFIG)); }

		typedef struct _NET_IP
		{
			char ip[MAX_IPADDRLEN_16];			//IP��ַ
			char mask[MAX_IPADDRLEN_16];			//��������
		}NET_IP,*PNET_IP;

		NET_IP		  ip[5];				//һ�������������5������
		char          macAddr[MAX_MACADDRLEN_18];      //mac
		char		  name[MAX_STR_LEN_128];			//���ڱ���
		unsigned char index;						//������ţ���1��ʼ
		unsigned char       ethType;                      //��������  DSE_TYPE::ET_ETH ,DSE_TYPE::ET_ETN,DSE_TYPE::ET_TNET
	}NETCONFIG,*PNETCONFIG;

}

#endif // SCDANALYST_GLOBAL_H