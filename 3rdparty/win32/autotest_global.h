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

//定值项
typedef struct _ITEM_PARAM
{
   char ref[65];
   char desc[128];         //为动态调用点表而增加
   char str_value[50];

   char max_val_str[30];//最大值
   char min_val_str[30];//最小
   char step_str[30];   //步长
   char	unit[10];       //单位

}ITEM_PARAM,*PITEM_PARAM;

//故障报告
typedef struct _FAULT_REPORT
{
	char server_name[65];   //服务器名
	char ref[65];           //引用
	char state[128];        //状态 
	char time[30];          //时间

}FAULT_REPORT,*PFAULT_REPORT;

//命令中止报告
typedef struct _CMD_TERMAINAL_REPORT
{
	char server_name[65];   //服务器名
	char oper_name[65];     //操作名称
	char error;             //错误结果 1有错误 0 无错误
	char cause_str[128];    //原因描述


}CMD_TERMAINAL_REPORT,*PCMD_TERMAINAL_REPORT;

//信息类型
enum INFO_TYPE_ID
{
	ITI_SETTING        = 1,//定值
	ITI_ZONE           = 2,//定值区
	ITI_EQUP_PARAM     = 3, //装置参数
	ITI_TELEMETRY      = 4, //遥测
	ITI_SWI            = 5, //遥信
	ITI_YB             = 6, //压板
	ITI_CHECK          = 7, //自检
	ITI_ALARM          = 8, //告警
	ITI_EVENT          = 9, //事件
	ITI_RECORD_EVENT   = 10, //录波事件
};

//名称列表类型
enum NAME_LIST_TYPE
{
	NLT_DOMAIN_NAME  = 1,   //域名列表
	NLT_VAR_NAME     = 2,   //变量名列表
	NLT_DATASET_NAME = 3,   //数据集名列表

};

//mms数据结构信息
typedef struct
{
	char		name[65 + 1];               //本层名称		
	int			type;		                //数据类型						
	int			len;					    //数据长度
	int			size;					    //数据占用字节数
}AUTO_TEST_DATA_INFO;

typedef struct
{
	char name[256];
	int  type;
	int  size;
	char desc[1025];
}AUTO_TEST_DATA_INFO1;


//文件项
typedef struct _FILE_ITEM
{
	char  name[256];      //文件名
	char  ext[50];        //扩展名
	int   size;           //大小

}FILE_ITEM,*PFILE_ITEM;


//文件属性 
typedef struct _FILE_ATTRIB
{

	unsigned int 	fsize; 			            //文件长度
	unsigned char   mtimpres;			        //是否显示最后一次修改时间标志
	__int64			mtime; 			            //文件最后一次修改时间
    char			filename [255+1]; //文件名称

}FILE_ATTRIB,*PFILEATTRIB;



typedef struct
{
	int		numOfSG;
	int		actSG;
	int		editSG;
	int		cnfEdit;//bool

}SGCB;

#pragma pack(pop)

//操作回调函数
typedef int (*LP_LOG_CALLFUN)(void* ptr, char *desc);

//故障报告回调函数
typedef int (*LP_FAULT_REPORT_CALLFUN)(void* ptr, FAULT_REPORT *fault_report);

//命令中止报告回调函数
typedef int (*LP_CMD_TERMINAL_REPORT_CALLFUN)(void* ptr, CMD_TERMAINAL_REPORT *cmd_terminal_report);

// 功能描述:动态库初始化        
// 入口参数 
// 出口参数:
// 返回值:0:成功:1:失败 
 AUTOTEST_EXPORT unsigned int   auto_test_init(char* err);  


// 功能描述:动态库退出       
// 入口参数 
// 出口参数:
// 返回值:0:成功:1:失败 
 AUTOTEST_EXPORT  unsigned int   auto_test_quit(char* err);  


// 功能描述:设置日志回调函数       
// 入口参数 : callbackfun      :回调函数名称
//            callbackfunparam :回调函数的参数
// 出口参数:
// 返回值:
 AUTOTEST_EXPORT void   auto_test_set_log_callback_fun(LP_LOG_CALLFUN callbackfun ,
													  void*    callbackfunparam	  
													 );  


// 功能描述:设置故障报告回调函数       
// 入口参数 : callbackfun      :回调函数名称
//            callbackfunparam :回调函数的参数
// 出口参数:
// 返回值:
 AUTOTEST_EXPORT void   auto_test_set_fault_report_callback_fun(LP_FAULT_REPORT_CALLFUN callbackfun  ,
															   void*    callbackfunparam 	  
															 );  

 // 功能描述命令中止回调函数       
// 入口参数 : callbackfun      :回调函数名称
//            callbackfunparam :回调函数的参数
// 出口参数:
// 返回值:
 AUTOTEST_EXPORT void   auto_test_set_cmd_termainal_report_callback_fun(LP_CMD_TERMINAL_REPORT_CALLFUN callbackfun  ,
															   void*    callbackfunparam 	  
															 );  


// 功能描述:61850连接装置   
// 入口参数:
//          server_name:服务器名
// 出口参数:
//          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT  unsigned int  auto_test_connect_equip(char *server_name, char* err ); 


// 功能描述:61850断开装置   
// 入口参数:
//          server_name:服务器名
// 出口参数:
//          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_disconnect_equip(char *server_name, char* err); 


// 功能描述:重新装载osicfg.xml文件
// 入口参数:
// 出口参数:
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT  unsigned int  auto_test_reload_osicfg(); 


// 功能描述:装载scl文件 模型  
// 入口参数:
//          server_name       :服务器名
//          scl_file_name     :scd或者cid文件名
//          ied_name          :模型的ied名称
//          access_point_name :MMS的访问点 一般为"S1"
// 出口参数:
//          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_local_scl_model(char *server_name, char *scl_file_name, char *ied_name, char *access_point_name, char* err); 


 // 功能描述:动态调用scl文件 模型  
// 入口参数:
//          server_name       :服务器名
// 出口参数:
//          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_dyn_scl_model(char *server_name, char* err); 


 // 功能描述:动态调用点表  
// 入口参数:
//          server_name       :服务器名
//          info_type         :信息类型 对应INFO_TYPE_ID
// 出口参数:
//          err        :错误描述
 //         pt         :返回获得的点表
//          pt_nums    :点表的数目
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_dyn_pt(char *server_name,unsigned char info_type,ITEM_PARAM **pt,unsigned short *pt_nums, char* err); 


 // 功能描述:释放点表  
// 入口参数:
//          ITEM_PARAM         :信息点指针
// 出口参数:

// 返回值:
 AUTOTEST_EXPORT void  auto_test_free_ptr(void *pt); 






// 功能描述:读取61850叶子节点的值   
// 入口参数:
//          server_name:服务器名称
//          ref        :叶子引用
// 出口参数:
//          str_val    :值
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_read_value(char *server_name,char *ref,char *str_val, char* err);



// 功能描述:读取61850多个叶子节点的值   
// 入口参数:
//          server_name:服务器名称
//          ref        :叶子引用
// 出口参数:
//          str_val    :值
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_read_multi_value(char *server_name,char **ref,int refNums,char **str_val, char* err);

 // 功能描述:读取61850整个数据集的值   
// 入口参数:
//          server_name:服务器名称
//          ref        :叶子引用
// 出口参数:
//          str_val    :值
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_read_dataSet_value(char *server_name,char **dataSet,int dataSetNums,char **ref,int refNums,char **str_val, char* err);


// 功能描述:写61850叶子节点的值   
// 入口参数:
//          server_name:服务器名称
//          ref        :叶子引用
// 出口参数:
//          str_val    :值
//          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT unsigned int  auto_test_write_value(char *server_name,char *ref,char *str_val, char* err);


// 功能描述:修改当前定值组的多个定值
// 入口参数:
//          server_name:服务器名称
//          item       :要修改的数组
// 出口参数:
//          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_modify_setting(char *server_name,ITEM_PARAM *item,unsigned short item_nums, char* err);

 

// 功能描述:修改多个装置参数
// 入口参数:
//          server_name:服务器名称
//          item       :要修改的数组
// 出口参数:
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_modify_equip_param(char *server_name,ITEM_PARAM *item,unsigned short item_nums, char* err);


// 功能描述:修改多个软压板
// 入口参数:
//          server_name:服务器名称
//          item       :要修改的数组
// 出口参数:
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
 AUTOTEST_EXPORT unsigned int  auto_test_modify_yb(char *server_name,ITEM_PARAM *item,unsigned short item_nums,unsigned char ctrl_model, char* err);



 
 // 功能描述: 根据类型获得域，变量名，数据集列表
// 入口参数:
//          server_name:服务器名称
//          name_type         :需要获得名字列表的类型 NAME_LIST_TYPE
//          dom_name          :域名,如果name_type=NLT_DOMAIN_NAME 可以填NULL
// 出口参数:
 //          names      :返回的名字数组
 //          nums       :返回的数组个数
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT  unsigned int auto_test_get_name_list(char *server_name,unsigned char name_type,char *dom_name,char (**names)[128], int *nums,char *err );

 // 功能描述: 释放域，变量名，数据集列表
// 入口参数:
//          server_name:服务器名称
//          name_type         :需要获得名字列表的类型 NAME_LIST_TYPE
//          names             :需要释放的名字数组
 AUTOTEST_EXPORT void  auto_test_free_name_list(unsigned char name_type,char (*names)[128]);

 
// 功能描述: 获得数据目录信息
// 入口参数:
//          server_name:服务器名称
//          var_ref           :叶子引用
//          dir_info          :域名,如果name_type=NLT_DOMAIN_NAME 可以填NULL
// 出口参数:
 //          dir_info   :返回的目录数组
 //          nums       :返回的数组个数
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT unsigned int auto_test_get_data_dir(char *server_name,char *var_ref, AUTO_TEST_DATA_INFO **dir_info, int *nums,char *err);

// 功能描述: 释放数据集目录信息
// 入口参数:
//          dir_info:目录数组
AUTOTEST_EXPORT void auto_test_free_data_dir(AUTO_TEST_DATA_INFO *dir_info);

// 功能描述: 获得数据集下的数据信息
// 入口参数:
//          server_name:服务器名称
//          dataset_ref :叶子引用
//          data_info   :域名,如果name_type=NLT_DOMAIN_NAME 可以填NULL
// 出口参数:
 //          data_info   :返回的目录数组
 //          nums       :返回的数组个数
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT unsigned int auto_test_get_dataset_datainfo(char *server_name, char *dataset_ref, AUTO_TEST_DATA_INFO1 **data_info,int *nums, int desc_get_mode,int leaf_mode);

// 功能描述: 释放数据集
// 入口参数:
//          server_name:服务器名称
//          dataset_ref           :叶子引用
//          data_info          :域名,如果name_type=NLT_DOMAIN_NAME 可以填NULL
// 出口参数:
 //          data_info   :返回的目录数组
 //          nums       :返回的数组个数
 //          err        :错误描述
// 返回值:
//			0:成功:
//          1:失败 
AUTOTEST_EXPORT void auto_test_free_dataset_datainfo(AUTO_TEST_DATA_INFO1 *data_info);



END_C_DECLS




#endif // AUTOTEST_GLOBAL_H
