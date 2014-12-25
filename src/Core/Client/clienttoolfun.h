#ifndef CLIENTTOOLFUN_H
#define CLIENTTOOLFUN_H

#include "clientdatadefine.h"


//打印信息 
void ToolFun_TraceInfo(t_char* model,t_char *str,t_uint8 debug = _DEBUG_);
//打印报文
void ToolFun_TraceBirary(t_uint8 *buf,t_uint32 len,bool bSend,t_uint32 port,t_uint8 debug = _DEBUG_);


//创建线程
 bool Comm_CreateThread(_HANDLE &handle ,ptrFun funName,void *funParam);

//关闭线程
 bool Comm_exitThread(_HANDLE &handle);

 ulong Comm_GetTickCount();



#endif // TOOLFUN_H