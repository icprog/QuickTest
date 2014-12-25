#ifndef CTEMPLATEMODEL_H
#define CTEMPLATEMODEL_H

#include <QList>
#include <QStringList>
#include <stdio.h>


typedef struct struct_smvData{
    int faultType;
    float Va[3];
    float Vb[3];
    float Vc[3];
    float Vn[3];
    float Ia[3];
    float Ib[3];
    float Ic[3];
    float In[3];
}SMVDATA;

class CCircuit
{
public:
    CCircuit();
    ~CCircuit();

    bool isEnable() {return bState;}
    void setEnable(bool);

private:
    bool bState;//线路状态(可用/不可用)
};

class CCircuitGroup
{
public:
    CCircuitGroup();
    ~CCircuitGroup();

    int circuitCount() {return nCircuitCount;}
    CCircuit*   circuit(int);

protected:
    int nCircuitCount;
    CCircuit    m_circuit[4];
private:
    void setCircuitCount(int circuitCount);
};

//class CCircuitInput : public CCircuitGroup
//{
//public:
//    CCircuitInput();
//    ~CCircuitInput();
//};

//class CCircuitOutput : public CCircuitGroup
//{
//public:
//    CCircuitOutput();
//    ~CCircuitOutput();
//};

//class CActionBoardState
//{
//public:
//    CActionBoardState();
//    ~CActionBoardState();

//    bool isServerConnect() {return bServerConnect;}

//private:
//    bool bServerConnect;
//    bool update();  //在已连接的状态下从服务器更新软压板信息
//};

class CSoftPlate
{
public:
    CSoftPlate();
    CSoftPlate(QString desc, int value, QString reference);

    QString desc() {return strDescribe;}
    int value() {return nValue;}
    QString reference() {return strReference;}

private:
    QString strDescribe;
    int     nValue;
    QString strReference;
};
#include <QList>
class CActionSoftPlate
{
public:
    CActionSoftPlate();
    ~CActionSoftPlate();

    typedef enum {
        Each,
        Before,
        After
    }eSoftPlate;

    QList<CSoftPlate*> beforList() { return m_lSoftPlatesBefor;}
    QList<CSoftPlate*> afterList() { return m_lSoftPlatesAfter;}
    void clearBefore();
    void clearAfter();
    void clear();
    void append(eSoftPlate position, QString desc, int value, QString reference);
//    bool isEmpty(eSoftPlate position);
protected:
    QList<CSoftPlate*> m_lSoftPlatesBefor;
    QList<CSoftPlate*> m_lSoftPlatesAfter;
};

typedef struct struct_statecontrol
{
    struct_statecontrol()
    {
        bTimeControl = true;
        timeValue = 5;
        bActionControl = true;
        bJumpPoint[0] = true;
        bJumpPoint[1] = false;
        bJumpPoint[2] = false;
        bJumpPoint[3] = false;
    }
    bool bTimeControl;
    int  timeValue;
    bool bActionControl;
    bool bJumpPoint[4];
}sStateContorl;

class CStateControl
{
    //该类需重构成 子：CTimeStateControl CActionStateControl 父：CStateControl
public:
    CStateControl();
    ~CStateControl();

    void setTimeEnable(int index,int);
    void setTimeDisable(int index);
    void setActionEnable(int index,bool p1,bool p2,bool p3,bool p4);
    void setActionDisable(int index);

    bool timeState(int index) {return m_stateContorl[index].bTimeControl;}
    int  timeValue(int index) {return m_stateContorl[index].timeValue;}
    bool actionState(int index) {return m_stateContorl[index].bActionControl;}
    bool actionPointState(int index, int num) {return m_stateContorl[index].bJumpPoint[num];}

private:
    sStateContorl       m_stateContorl[4];
};

//#define Fault_AE  0
//#define Fault_BE  1
//#define Fault_CE  2
//#define Fault_AB  3
//#define Fault_BC  4
//#define Fault_CA  5
//#define Fault_ABE  6
//#define Fault_BCE  7
//#define Fault_CAE  8
//#define Fault_ABC  9
//#define Fault_NORMAL 10
//#define Fault_ANY 11

class CTestAction
{
public:
    CTestAction(QString,QString,QString,int,bool,bool);
    QStringList name() {return QStringList() << m_className << m_statusName << m_actionName;}
    int id() {return m_actionNum;}

    bool isAutoBoard() {return bAutoBoard;}
    bool isAutoTest() {return bAutoTest;}
    bool isConfiged() {return bCofiged;}

    CActionSoftPlate*   softPlate() {return &m_softPlate;}     //软压板
    CStateControl*      states() {return &m_stateControl;}
    CCircuitGroup*      circuit_state(int);

    void setDefaultValue();
    SMVDATA    smvData[4];

//    typedef enum {
//        Fault_AE = 0,   //A相接地
//        Fault_BE = 1,   //B相接地
//        Fault_CE = 2,   //C相接地

//        Fault_AB = 3,   //AB短路
//        Fault_BC = 4,   //BC短路
//        Fault_CA = 5,   //CA短路

//        Fault_ABE = 6,  //AB接地
//        Fault_BCE = 7,  //BC接地
//        Fault_CAE = 8,  //CA接地
//        Fault_ABC = 9,  //ABC三相接地
//        Fault_NORMAL = 10,  //正常
//        Fault_ANY = 11  //任意
//    }eFAULT_TYPE;
private:
    QString m_className;
    QString m_statusName;
    QString m_actionName;
    int m_actionNum;
    bool bAutoBoard;
    bool bAutoTest;
    bool bCofiged;

    CActionSoftPlate   m_softPlate;
    CStateControl       m_stateControl;

    CCircuitGroup       m_circuit[4];
};

class CTemplateModel
{
public:
    CTemplateModel();

    bool loadData(QString path);
    void clear();
    void deleteAction(int testNum);
    bool appendAction(CTestAction*);
    QList<CTestAction*> actions() {return m_list;}

    CTestAction* findActionByName(QString name);
    void setCurrentAction(CTestAction* action) {m_curAction = action;}

    CTestAction* currentAction() { return m_curAction;}

    bool save(QString name);
private:
    QList<CTestAction*> m_list;
    CTestAction *m_curAction;
};

#endif // CTEMPLATEMODEL_H
