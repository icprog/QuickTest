#ifndef _SCDHEADER_H
#define _SCDHEADER_H

#include "clientdatadefine.h"

#define ATTRIB_SETTING			 20       //��ֵ��Ϣ
#define ATTRIB_EQU_PARAM		 23       //װ�ò�����Ϣ
#define ATTIRB_RUN_SETTING_NONE  21       //���ж�ֵ����Ϣ
#define ATTIRB_CUR_SETTING_NONE  22       //��ǰ��ֵ����Ϣ

#define ATTRIB_SAMPLE			 10       //ģ������Ϣ 

#define ATTRIB_SWI				 0        //��������Ϣ            
#define ATTRIB_YB				 1        //��ѹ����Ϣ
#define ATTRIB_YYB				 2        //Ӳѹ����Ϣ

#define ATTRIB_ALARM			 30       //�澯��Ϣ
#define ATTRIB_SELF_CHECK		 32       //�Լ���Ϣ

#define ATTRIB_EVENT             40       //�¼���Ϣ

#define ATTRIB_RELAYREC          50       //���ϼ�

#define ATTRIB_REPORTCTRL        160     //������ƿ�
#define ATTRIB_SETTINCTRL        161     //��ֵ���ƿ�

#define DECODE_COMMLAYER         0x01       //����ͨѶ��
#define DECODE_PT                0x02       //�������
#define DECODE_ALL               0x0f       //��������



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
			GV_INT16           =  16,      //INT16
			GV_Sbpos           =  17       //����
			
		 };

		
	   t_bool  bStruct;                            //�Ƿ���һ���ṹ
	   t_uint8 fromatValue;                        //��������ʽ,��strVal,stVal��q ,strVal��q��t 			 
	   t_uint8  type;						       //��������

   }GSDATA_TYPE,*PGSDATA_TYPE;


	//gooseͨ����Ϣ
	typedef struct _GSCHNINFO 
	{
		_GSCHNINFO() { memset(this, 0, sizeof(_GSCHNINFO)); }
		t_ushort    index;							//��ţ�1~N��
		t_char	    chnName[MAX_NAMELEN];			//ͨ������
		t_char      datSetName[MAX_NAMELEN];		//���ݶ�����
		t_uint8     dataType;					    //��������
		t_uint8     dataSize;                      //���ݿ��
		t_char      dataTypeStr[MAX_STRLEN];
		t_uchar     type;							//ͨ�����ͣ���������/��·��λ�á���������
		t_uchar     flag;							//ͨ����ʶ����A��������

		t_ushort    enable;							//0λ���Ƿ�ѡ��1λ���Ƿ�˫�ڵ㰴���㴦��

		//Ϊģ�����������
		t_char   strVal[MAX_STRLEN];
	}GSCHNINFO ,*PGSCHNINFO ;

	
	typedef struct _REPORTCONTROL
	{
		_REPORTCONTROL()
		{
			memset(this,0x00,sizeof(_REPORTCONTROL));

		}
		t_char     ld[MAX_CHARLEN];    //�߼��豸
		t_char     name[MAX_CHARLEN];
		t_char     datSet[MAX_CHARLEN];
		t_char     rptID[MAX_CHARLEN];
		t_uint8    max;        //��������ӵĸ���

	}REPORTCONTROL,*PREPORTCONTROL;

	//goose���ƿ�
	typedef struct _GOOSEINFO//˫��������ʲô�ط���ʶ
	{
		_GOOSEINFO()
		{
			customId = 0;
			ethIndex = 0;					      //���ڱ��,��1��ʼ
			appid = 0;						  //APPID 0x0000
			memset(prefix,0x00,MAX_NAMELEN);
			memset(mac,0x00,MAX_CHARLEN);
		    confRev = 0;						  //1~N ���ݼ��ı����
			memset(gcRef,0x00,MAX_CHARLEN);
			memset(datSet,0x00,MAX_CHARLEN);
			memset(goId,0x00,MAX_CHARLEN);
			datCount = 0;						  //���ݶ�����Ŀ
			vlan = 0;
			enable = 0;						  //0λ���Ƿ������ݶ�����Ŀ 1λ���Ƿ���confRev�ֶ� 2λ�����gcRef�ֶ� 3λ�����datSet�ֶ� 4λ����Ҫ��̬¼��,5:�Ƿ���AB��
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

		t_ushort  customId;                       //�Զ������ ��Ψһ    
		t_uchar   ethIndex;					      //���ڱ��,��1��ʼ
		t_uint32  appid;						  //APPID 0x0000
		t_char    prefix[MAX_NAMELEN];			  //ǰ׺
		t_char	  mac[MAX_CHARLEN];				  //MAC��ַ��01-0C-CD-04-01-01��
		t_uchar	  confRev;						  //1~N ���ݼ��ı����
		t_char	  gcRef[MAX_CHARLEN];			  //
		t_char	  datSet[MAX_CHARLEN];			  //
		t_ushort  datCount;						  //���ݶ�����Ŀ
		t_ushort  vlan;                               //vlan
		t_char    goId[MAX_CHARLEN];
		t_int     t0;                               //�ȶ��������ش�ʱ��
		t_int     t1;                               //��̴���ʱ��
		t_ushort  enable;						   //0λ���Ƿ������ݶ�����Ŀ 1λ���Ƿ���confRev�ֶ� 2λ�����gcRef�ֶ� 3λ�����datSet�ֶ� 4λ����Ҫ��̬¼��,5:�Ƿ���AB��,6�����Ƿ���goid
	
		t_uint8  attribId;                           //����id ����ֵ��ģ���������������澯���¼�����Ϣ
		REPORTCONTROL reportCtrl;                    //�������

		std::vector<GSCHNINFO*> chnList;
	}GOOSEINFO,*PGOOSEINFO;

	//smvͨ����Ϣ
	typedef struct _SVCHNINFO //˫AD��SV���б�ʶ
	{
		_SVCHNINFO() { memset(this, 0, sizeof(_SVCHNINFO)); }

		t_ushort       index;							 //��ţ�1~N��
		t_ushort       remoteIndex;                      //�Զ���� ,��Ҫ����˫AD 0����û��˫AD
		t_char         chnName[MAX_NAMELEN];			 //ͨ������
		t_char         datSetName[MAX_NAMELEN];		     //���ݶ�����
		t_uchar        type;							 //ͨ�����ͣ���������/������ѹ/������
		t_uchar		   flag;							 //ͨ����ʶ��A��B��C��N������
		t_float		   priRated;						 //һ�ζֵ
		t_float	       secRated;						 //���ζֵ
		t_float        priReferVal;						 //һ�βο�ֵ����ѹ0.01 ����0.001��
		t_int		   referToSamp;						 //�ο�ֵ��Ӧ����ֵ0x2d41 ��Ҫ����9-1,��һ����׼ֵ ��2041����1000v ��2041/1000*�յ���ֵ
		t_ushort       overLaod;						 //���ر�����������50 ��ѹ��2��
		t_float        sampLimit;						 //AD��Ч�����ż����ֵ5%��
		t_float		   twoADDiff;						 //˫ADƫ�10%��
		int            freq;                              //����Ƶ��  //????
		t_float        precipValue;                       //�����쳣����ż�ֵ

		t_float		   range;
		t_float		   blm;
		t_float		   smpMin;

		t_ushort enable;							   //0λ���Ƿ�ѡ��1λ���Ƿ������Ч�澯

		//Ϊ��������
		t_float  maxVal;                             //���ֵ
		t_float  minVal;                             //��Сֵ
		t_float  stepVal;                            //����
		t_char   unitStr[MAX_STRLEN];                //��λ
		t_uint8  dataType;					    //��������
		t_uint8  dataSize;                      //���ݿ��
		t_char   dataTypeStr[MAX_STRLEN];

		//Ϊģ�����������
		t_char   strVal[MAX_NAMELEN];

		
	}SVCHNINFO,*PSVCHNINFO;

	//smv���ƿ�
	typedef struct _SVINFO
	{
		_SVINFO()
		{
			customId = 0;
			ethIndex = 0;					         //������Ŵ�1��ʼ
			appid = 0;						         //APPID 0x0000
			memset(prefix,0x00,MAX_NAMELEN);
			memset(mac,0x00,MAX_MACADDRLEN);
			confRev = 0;						     //1~N ���ݼ��ı����
			memset(svId,0x00,MAX_NAMELEN);
			memset(datSet,0x00,MAX_NAMELEN);
			memset(ldDevName,0x00,MAX_NAMELEN);
			memset(lnName,0x00,MAX_NAMELEN);
			datCount = 0;									 //���ݶ�����Ŀ
			datSetType = 0;						 //���ݼ�ģ��
			convertType = 0;						 //����ת������
			enable = 0;							 // 0λ���Ƿ������ݶ�����Ŀ 1λ���Ƿ���confRev�ֶ� 2λ�����SMVID�ֶ� 3λ�����datSet�ֶ�4λ����Ҫ��̬¼�� 5λ���Ƿ���˫ADƫ��
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
		t_ushort customId;                           //�Զ������ ��Ψһ   
		t_uchar  ethIndex;					         //������Ŵ�1��ʼ
		t_uint32 appid;						         //APPID 0x0000
		t_char	 prefix[MAX_NAMELEN];		         //ǰ׺
		t_char	 mac[MAX_MACADDRLEN];		         //MAC��ַ��01-0C-CD-04-01-01��
		t_uchar  confRev;						     //1~N ���ݼ��ı����
		t_char	 svId[MAX_NAMELEN];				     //SMVID
		t_char	 datSet[MAX_NAMELEN];				 //���ݼ��ֶ�
		t_char   ldDevName[MAX_NAMELEN];             //�߼��豸��
		t_char   lnName[MAX_NAMELEN];                //�߼���ڵ�
		t_ushort datCount;						     //���ݶ�����Ŀ

		t_uchar  datSetType;						 //���ݼ�ģ��
		t_uchar  convertType;						 //����ת������
		t_ushort vlan;                               //vlan

		t_ushort enable;							 // 0λ���Ƿ������ݶ�����Ŀ 1λ���Ƿ���confRev�ֶ� 2λ�����SMVID�ֶ� 3λ�����datSet�ֶ�4λ����Ҫ��̬¼�� 5λ���Ƿ���˫ADƫ��

		//Ϊ��������
		t_uint8  attribId;                           //����id ����ֵ��ģ���������������澯���¼�����Ϣ

		typedef struct _AV_INFO
		{
			_AV_INFO()
			{
	          priRated = 0;					//һ�ζֵ
	          secRated= 0.0;					//���ζֵ
	          priReferVal= 0.0;			    //һ�βο�ֵ����ѹ0.01 ����0.001��
	          referToSamp= 0;				//�ο�ֵ��Ӧ����ֵ0x1
	          overLaod= 0;					//���ر�����������50 ��ѹ��2��
	          sampLimit= 0.0;					//AD��Ч�����ż����ֵ5%��
	          twoADDiff= 0.0;					//˫ADƫ�10%��
			  precipValue = 1;

			}
			t_float          priRated;					//һ�ζֵ
			t_float          secRated;					//���ζֵ
			t_float          priReferVal;			    //һ�βο�ֵ����ѹ0.01 ����0.001��
			t_int            referToSamp;				//�ο�ֵ��Ӧ����ֵ0x1
			t_ushort         overLaod;					//���ر�����������50 ��ѹ��2��
			t_float          sampLimit;					//AD��Ч�����ż����ֵ5%��
			t_float          twoADDiff;					//˫ADƫ�10%��
			t_float			 precipValue;               //�����쳣����ż�ֵ
		}AV_INFO;

		AV_INFO curInfo;
		AV_INFO volInfo;

		std::vector<SVCHNINFO*> chnList;			   //ͨ���б�
	}SVINFO,*PSVINFO;


	//MMS���ƿ�
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
		t_char      ip[MAX_IPADDRLEN];		    //ip��ַ
		t_char      apName[MAX_STRLEN];
		t_char	    mac[MAX_MACADDRLEN];		         //MAC��ַ��01-0C-CD-04-01-01��
		t_char      mask[MAX_IPADDRLEN];		//����
		t_char      destIp[MAX_IPADDRLEN];      //Ŀ��ip
		t_char      destMask[MAX_IPADDRLEN];      //Ŀ������
 		t_uint32    appid;                      //mms����û��appid��Ϊ�˲�ѯ���� ������appid = MT_MMS << 16+ i
		t_uchar     ethIndex;					//���ڱ��,��1��ʼ
		t_int       overtime;                   //��ʱʱ��

	}MMSINFO,*PMMSINFO;


	//PTP���ƿ�
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
 		t_uint32    appid;                      //ptp����û��appid��Ϊ�˲�ѯ���� ������appid = MT_PTP << 16+i;
		t_uchar     ethIndex;					//���ڱ��,��1��ʼ

	}PTPINFO,*PPTPINFO;

	//��ֵ���ƿ� һ��iedӦ��ֻ��һ��
	typedef struct _SETTINGCONTROL
	{
		_SETTINGCONTROL()
		{
			memset(this,0x00,sizeof(_SETTINGCONTROL));

		}

		t_uint8  actSG;
		t_uint8  numOfSGs;

	}SETTINGCONTROL,*PSETTINGCONTROL;

	//IED��Ϣ
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

		SETTINGCONTROL  settingCtrl;     //Ϊģ����������ӵĶ�ֵ���ƿ�


	}IED,*PIED;

	//������
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






//�������Ͷ���
namespace CMD_TYPE_DEF
{
	enum MESSAGE_TYPE
	{
		MT_GOOSE        = 1,         //goose
		MT_SMV          = 2,         //smv
		MT_MMS          = 3,         //mms
		MT_PTP          = 4,         //ptp
		MT_SNTP         = 5,         //sntp
		MT_OTHER        = 6,         //����     //������other.other ���ֵ���Բ�����
		MT_ETH          = 7,         //����
		MT_OTHER_GOOSE  = 0x0601,      //other.goose
		MT_OTHER_SMV    = 0x0602,      //other.smv
		MT_OTHER_MMS    = 0x0603,      //other.mms
		MT_OTHER_PTP    = 0x0604,      //other.ptp
		MT_OTHER_SNTP   = 0x0605,      //other.sntp
		MT_OTHER_OTHER  = 0x0606,      //other.other

	};
	//�¼�����
	enum MES_EVENT_TYPE
	{
		MET_EVENT        = 1,          //�¼� 
		MET_ALARM        = 2,          //�澯
		MET_COMM_STATE   = 3,          //ͨѶ״̬
	};


	enum CMDSIZE
	{
        CMDDATA_MAXSIZE       = 1024,		      //(/*�������ݵ���󳤶�*/)
        CMDTIME_MAXSIZE       = 23,				  //(/*����ʱ����󳤶�*/) 
     
    };

	//������
	enum CMDCODE
	{
		 CMD_PROCESS_ALIVE         = 1,		      //���̱�����Ϣ
		 CMD_KILL_PROCESS          = 2,		      //ֹͣ����
	     CMD_MANUAL_TRIGGER        = 3,		      //�ֶ�¼��
         CMD_TIME_TRIGGER          = 4,		      //��ʱ¼��
		 CMD_ENABLE_FIXVALUE       = 5,		      //��ǰ�û����Ͷ�ֵ
		 CMD_REQUEST_FIXVALUE      = 6,		      //����ǰ�û���ֵ
		 CMD_NEW_TRIGGER_INFO      = 7,		      //�������¼���Ϣ
	     CMD_SET_TIME              = 8,		      //�޸�ǰ�û�ʱ��
		 CMD_FORCE_STOP_RECORD     = 9,		      //ǿ��ֹͣ¼��
		 CMD_SETTING_CHANGED       = 11,		  //������Ϣ�ı�
		 CMD_FIXVALUE_CHANGED      = 12,		  //��ֵ��Ϣ�ı�
		 CMD_REQUEST_VERINFO       = 13,		  //����Ӳ���汾��Ϣ
		 CMD_SYSTEMPARAM_CHANGED   = 14,		  //ϵͳ�����ı�
		 CMD_COMMPARAM_CHANGED     = 15,		  //ͨ�Ų����ı�
		 CMD_GPSPARAM_CHANGED      = 16,		  //GPS�����ı�
		 CMD_REQUEST_TIME          = 17,		  //����ǰ�û�ʱ��
		 CMD_RECORD_COMPLETED      = 18,		  //¼����ɣ�����¼���ļ����ɣ�
		 CMD_CLEAR_WARNING         = 19,		  //����澯�ź�
		 CMD_REMOTE_RECORD         = 20,		  //Զ��ң��¼��
		 CMD_GPS_SET_TIME          = 21,		  //GPS�����ʱ
		 CMD_REMOTE_ENABLE_FIX     = 22,		  //Զ���޸�ǰ�û���ֵ
		 CMD_REMOTE_SET_TIME       = 23,		  //Զ���޸�ǰ�û�ʱ�䣬����TIME_STRUCT����
		 CMD_GPS_STATE             = 24,		  //����GPS״̬
		 CMD_COLLECTING            = 25,		  //���ڲɼ�¼�����ݣ�����t_uint32��������ָʾ���ݴ�С���ֽڣ�
		 CMD_ANALYZING             = 26,		  //���ڽ��й��Ϸ���������t_uint32��������ָʾ��������
		 CMD_ANALYZFINSH           = 27,		  //�������
		 CMD_QUEST_COMMTRADE_FILE  = 28,		  //����commtrade�ļ�
		 CMD_QUEST_GENERAL_FILE    = 29,		  //������ͨ�ļ�
		 CMD_FILEDATA              = 30,		  //��ͨ�ļ�����
		 CMD_REALFILEDATA          = 31,		  //ʵʱ�ļ�����
		 CMD_READYFILE             = 32,		  //׼�������ļ�
		 CMD_FILELIST              = 33,		  //�����ļ��б�
		 CMD_DOWNGENERALFILE       = 34,          //��װ�ļ�
		 CMD_FILE_PROGRESSS        = 35,		  //�ļ�����ָʾ  
		 CMD_FRONT_COMMSTATS       = 36,		  //ǰ�û�ͨ��״̬ 
		 CMD_EVENTLIST             = 37,          //�����¼��б�
		 CMD_DELETE_FILE           = 38,          //ɾ��comtrade�ļ�
		 CMD_DELETE_EVENT          = 39,          //ɾ���¼�
		 CMD_ROOT_FRONTSYS         = 40,          //����ǰ�û�ϵͳ
		 CMD_FRONT_TIME            = 41,          //ǰ�û�ϵͳʱ��
		 CMD_SEND_SYSCMD           = 42,          //����ϵͳ��������
		 CMD_SMV_GOOSE_CHANAGE     = 43,          //goose,sv���øı�
		 CMD_MSG_FIX_CHANAGE       = 44,          //���Ķ�ֵ�ı�


		 CMD_MSG_FILELIST          = 100,         //�����ĵ���̬�ļ��б� 
		 CMD_MSG_TRANS_FILELIST    = 101,         //�����ĵ���̬�ļ��б�
		 CMD_MSG_EVENTLIST         = 102,         //�����ĵ��¼��б�
		 CMD_MSG_STREAMLIST        = 103,         //�����ĵ������б�
		 CMD_MSG_DELTE_FILE        = 104,         //ɾ�����ĵ���̬�ļ� 
		 CMD_MSG_DELETE_TRANSFILE  = 105,         //ɾ�����ĵ���̬�ļ�
		 CMD_MSG_DELTE_EVENTLIST   = 106,         //ɾ�����ĵ��¼��б�
		 CMD_MSG_DELTE_STREAMLIST  = 107,         //ɾ�����ĵ������б�
		 CMD_MSG_EVENT             = 108,         //�¼�
		 CMD_MSG_NEW_FILE          = 109,         //���ĵ���̬�ļ���Ϣ
		 CMD_MSG_NEW_TRANS_FILE    = 110,         //���ĵ���̬�ļ���Ϣ
		 CMD_MSG_QUEST_TRANS_FILE  = 111,         //������̬�ļ�
		
		 CMD_DBGINFO_LOGIN         = 200,         //ע����Ϣ
		 CMD_DBGINFO_LOGON         = 201,         //ע����Ϣ
		 CMD_DBGINFO_HINT		   = 202,		  //��ʾ��Ϣ
		 CMD_DBGINFO_ALARM		   = 203,		  //�澯��Ϣ
		 CMD_DBGINFO_ERR		   = 204,		  //������Ϣ
		 CMD_DBGINFO_SPECIAL	   = 205,		  //������Ϣ
		 CMD_DBGINFO_IMPORT		   = 206,		  //��Ҫ��Ϣ
		 CMD_DBGINFO_DATAMSG	   = 207,		  //���ݱ���
		 CMD_DBGINFO_SYSTEM		   = 208		  //ϵͳ��Ϣ
    };

	//������
	enum CMDRESLT
	{
		 CMD_RESULT_SUCCESS         = 0,		      //����سɹ�
	     CMD_RESULT_RESULT_FAILD    = 1,		      //�����ʧ��
		 CMD_RESULT_BEGINPROCESS    = 2,		      //���ʼ����
		 CMD_RESULT_PROCESSING      = 3,		      //�������ڴ���
		 CMD_RESULT_ENDBEGINPROCESS = 4,		      //�����������

	};

	
#pragma pack(push)
#pragma pack(1)
	
		
	
	//�ļ���������
	typedef struct _FILE_PROGRESS
	{
		t_char	 destName[MAX_NAMELEN];		//Ŀ���ļ���
		t_uint32 fileTotalLen ;   //�ļ��ܳ���
		t_uint32 fileLengthed;	  //�Ѿ����յĳ���

	}FILE_PROGRESS, *PFILE_PROGRESS;

	//�ļ��� Դ�ļ�����Ŀ���ļ���
	typedef struct _SRC_DEST_FILENAME
	{
		t_char	 srcName[MAX_NAMELEN];		//Դ�ļ���
		t_char	 destName[MAX_NAMELEN];		//Ŀ���ļ���

	}SRC_DEST_FILENAME, *PSRC_DEST_FILENAME;

	//��������
	typedef struct _CMD_DATA
	{
		t_char str[CMDDATA_MAXSIZE];
	}CMD_DATA, *PCMD_DATA;

	//�����ַ
	typedef struct _PROG_ADDR
	{

		t_ushort  id;                        //�����ʶ��
		t_char    ip[MAX_IPADDRLEN] ;         //����IP��ַ

	}PROG_ADDR, *PPROG_ADDR;


	//����ͷ
	typedef struct _CMDHEAD
	{
		PROG_ADDR    localAddr;						   //�����ַ,�������Զ������
		PROG_ADDR    remoteAddr;						   //�����ַ,�������Զ������
		t_uint32     cmd;						   //�����롣
		t_uint32     result;					   //��Ϣ�����־��0δ����1,���ڴ��� 2����ɹ�   
	}CMDHEAD,*PCMDHEAD;


	//��������
	typedef struct _CMD
	{
		_CMD() { memset(this, 0, sizeof(_CMD)); }
		_CMD(const _CMD& r){  	memcpy(this, &r, sizeof(_CMD));}
		_CMD& operator=(const _CMD& r){if(this == &r) return *this;memcpy(this, &r, sizeof(_CMD));return *this;}
		~_CMD(){};
  
		CMDHEAD		head;                            //����ͷ       
		CMD_DATA	data;							 //��������ݡ�

	}CMD, *PCMD;



#pragma pack(pop)
    
	//����Ļص���������
    typedef t_int (*LPRECVROUTINE)(void* ptr, CMD cmd);


}



namespace DSE_TYPE
{
	enum DEV_TYPE
	{
		DT_BUS     = 1,              //��ĸ������
		DT_LINE    = 2,              //����·����
		DT_TRANS   = 3,              //�Ǳ�ѹ������
		DT_GEN     = 4,              //�������Ϣ
		DT_EXCITER = 5               //�����Ż�
	};

	enum ANALOG_TYPE
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

	enum ANALOG_FLAG
	{
		AF_NONE = 0,//δ����ͨ����ʶ
		AF_A    = 1,//A��ͨ��
		AF_B    = 2,//B��ͨ��
		AF_C    = 3,//C��ͨ��
		AF_N    = 4, //N��ͨ��
		AF_L    = 5  //Lͨ��
	};

}









#endif // _SCDHEADER_H