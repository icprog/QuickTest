#ifndef PAM300SHARE_GLOBAL_H
#define PAM300SHARE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qstring.h>

#if defined(PAM300SHARE_LIBRARY)
#  define PAM300SHARESHARED_EXPORT Q_DECL_EXPORT
#else
#  define PAM300SHARESHARED_EXPORT Q_DECL_IMPORT
#endif
#define  conNormalV     57.735
#define  conPI          3.14159

#define AB_VA_PHASE  -5.26
#define AB_VB_PHASE  -114.74
#define AB_VC_PHASE  120
#define BC_VA_PHASE  0
#define BC_VB_PHASE  -125.26
#define BC_VC_PHASE  125.26
#define CA_VA_PHASE  5.26
#define CA_VB_PHASE  -120
#define CA_VC_PHASE  114.74

#define Fault_AE  0
#define Fault_BE  1
#define Fault_CE  2
#define Fault_AB  3
#define Fault_BC  4
#define Fault_CA  5
#define Fault_ABE  6
#define Fault_BCE  7
#define Fault_CAE  8
#define Fault_ABC  9
#define Fault_NORMAL 10
#define Fault_ANY 11

typedef struct StruInputPara1 {
    StruInputPara1()
    {
        memset(this,0,sizeof(StruInputPara1));
    }
    int   Type;      // 参考文档 “保护功能(加备注).xlsx”
    int   FaultType; //0-9
    float Para[8];   //输入参数
    float KPara[3];  //零序补偿系数 ， 0： 模式， 1，2 参数
} StruInputPara;

typedef struct StruOutPara1{
    StruOutPara1()
    {
      //  memset(this,0,sizeof(StruOutPara1));
    }
    float V[4];  //0:A,1:B;2:C;3:N
    float VPhase[4];
    float I[4]; //0:A,1:B;2:C;3:N
    float IPhase[4];
} StruOutPara;




#endif // PAM300SHARE_GLOBAL_H
