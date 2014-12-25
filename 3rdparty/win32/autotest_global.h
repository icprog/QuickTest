#ifndef AUTOTEST_GLOBAL_H
#define AUTOTEST_GLOBAL_H

#ifdef __cplusplus
#define BEGIN_C_DECLS  extern "C" {
#define END_C_DECLS      } 
#else
#define BEGIN_C_DECLS
#define END_C_DECLS 
#endif

#define AUTOTEST_EXPORT __declspec(dllexport)

BEGIN_C_DECLS

#pragma pack(push)
#pragma pack(1)

//��ֵ��
typedef struct _ITEM_PARAM
{
   char ref[65];
   char desc[128];         //Ϊ��̬���õ�������
   char str_value[50];

   char max_val_str[30];//���ֵ
   char min_val_str[30];//��С
   char step_str[30];   //����
   char	unit[10];       //��λ

}ITEM_PARAM,*PITEM_PARAM;

//���ϱ���
typedef struct _FAULT_REPORT
{
	char server_name[65];   //��������
	char ref[65];           //����
	char state[128];        //״̬ 
	char time[30];          //ʱ��

}FAULT_REPORT,*PFAULT_REPORT;

//������ֹ����
typedef struct _CMD_TERMAINAL_REPORT
{
	char server_name[65];   //��������
	char oper_name[65];     //��������
	char error;             //������ 1�д��� 0 �޴���
	char cause_str[128];    //ԭ������


}CMD_TERMAINAL_REPORT,*PCMD_TERMAINAL_REPORT;

//��Ϣ����
enum INFO_TYPE_ID
{
	ITI_SETTING        = 1,//��ֵ
	ITI_ZONE           = 2,//��ֵ��
	ITI_EQUP_PARAM     = 3, //װ�ò���
	ITI_TELEMETRY      = 4, //ң��
	ITI_SWI            = 5, //ң��
	ITI_YB             = 6, //ѹ��
	ITI_CHECK          = 7, //�Լ�
	ITI_ALARM          = 8, //�澯
	ITI_EVENT          = 9, //�¼�
	ITI_RECORD_EVENT   = 10, //¼���¼�
};

//�����б�����
enum NAME_LIST_TYPE
{
	NLT_DOMAIN_NAME  = 1,   //�����б�
	NLT_VAR_NAME     = 2,   //�������б�
	NLT_DATASET_NAME = 3,   //���ݼ����б�

};

//mms���ݽṹ��Ϣ
typedef struct
{
	char		name[65 + 1];               //��������		
	int			type;		                //��������						
	int			len;					    //���ݳ���
	int			size;					    //����ռ���ֽ���
}AUTO_TEST_DATA_INFO;

typedef struct
{
	char name[256];
	int  type;
	int  size;
	char desc[1025];
}AUTO_TEST_DATA_INFO1;


//�ļ���
typedef struct _FILE_ITEM
{
	char  name[256];      //�ļ���
	char  ext[50];        //��չ��
	int   size;           //��С

}FILE_ITEM,*PFILE_ITEM;


//�ļ����� 
typedef struct _FILE_ATTRIB
{

	unsigned int 	fsize; 			            //�ļ�����
	unsigned char   mtimpres;			        //�Ƿ���ʾ���һ���޸�ʱ���־
	__int64			mtime; 			            //�ļ����һ���޸�ʱ��
    char			filename [255+1]; //�ļ�����

}FILE_ATTRIB,*PFILEATTRIB;



typedef struct
{
	int		numOfSG;
	int		actSG;
	int		editSG;
	int		cnfEdit;//bool

}SGCB;

#pragma pack(pop)

//�����ص�����
typedef int (*LP_LOG_CALLFUN)(void* ptr, char *desc);

//���ϱ���ص�����
typedef int (*LP_FAULT_REPORT_CALLFUN)(void* ptr, FAULT_REPORT *fault_report);

//������ֹ����ص�����
typedef int (*LP_CMD_TERMINAL_REPORT_CALLFUN)(void* ptr, CMD_TERMAINAL_REPORT *cmd_terminal_report);

// ��������:��̬���ʼ��        
// ��ڲ��� 
// ���ڲ���:
// ����ֵ:0:�ɹ�:1:ʧ�� 
 AUTOTEST_EXPORT unsigned int   auto_test_init(char* err);  


// ��������:��̬���˳�       
// ��ڲ��� 
// ���ڲ���:
// ����ֵ:0:�ɹ�:1:ʧ�� 
 AUTOTEST_EXPORT  unsigned int   auto_test_quit(char* err);  


// ��������:������־�ص�����       
// ��ڲ��� : callbackfun      :�ص���������
//            callbackfunparam :�ص������Ĳ���
// ���ڲ���:
// ����ֵ:
 AUTOTEST_EXPORT void   auto_test_set_log_callback_fun(LP_LOG_CALLFUN callbackfun ,
													  void*    callbackfunparam	  
													 );  


// ��������:���ù��ϱ���ص�����       
// ��ڲ��� : callbackfun      :�ص���������
//            callbackfunparam :�ص������Ĳ���
// ���ڲ���:
// ����ֵ:
 AUTOTEST_EXPORT void   auto_test_set_fault_report_callback_fun(LP_FAULT_REPORT_CALLFUN callbackfun  ,
															   void*    callbackfunparam 	  
															 );  

 // ��������������ֹ�ص�����       
// ��ڲ��� : callbackfun      :�ص���������
//            callbackfunparam :�ص������Ĳ���
// ���ڲ���:
// ����ֵ:
 AUTOTEST_EXPORT void   auto_test_set_cmd_termainal_report_callback_fun(LP_CMD_TERMINAL_REPORT_CALLFUN callbackfun  ,
															   void*    callbackfunparam 	  
															 );  


// ��������:61850����װ��   
// ��ڲ���:
//          server_name:��������
// ���ڲ���:
//          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT  unsigned int  auto_test_connect_equip(char *server_name, char* err ); 


// ��������:61850�Ͽ�װ��   
// ��ڲ���:
//          server_name:��������
// ���ڲ���:
//          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_disconnect_equip(char *server_name, char* err); 


// ��������:����װ��osicfg.xml�ļ�
// ��ڲ���:
// ���ڲ���:
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT  unsigned int  auto_test_reload_osicfg(); 


// ��������:װ��scl�ļ� ģ��  
// ��ڲ���:
//          server_name       :��������
//          scl_file_name     :scd����cid�ļ���
//          ied_name          :ģ�͵�ied����
//          access_point_name :MMS�ķ��ʵ� һ��Ϊ"S1"
// ���ڲ���:
//          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_local_scl_model(char *server_name, char *scl_file_name, char *ied_name, char *access_point_name, char* err); 


 // ��������:��̬����scl�ļ� ģ��  
// ��ڲ���:
//          server_name       :��������
// ���ڲ���:
//          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_dyn_scl_model(char *server_name, char* err); 


 // ��������:��̬���õ��  
// ��ڲ���:
//          server_name       :��������
//          info_type         :��Ϣ���� ��ӦINFO_TYPE_ID
// ���ڲ���:
//          err        :��������
 //         pt         :���ػ�õĵ��
//          pt_nums    :������Ŀ
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_dyn_pt(char *server_name,unsigned char info_type,ITEM_PARAM **pt,unsigned short *pt_nums, char* err); 


 // ��������:�ͷŵ��  
// ��ڲ���:
//          ITEM_PARAM         :��Ϣ��ָ��
// ���ڲ���:

// ����ֵ:
 AUTOTEST_EXPORT void  auto_test_free_ptr(void *pt); 






// ��������:��ȡ61850Ҷ�ӽڵ��ֵ   
// ��ڲ���:
//          server_name:����������
//          ref        :Ҷ������
// ���ڲ���:
//          str_val    :ֵ
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_read_value(char *server_name,char *ref,char *str_val, char* err);



// ��������:��ȡ61850���Ҷ�ӽڵ��ֵ   
// ��ڲ���:
//          server_name:����������
//          ref        :Ҷ������
// ���ڲ���:
//          str_val    :ֵ
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_read_multi_value(char *server_name,char **ref,int refNums,char **str_val, char* err);

 // ��������:��ȡ61850�������ݼ���ֵ   
// ��ڲ���:
//          server_name:����������
//          ref        :Ҷ������
// ���ڲ���:
//          str_val    :ֵ
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_read_dataSet_value(char *server_name,char **dataSet,int dataSetNums,char **ref,int refNums,char **str_val, char* err);


// ��������:д61850Ҷ�ӽڵ��ֵ   
// ��ڲ���:
//          server_name:����������
//          ref        :Ҷ������
// ���ڲ���:
//          str_val    :ֵ
//          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT unsigned int  auto_test_write_value(char *server_name,char *ref,char *str_val, char* err);


// ��������:�޸ĵ�ǰ��ֵ��Ķ����ֵ
// ��ڲ���:
//          server_name:����������
//          item       :Ҫ�޸ĵ�����
// ���ڲ���:
//          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_modify_setting(char *server_name,ITEM_PARAM *item,unsigned short item_nums, char* err);

 

// ��������:�޸Ķ��װ�ò���
// ��ڲ���:
//          server_name:����������
//          item       :Ҫ�޸ĵ�����
// ���ڲ���:
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_modify_equip_param(char *server_name,ITEM_PARAM *item,unsigned short item_nums, char* err);


// ��������:�޸Ķ����ѹ��
// ��ڲ���:
//          server_name:����������
//          item       :Ҫ�޸ĵ�����
// ���ڲ���:
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
 AUTOTEST_EXPORT unsigned int  auto_test_modify_yb(char *server_name,ITEM_PARAM *item,unsigned short item_nums,unsigned char ctrl_model, char* err);



 
 // ��������: �������ͻ���򣬱����������ݼ��б�
// ��ڲ���:
//          server_name:����������
//          name_type         :��Ҫ��������б������ NAME_LIST_TYPE
//          dom_name          :����,���name_type=NLT_DOMAIN_NAME ������NULL
// ���ڲ���:
 //          names      :���ص���������
 //          nums       :���ص��������
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT  unsigned int auto_test_get_name_list(char *server_name,unsigned char name_type,char *dom_name,char (**names)[128], int *nums,char *err );

 // ��������: �ͷ��򣬱����������ݼ��б�
// ��ڲ���:
//          server_name:����������
//          name_type         :��Ҫ��������б������ NAME_LIST_TYPE
//          names             :��Ҫ�ͷŵ���������
 AUTOTEST_EXPORT void  auto_test_free_name_list(unsigned char name_type,char (*names)[128]);

 
// ��������: �������Ŀ¼��Ϣ
// ��ڲ���:
//          server_name:����������
//          var_ref           :Ҷ������
//          dir_info          :����,���name_type=NLT_DOMAIN_NAME ������NULL
// ���ڲ���:
 //          dir_info   :���ص�Ŀ¼����
 //          nums       :���ص��������
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT unsigned int auto_test_get_data_dir(char *server_name,char *var_ref, AUTO_TEST_DATA_INFO **dir_info, int *nums,char *err);

// ��������: �ͷ����ݼ�Ŀ¼��Ϣ
// ��ڲ���:
//          dir_info:Ŀ¼����
AUTOTEST_EXPORT void auto_test_free_data_dir(AUTO_TEST_DATA_INFO *dir_info);

// ��������: ������ݼ��µ�������Ϣ
// ��ڲ���:
//          server_name:����������
//          dataset_ref :Ҷ������
//          data_info   :����,���name_type=NLT_DOMAIN_NAME ������NULL
// ���ڲ���:
 //          data_info   :���ص�Ŀ¼����
 //          nums       :���ص��������
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT unsigned int auto_test_get_dataset_datainfo(char *server_name, char *dataset_ref, AUTO_TEST_DATA_INFO1 **data_info,int *nums, int desc_get_mode,int leaf_mode);

// ��������: �ͷ����ݼ�
// ��ڲ���:
//          server_name:����������
//          dataset_ref           :Ҷ������
//          data_info          :����,���name_type=NLT_DOMAIN_NAME ������NULL
// ���ڲ���:
 //          data_info   :���ص�Ŀ¼����
 //          nums       :���ص��������
 //          err        :��������
// ����ֵ:
//			0:�ɹ�:
//          1:ʧ�� 
AUTOTEST_EXPORT void auto_test_free_dataset_datainfo(AUTO_TEST_DATA_INFO1 *data_info);



END_C_DECLS




#endif // AUTOTEST_GLOBAL_H
