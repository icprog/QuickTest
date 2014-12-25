#ifndef CSUBSTATIONSERVER_H
#define CSUBSTATIONSERVER_H

#include <QObject>
#include <QMutex>
#include <QApplication>
#include "./Client/clientcmdthread.h"

#include "csoftplate.h"

#define DIR_CLIENT_CONFIG ("config")


//class CSubstationSoftPlate : public CSoftPlate
//{
//    Q_OBJECT
//public:
//    CSubstationSoftPlate(QObject *parent = 0);
//    ~CSubstationSoftPlate();
//private:

//};

class CSubstationServer : public QObject
{
    Q_OBJECT
public:
    static CSubstationServer* getInstance()
    {
        if (m_pInstance == NULL) {
            m_pInstance = new CSubstationServer();
        }
        return m_pInstance;
    }
private:
    CSubstationServer(QObject *parent = 0);
    CSubstationServer(const CSubstationServer &);
    CSubstationServer & operator=(const CSubstationServer &);
    static CSubstationServer *m_pInstance;

    QMutex mutex;//实例互斥锁。

/* Interface */
public:
    typedef enum {
        CONNECT = 0,
        WAIT = 1,
        DISCONNECT = 2
    }eConnectState;

    /* initialize */
    bool init();

    /* network */
    bool isConnected() {return bConnectState;}
    QString ied() {return QString(m_sclPath.ied_name);}
    QString accessPoint() {return QString(m_sclPath.access_point_name);}
    QString IP() {return QString(m_curIpAddr);}
    void setIp(QString strIP);

    void querySoftPlate(bool isExport);

    PEQU_PARAM data() {return m_pThread->getEquipParam(m_curServerName.toAscii().data());}

    /* subClass */


    ClientCmdThread* workThread() {return m_pThread;}
    QString          serverName() {return m_curServerName;}

signals:
    void signalConnectStateChanged(int);
    void signalConnectFaild(QString);
    void signalHint(QString);

    void signalServerChanged();
    void signalSoftPlateDataReady();

    void signalExportSoftPlateDone(bool);

    void modifyYbSuccess();

private slots:
    void slotConnect();
    void slotDisconnect();

    void resultData_slots(CMD_GEN *);

    void slotRequireSoftPlateData();

private:
    //Core
    DSE_SysImpl m_sysImpl;

    ClientCmdThread  *m_pThread;                   //命令处理线程
    SCL_PATH   m_sclPath;
    QString    m_curServerName;              //当前服务器名
    QString    m_curIpAddr;                  //当前服务器名

    QString m_domain;
    QString m_sta;
    QString m_equ;

    void readClientInfo();
    void loadClientInfo();
    void updateOsicfg();

    /* user */
    bool bConnectState;

    bool bExportSoftPlate;
    bool exportSoftPlate();

};

#endif // CSUBSTATIONSERVER_H
