#ifndef QUICKTESTINTERFACE_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include "protection.h"
#include "../Core/csubstationserver.h"
//#include "yboperation.h"

#include <QtNetwork>
#include "pam300engine.h"
#include "PAM300Engine_global.h"



class QuickTestInterface : public QObject
{
    Q_OBJECT
public:
    explicit QuickTestInterface(QObject *parent = 0);

    bool loadScheme(const QString &path, QString &errorString);

    QAbstractItemModel* treeModel() { return tree;}

    QMap<int,Protection*> selectedItems();//获取选中的测试项目

signals:
    void parseDone();

    void hint(QString);

    void error(QString);

    void connectServer();
    void connectFailure(QString);

    void pullRemoteYbData();

private slots:
    void itemCheckStateChanged(QStandardItem * item);


    void start();
    void stop();

    void connectStateChanged(int);
    void connectServerFailure(const QString &errString);

    void pullYbDataSuccess();

    void pushYbDataSuccess();

private:
//    QVector<QString,int> aa;
    QMap<int,Protection*> data;
    QMap<int,Protection*> testData;
    QStandardItemModel *tree;
    QMap<QString, Yb_Data*> yb_Server;
    QList<Yb_Data*> yb_Local;

    QList<Yb_Data*> m_modify;//当前测试被修改的软压板列表
//    QList<Yb_Data*> yb_Server;

    CSubstationServer   *server;

    int curSection;
    Protection *curProtection;
    bool bAutoYb;

    enum {
        MODIFY = 0,
        RESTORE = 1
    };
    int ybFlag;

private:
    bool parseTestObject(QXmlStreamReader &r, Protection &protection);
    bool parseYbObject(QXmlStreamReader &r, Protection &protection);
    bool parseStateObject(QXmlStreamReader &r, Protection &protection);
    bool parseBinOutObject(QXmlStreamReader &r, PSTATE_DATA);

    void buildTree();

    void itemCheckAllChild(QStandardItem * item, bool check);//子项全选
    void itemCheckAllChild_recursion(QStandardItem * item,bool check);//子项递归全选
    void itemCheckChildChanged(QStandardItem * item);//判断兄弟节点的情况
    Qt::CheckState itemCheckSibling(QStandardItem * item);//判断兄弟节点的关系

    void selectedItem_recursion(QStandardItem *item, QMap<int,Protection*> &map);

    void initPAM300Engine();

    void loopForTest();

    void pushYbData();

    void commitYb(const QList<Yb_Data *> &ybList);

    void sendMsg();
};

#endif // QUICKTESTINTERFACE_H
