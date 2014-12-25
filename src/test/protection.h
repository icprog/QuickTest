#ifndef PROTECTION_H
#define PROTECTION_H

#include <QVector>
#include <QString>

typedef struct stuct_yb{
    QString name_cn;
    QString name_ref;
    bool state;
}Yb_Data;

typedef struct _META_DATA{

    _META_DATA() {
        Value = 0.000;
        Frequency = 0.000;
        Phase = 0;
    }

    double Value;       //幅值
    double Frequency;   //频率
    int Phase;          //相位

}META_DATA,*PMETA_DATA;

typedef struct _STATE_DATA {

    _STATE_DATA() {
        faultType = 11;
    }

    META_DATA Vsignal[4];	//电压
    META_DATA Isignal[4];	//电流

    int faultType;

    unsigned int timeTrigger;	//时间跳转：-1 = false else true. rated = 5
    int timeJumpTo;		//跳转对象状态编号	0表示不再跳转
    bool nodeTrigger;
    bool nodeState[4];	//节点跳转：{1,0,0,0}
    int nodeJumpTo;		//跳转对象状态编号	0表示不再跳转

    bool binOut[4]; //输出节点开关

}STATE_DATA,*PSTATE_DATA;

class Protection// : public AbstractProtection
{
public:
    Protection();

    unsigned int faultType() { return type;}
    void appendState(PSTATE_DATA p) { states.append(p);}

public:
    unsigned int type;
    QString typeName;
    QString instName;
    QString testName;
    QVector<PSTATE_DATA>  states;//4个状态

    QList<Yb_Data*> yb;
};

#endif // PROTECTION_H
