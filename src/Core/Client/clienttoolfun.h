#ifndef CLIENTTOOLFUN_H
#define CLIENTTOOLFUN_H

#include "clientdatadefine.h"


//��ӡ��Ϣ 
void ToolFun_TraceInfo(t_char* model,t_char *str,t_uint8 debug = _DEBUG_);
//��ӡ����
void ToolFun_TraceBirary(t_uint8 *buf,t_uint32 len,bool bSend,t_uint32 port,t_uint8 debug = _DEBUG_);


//�����߳�
 bool Comm_CreateThread(_HANDLE &handle ,ptrFun funName,void *funParam);

//�ر��߳�
 bool Comm_exitThread(_HANDLE &handle);

 ulong Comm_GetTickCount();



#endif // TOOLFUN_H