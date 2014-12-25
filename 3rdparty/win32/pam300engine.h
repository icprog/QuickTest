#ifndef PAM300ENGINE_H
#define PAM300ENGINE_H

#include "pam300engine_global.h"
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QtNetwork>
#include "constant.h"

#define MAX_STATUS_NUM    5     //5 ��״̬
#define MAX_OUTPUT_NUM    8     //V,I ���ͨ����
#define MAX_OUTGOOSE_NUM  6     //��� 6 �� GOOSE ���
#define MAX_GOOSE_SEGMENT 6     // ÿ�ε��� GOOSE �仯��<=6
#define MAX_BININP_NUM    4     // ���4������ӵ�
#define MAX_PORT_NUM      5     // 5 �����

//���Բο���Znzd �� TestModule_message.h�� handdata��h

typedef struct OutChannelPara_tag {
    uint8_t  Wave;         	 //0��sine  �� 1��DC ; 2��harmonic
    float  factor;			 //��������
    float  Freqency;   	 	 //
    float  Value;     	 	 //��Чֵ
    float  Phase;   	     //��λ
    uint8_t VorI;            //0 : voltage ; 1:current
    float  RelPhase;         //
//	float  CurrPhase;		 //��ǰ��λ
//	float  StepPhase;        //2*PI*f*t
//	float  Amp;				 //Value*sqrt(2)/��������
//	int    InstantValue;     //˲ʱֵ
} struOutChannelPara ;
//��ת�� 1 ��ʼ��������ǰ�Ĵ��룬�������Ǵ� 0 ��ʼ
typedef struct StatusControl_tag {
    unsigned char   flag;         //  ����������Bit0: ʱ������,Bit1������������,Bit2���������� :Bit 3: Ӳ�ӵ�
    unsigned int    TimeOutCon;   //  ʱ������ :s
    unsigned char   TimeJump;     //  �� 1 ��ʼ
    unsigned char   TriggerCon;   //  ���������� bit0 -3 ��Ӧ������ 1-4
    unsigned char   TriggerJump;  //  �� 1 ��ʼ
    unsigned char   BinInp;       //  Ӳ�ӵ㣬1: ���ã� 0��δ����
    unsigned char   BinInpJump;  //   ����Ӳ�ӵ������������ڼ�״̬��

} struStatusControl ;

//�򵥴����̶����䣬����Դ�����󣬿�������ָ�룬�����Ƚ��鷳
class CStep
{
public:
    CStep();
    ~CStep();
    struOutChannelPara VSignal[MAX_OUTPUT_NUM];    //V: ˳��:VA1,VA2,VB1,VB2,VC1,VC2,VN1,VN2
    struOutChannelPara ISignal[MAX_OUTPUT_NUM];    //I: ˳��:IA1,IA2,IB1,IB2,IC1,IC2,IN1,IN2
    struStatusControl  Control;                    // ��������
    unsigned char    DigOut;      //Ӳ�ӵ���� 2:�޶��壬1����� �� 0���ض�
    unsigned char    OutBin[4];   //����ӵ� ,ֻ֧�� bool, dbpos
    quint8           FaultType;
};

class PAM300ENGINESHARED_EXPORT PAM300Engine:public QObject
{
    Q_OBJECT
public:
    PAM300Engine();
    ~PAM300Engine();
    //���ݵ�ǰ�ļ�����֯���ģ����͸� ARM����� 0x918c ��0x908c �� 04,05 ���
    bool InitGPP(char* filename);
    //�� ARM ��������
    bool SendCommand(const QByteArray &);
    //��������,
    CStep TestStep[MAX_STATUS_NUM];
    int MaxStep;
    bool ExecSeq();
    bool CheckSeqExec(int statusno,unsigned char *pbuf );
    //��������Ƿ���ɣ� 0x928c
    bool SeqIsFinished(char* buf);
    //ֹͣ���
    bool StopExec();
    //network
    bool SetUdpConnect(int port,const QHostAddress & address );
    QUdpSocket      *socket2001;
    QHostAddress    *hostaddr;
    bool            bResponseFlag;        //�Ƿ��з������ݱ�
    unsigned char   ResPkt[UDP_MAX_LEN];  //�������ݱ� buffer
    ushort          Reslen; //�������ݰ�����
    ushort          Respos; //��ʼλ��
    void SetDoubleAD();            //˫ AD һ��
    void OffSignal(int whichstep);
    bool recv2001(int flag);
    void SetProtocal(int flag);
    //�����������һ��ֵ�����ֵ�Լ� PC,CT
    void SetVIInputWay(int flag,int PT,int CT);  //0: ����ֵ�� 1�� һ��ֵ
    void InitSignal();
    void ReSetPhase(int i1,int i2);    //���� i2������ i1����λ
private slots:
    void CommWith2001(const QByteArray tmp);
private:
    void SetGoose(QDomElement &son1,QByteArray &datatosent);
    void SetSMV(QDomElement &son1,QByteArray &datatosent);
    void SetBinMap(QDomElement &son1,QByteArray &datatosent);
    QByteArray sendcmd01(struOutChannelPara &Channel,int whichstep,int Channelno);
//    QSemaphore semSendNext;
    quint16  myport;
    quint8   curSMVNo;    //��ǰ�� SMV ��
    quint8   curGooseNo;  //��ǰ�� GOOSE ���
    quint8   curInpGoose; //��ǰ�� ����GOOSE��ӳ��
    quint8   curLogiclNo; //��ǰ�߼�ͨ����
    //��ǰ�������� ����1 ��ʼ , bit0 �C bit 4,��� 20��,��ڣ���0 ��ʼ , bit5 �C bit 7
    //0-4 ��Ӧ���  1-5 , ��ӵ�ӳ���й�
    quint8   GooseGroupNoWithPort[MAX_PORT_NUM];
    quint16  SampleRate;
    quint8   fnom;
    uint8_t  Protocal; //0: 9-2 ; 1:FT3
    int      PTPara,CTPara,InputVIFlag;
    bool     commstatus;
    int      T0,T1; //T0:sec  ;T1:ms
    //GOOSE ���ӳ��
    int8_t  OutBinMap[4][2];  //fisrt:��������(0-3 ��Ӧ 1-4)��second:0:goose ���, �� 0 ��ʼ��-1�� ��ʾ��δ���壻1��pos
    void sendcmd09(int whichstep);
};
#endif // PAM300ENGINE_H
