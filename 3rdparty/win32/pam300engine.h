#ifndef PAM300ENGINE_H
#define PAM300ENGINE_H

#include "pam300engine_global.h"
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QtNetwork>
#include "constant.h"

#define MAX_STATUS_NUM    5     //5 个状态
#define MAX_OUTPUT_NUM    8     //V,I 最大通道数
#define MAX_OUTGOOSE_NUM  6     //最大 6 个 GOOSE 输出
#define MAX_GOOSE_SEGMENT 6     // 每次单个 GOOSE 变化量<=6
#define MAX_BININP_NUM    4     // 最多4个输入接点
#define MAX_PORT_NUM      5     // 5 个光口

//可以参考：Znzd 的 TestModule_message.h， handdata。h

typedef struct OutChannelPara_tag {
    uint8_t  Wave;         	 //0：sine  ； 1：DC ; 2：harmonic
    float  factor;			 //比例因子
    float  Freqency;   	 	 //
    float  Value;     	 	 //有效值
    float  Phase;   	     //相位
    uint8_t VorI;            //0 : voltage ; 1:current
    float  RelPhase;         //
//	float  CurrPhase;		 //当前相位
//	float  StepPhase;        //2*PI*f*t
//	float  Amp;				 //Value*sqrt(2)/比例因子
//	int    InstantValue;     //瞬时值
} struOutChannelPara ;
//跳转从 1 开始，兼容以前的代码，其它都是从 0 开始
typedef struct StatusControl_tag {
    unsigned char   flag;         //  设置条件，Bit0: 时间条件,Bit1：开关量条件,Bit2：按键条件 :Bit 3: 硬接点
    unsigned int    TimeOutCon;   //  时间条件 :s
    unsigned char   TimeJump;     //  从 1 开始
    unsigned char   TriggerCon;   //  动作条件： bit0 -3 对应开关量 1-4
    unsigned char   TriggerJump;  //  从 1 开始
    unsigned char   BinInp;       //  硬接点，1: 设置， 0：未设置
    unsigned char   BinInpJump;  //   满足硬接点条件，跳到第几状态，

} struStatusControl ;

//简单处理：固定分配，如资源开销大，可以是用指针，这样比较麻烦
class CStep
{
public:
    CStep();
    ~CStep();
    struOutChannelPara VSignal[MAX_OUTPUT_NUM];    //V: 顺序:VA1,VA2,VB1,VB2,VC1,VC2,VN1,VN2
    struOutChannelPara ISignal[MAX_OUTPUT_NUM];    //I: 顺序:IA1,IA2,IB1,IB2,IC1,IC2,IN1,IN2
    struStatusControl  Control;                    // 控制条件
    unsigned char    DigOut;      //硬接点输出 2:无定义，1：输出 ， 0：关断
    unsigned char    OutBin[4];   //输出接点 ,只支持 bool, dbpos
    quint8           FaultType;
};

class PAM300ENGINESHARED_EXPORT PAM300Engine:public QObject
{
    Q_OBJECT
public:
    PAM300Engine();
    ~PAM300Engine();
    //根据当前文件，组织报文，下送给 ARM（完成 0x918c 和0x908c 的 04,05 命令）
    bool InitGPP(char* filename);
    //向 ARM 发送命令
    bool SendCommand(const QByteArray &);
    //测试序列,
    CStep TestStep[MAX_STATUS_NUM];
    int MaxStep;
    bool ExecSeq();
    bool CheckSeqExec(int statusno,unsigned char *pbuf );
    //检查序列是否完成， 0x928c
    bool SeqIsFinished(char* buf);
    //停止输出
    bool StopExec();
    //network
    bool SetUdpConnect(int port,const QHostAddress & address );
    QUdpSocket      *socket2001;
    QHostAddress    *hostaddr;
    bool            bResponseFlag;        //是否有返回数据报
    unsigned char   ResPkt[UDP_MAX_LEN];  //返回数据报 buffer
    ushort          Reslen; //返回数据包长度
    ushort          Respos; //起始位置
    void SetDoubleAD();            //双 AD 一致
    void OffSignal(int whichstep);
    bool recv2001(int flag);
    void SetProtocal(int flag);
    //设置输入的是一次值或二次值以及 PC,CT
    void SetVIInputWay(int flag,int PT,int CT);  //0: 二次值； 1： 一次值
    void InitSignal();
    void ReSetPhase(int i1,int i2);    //根据 i2，设置 i1的相位
private slots:
    void CommWith2001(const QByteArray tmp);
private:
    void SetGoose(QDomElement &son1,QByteArray &datatosent);
    void SetSMV(QDomElement &son1,QByteArray &datatosent);
    void SetBinMap(QDomElement &son1,QByteArray &datatosent);
    QByteArray sendcmd01(struOutChannelPara &Channel,int whichstep,int Channelno);
//    QSemaphore semSendNext;
    quint16  myport;
    quint8   curSMVNo;    //当前的 SMV 号
    quint8   curGooseNo;  //当前的 GOOSE 组号
    quint8   curInpGoose; //当前的 输入GOOSE的映射
    quint8   curLogiclNo; //当前逻辑通道号
    //当前光口下组号 ，从1 开始 , bit0 – bit 4,最大 20组,光口，从0 开始 , bit5 – bit 7
    //0-4 对应光口  1-5 , 与接点映射有关
    quint8   GooseGroupNoWithPort[MAX_PORT_NUM];
    quint16  SampleRate;
    quint8   fnom;
    uint8_t  Protocal; //0: 9-2 ; 1:FT3
    int      PTPara,CTPara,InputVIFlag;
    bool     commstatus;
    int      T0,T1; //T0:sec  ;T1:ms
    //GOOSE 输出映射
    int8_t  OutBinMap[4][2];  //fisrt:开关量号(0-3 对应 1-4)；second:0:goose 组号, 从 0 开始，-1， 表示：未定义；1：pos
    void sendcmd09(int whichstep);
};
#endif // PAM300ENGINE_H
