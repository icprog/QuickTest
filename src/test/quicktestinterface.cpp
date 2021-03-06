#include "quicktestinterface.h"
#include <QFile>
#include <QDebug>

#include "filepath.h"
PAM300Engine engine;

QuickTestInterface::QuickTestInterface(QObject *parent) :
    QObject(parent),
    tree(0),
    server(0),
    curProtection(0),
    ybFlag(MODIFY),
    bAutoYb(true)
{
    data.clear();

    server = CSubstationServer::getInstance();

    connect(this, SIGNAL(connectServer()), server, SLOT(slotConnect()));
    connect(server, SIGNAL(signalConnectStateChanged(int)), SLOT(connectStateChanged(int)));
    connect(server, SIGNAL(signalConnectFaild(QString)), SLOT(connectServerFailure(QString)));
    connect(this, SIGNAL(pullRemoteYbData()), server, SLOT(slotRequireSoftPlateData()));
    connect(server, SIGNAL(signalSoftPlateDataReady()), SLOT(pullYbDataSuccess()));
    connect(server, SIGNAL(modifyYbSuccess()), this, SLOT(pushYbDataSuccess()));

//    connect(this, SIGNAL(connectServer()), &yb, SLOT(connectServer()));
}

bool QuickTestInterface::loadScheme(const QString &path, QString &errorString)
{
    QFile file(path);

    if (!file.open(QFile::ReadOnly|QFile::Text)) {
        return false;
    }

    data.clear();

    QXmlStreamReader r(&file);

    r.readNext();

    while (!r.atEnd() && !r.hasError()) {

        if (r.name() == "Test") {

            Protection p;
            if (this->parseTestObject(r,p)) {
                //解析Test正确
                Protection *item = new Protection(p);
                this->data.insert(item->faultType(),item);
            } else {
                //解析到错误的Test块，待完善
                //提供一个解析错误的Map表
            }

        }

        r.readNextStartElement();
    }

    if (r.hasError()) {
        errorString = r.errorString();
    }

    file.close();

    this->buildTree();
//    qDebug() << data.count();
    emit parseDone();

    return true;
}

bool QuickTestInterface::parseTestObject(QXmlStreamReader &r, Protection &protection)
{

    Q_ASSERT_X(!r.atEnd(), "parseTestObject()", "r.atEnd()");

    QXmlStreamAttributes atts = r.attributes();
    QString fullName = atts.value("fullName").toString();
    QStringList names = fullName.split("|", QString::SkipEmptyParts);
    protection.type = atts.value("type").toString().toUInt();
    if (names.count() != 3) {
        return false;
    }

    protection.typeName = names.first();
    protection.instName = names.at(1);
    protection.testName = atts.value("name").toString();

    r.readNextStartElement();
    while (!r.atEnd() && !r.hasError()) {

        QXmlStreamReader::TokenType t = r.tokenType();

        if (t == QXmlStreamReader::StartElement) {

            if (r.name() == "SoftPlate") {

                if (!this->parseYbObject(r, protection)) {

                    return false;

                }

            } else if (r.name() == "State") {

                if (!this->parseStateObject(r,protection)) {

                    return false;

                }

            }

        } else if (t == QXmlStreamReader::EndElement) {

            if (r.name() == "Test") {
                break;
            }

        }

        r.readNext();
    }
    return true;

}

bool QuickTestInterface::parseYbObject(QXmlStreamReader &r, Protection &protection)
{

    Q_ASSERT_X(!r.atEnd(), "parseYbObject()", "r.atEnd()");

    r.readNextStartElement();
    while (!r.atEnd() && !r.hasError()) {

        QXmlStreamReader::TokenType t = r.tokenType();
        if (t == QXmlStreamReader::StartElement) {

            if (r.name() == "Plate") {

                Yb_Data *yb = new Yb_Data;
                yb->name_cn = r.attributes().value("desc").toString();
                yb->name_ref = r.attributes().value("ref").toString();
                yb->state = (r.attributes().value("value").toString() == "1")?true:false;

                protection.yb.append(yb);

            }

        } else if (t == QXmlStreamReader::EndElement) {

            if (r.name() == "Before")
                break;

        }

        r.readNext();
    }


    return true;
}

bool QuickTestInterface::parseStateObject(QXmlStreamReader &r, Protection &protection)
{

    Q_ASSERT_X(!r.atEnd(), "parseStateObject()", "r.atEnd()");

    PSTATE_DATA state = new STATE_DATA;

    int faultType = r.attributes().value("faultType").toString().toInt();
    state->faultType = ((faultType < 12)&&(faultType >= 0))?faultType:11;

    r.readNextStartElement();

    int i = 0,v = 0;
    while (!r.atEnd() && !r.hasError()) {

        QXmlStreamReader::TokenType t = r.tokenType();

        if (t == QXmlStreamReader::StartElement) {

            if (r.name() == "Chn") {

                if (r.attributes().value("class").toString() == "i") {

                    state->Isignal[i].Value = r.attributes().value("value").toString().toDouble();
                    state->Isignal[i].Frequency = r.attributes().value("frequency").toString().toDouble();
                    state->Isignal[i].Phase = r.attributes().value("phase").toString().toInt();
//                    qDebug() << state->Isignal[i].Value << state->Isignal[i].Frequency << state->Isignal[i].Phase;
                    ++i;

                } else {

                    state->Vsignal[v].Value = r.attributes().value("value").toString().toDouble();
                    state->Vsignal[v].Frequency = r.attributes().value("frequency").toString().toDouble();
                    state->Vsignal[v].Phase = r.attributes().value("phase").toString().toInt();

                    ++v;

                }
            }
            else if (r.name() == "BinOut") {

                this->parseBinOutObject(r, state);

            }
            else if (r.name() == "Time") {

                state->timeTrigger = r.attributes().value("value").toString().toInt();
                state->timeJumpTo = r.attributes().value("jump").toString().toInt();

            }
            else if (r.name() == "Action") {

                state->nodeTrigger = true;
                state->nodeJumpTo = r.attributes().value("jump").toString().toInt();

                state->nodeState[0] = (r.attributes().value("bin1").toString() == "true")?true:false;
                state->nodeState[1] = (r.attributes().value("bin2").toString() == "true")?true:false;
                state->nodeState[2] = (r.attributes().value("bin3").toString() == "true")?true:false;
                state->nodeState[3] = (r.attributes().value("bin4").toString() == "true")?true:false;

            }
        }
        else if (t == QXmlStreamReader::EndElement) {
            if (r.name() == "State") {

                break;

            }
        }

        r.readNext();
    }

    if (r.hasError()) {
        delete state;
        state = NULL;
        return false;
    }

    protection.appendState(state);
    /*
    for(int i = 0; i < 4; i++) {
        qDebug() << "I" << state->Isignal[i].Value << "V" << state->Vsignal[i].Value;

    }*/
    return true;
}

bool QuickTestInterface::parseBinOutObject(QXmlStreamReader &r, PSTATE_DATA state)
{
    Q_ASSERT_X(!r.atEnd(), "parseBinOutObject()", "r.atEnd()");

    r.readNextStartElement();

    while (!r.atEnd() && !r.hasError()) {

        QXmlStreamReader::TokenType t = r.tokenType();

        if (t == QXmlStreamReader::StartElement) {

            if (r.name() == "Which") {

                int num = r.attributes().value("no").toString().toInt() - 1;
                state->binOut[num] = (r.attributes().value("value").toString().toInt() == 0)?false:true;

            }

        }
        else if (t == QXmlStreamReader::EndElement) {
            if (r.name() == "BinOut")
                break;
        }

        r.readNext();
    }

    return true;
}

void QuickTestInterface::buildTree()
{

    qDebug() << data.keys();
    if (tree) {
        delete tree;
        tree = NULL;
    }

    tree = new QStandardItemModel;

    connect(tree, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemCheckStateChanged(QStandardItem*)));
    tree->setHorizontalHeaderItem(0, new QStandardItem(QObject::trUtf8("测试项目")));

    foreach (Protection *p, data) {

            QStandardItem *itemType = NULL;
            QStandardItem *itemInst = NULL;
            QStandardItem *itemTest = NULL;

        QList<QStandardItem*> listType = tree->findItems(p->typeName, Qt::MatchRecursive, 0);
        if (listType.isEmpty()) {
            //新建Type节点
            itemType = new QStandardItem(p->typeName);
            itemType->setSelectable(false);
            itemType->setEditable(false);
            itemType->setCheckable(true);
            itemType->setTristate(true);
            tree->appendRow(itemType);
        } else {
            itemType = listType.first();
            if (!itemType) {
                continue;
            }
        }

        if (!itemType) {
            continue;
        }
        QList<QStandardItem*> listInst = tree->findItems(p->instName, Qt::MatchRecursive, 0);
        if (listInst.isEmpty()) {

            //新建Inst节点
            itemInst = new QStandardItem(p->instName);
            itemInst->setSelectable(false);
            itemInst->setEditable(false);
            itemInst->setCheckable(true);
            itemInst->setTristate(true);
            itemType->appendRow(itemInst);

        } else {

            foreach (QStandardItem*match, listInst) {
                QStandardItem *matchType = match->parent();
                if (matchType && (matchType->data(Qt::DisplayRole).toString() == p->typeName)) {
                    itemInst = match;
                }
            }

            if (!itemInst) {
                //新建Inst节点
                itemInst = new QStandardItem(p->instName);
                itemInst->setSelectable(false);
                itemInst->setEditable(false);
                itemInst->setCheckable(true);
                itemInst->setTristate(true);
                itemType->appendRow(itemInst);
            }

        }

        if (!itemInst) {
            continue;
        }
        itemTest = new QStandardItem(p->testName);
        itemTest->setEditable(false);
        itemTest->setCheckable(true);
        itemTest->setData(p->faultType(), Qt::UserRole+1);
        itemInst->appendRow(itemTest);
//        qDebug() << itemTest->isCheckable();

    }


    return;

}

void QuickTestInterface::itemCheckStateChanged(QStandardItem *item)
{

    if (item == NULL)
        return ;

    if (item->isCheckable())
    {
        //如果条目是存在复选框的，那么就进行下面的操作
        Qt::CheckState state = item->checkState(); //获取当前的选择状态
        if (item->isTristate()) {
            //如果条目是三态的，说明可以对子目录进行全选和全不选的设置
            if (state != Qt::PartiallyChecked) {
                //当前是选中状态，需要对其子项目进行全选
                itemCheckAllChild(item, state == Qt::Checked?true:false );
            }
        } else {
            //说明是两态的，两态会对父级的三态有影响
            //判断兄弟节点的情况
            itemCheckChildChanged(item);
        }

    }

    return;
}

void QuickTestInterface::itemCheckAllChild(QStandardItem * item, bool check)
{
    if(item == NULL)
        return;

    int rowCount = item->rowCount();
    for(int i=0;i<rowCount;++i)
    {

        QStandardItem* childItems = item->child(i);
        itemCheckAllChild_recursion(childItems,check);

    }

    if(item->isCheckable()) {
        item->setCheckState(check?(Qt::Checked):(Qt::Unchecked));
    }

    return;
}

void QuickTestInterface::itemCheckAllChild_recursion(QStandardItem * item,bool check)
{
    if(item == NULL)
        return;

    int rowCount = item->rowCount();
    for(int i=0;i<rowCount;++i)
    {

        QStandardItem* childItems = item->child(i);
        itemCheckAllChild_recursion(childItems,check);

    }

    if(item->isCheckable()) {
        item->setCheckState(check?(Qt::Checked):(Qt::Unchecked));
    }

    return;
}

void QuickTestInterface::itemCheckChildChanged(QStandardItem * item)
{
    if(NULL == item)
        return;

    Qt::CheckState siblingState = itemCheckSibling(item);
    QStandardItem * parentItem = item->parent();

    if(NULL == parentItem)
        return;

    if(Qt::PartiallyChecked == siblingState)
    {

        if(parentItem->isCheckable() && parentItem->isTristate()) {
            parentItem->setCheckState(Qt::PartiallyChecked);
        }

    } else if(Qt::Checked == siblingState) {
        if(parentItem->isCheckable()) {
            parentItem->setCheckState(Qt::Checked);
        }
    } else {
        if(parentItem->isCheckable()) {
            parentItem->setCheckState(Qt::Unchecked);
        }
    }

    itemCheckChildChanged(parentItem);

    return;
}

Qt::CheckState QuickTestInterface::itemCheckSibling(QStandardItem * item)
{

    //先通过父节点获取兄弟节点
    QStandardItem * parent = item->parent();
    if(NULL == parent)
        return item->checkState();

    int brotherCount = parent->rowCount();
    int checkedCount(0),unCheckedCount(0);
    Qt::CheckState state;

    for(int i=0;i<brotherCount;++i) {

        QStandardItem* siblingItem = parent->child(i);
        state = siblingItem->checkState();

        if(Qt::PartiallyChecked == state) {
            return Qt::PartiallyChecked;
        } else if(Qt::Unchecked == state) {
            ++unCheckedCount;
        } else {
            ++checkedCount;
        }

        if(checkedCount>0 && unCheckedCount>0) {
            return Qt::PartiallyChecked;
        }

    }

    if(unCheckedCount>0) {
        return Qt::Unchecked;
    }

    return Qt::Checked;

}

QMap<int,Protection*> QuickTestInterface::selectedItems()
{
    QMap<int,Protection*> map;

    if (!tree) {
        return map;
    }

    int cntType = tree->rowCount();
    for(int i=0;i<cntType;++i) {
        QStandardItem *itemType = tree->item(i,0);
        if (itemType->hasChildren()) {

            selectedItem_recursion(itemType, map);

        }
    }
//    foreach (QStandardItem *itemType, tree->rowCount()) {

//    }
//    qDebug() << "keys" << map.keys();
    return map;
}

void QuickTestInterface::selectedItem_recursion(QStandardItem *item, QMap<int,Protection*> &map)
{
    int cnt = item->rowCount();
    for (int i=0;i<cnt;++i) {
        QStandardItem *instance = item->child(i,0);
        if (instance && instance->hasChildren()) {
            selectedItem_recursion(instance, map);
        } else if (instance && !instance->hasChildren()){

            if (instance->checkState() == Qt::Checked) {
                map.insert(instance->data(Qt::UserRole+1).toInt(), data.value(instance->data(Qt::UserRole+1).toInt()));
            }
        }
    }
}



void QuickTestInterface::start()
{

    //获取测试列表
    emit hint(QObject::trUtf8("获取测试列表...\n"));
    testData = this->selectedItems();

    //检查测试列表
    emit hint(QObject::trUtf8("检查列表...\n"));
    if (testData.isEmpty()) {
        emit error(QObject::trUtf8("测试列表为空"));
        return;
    }

    emit hint(QObject::trUtf8("连接服务器...\n"));

    //连接服务器
    emit connectServer();

    return;
}

void QuickTestInterface::stop()
{
    engine.StopExec();
}

void QuickTestInterface::connectStateChanged(int state)
{

    if (state != CSubstationServer::CONNECT) {
        return;
    }

    emit hint(QObject::trUtf8("连接成功,获取远端软压板列表...\n"));
//    qDebug() << "connected";

//    bNeedRestoreYb = false;

    //获取软压板数据
    emit pullRemoteYbData();

}
#include <QMessageBox>
void QuickTestInterface::connectServerFailure(const QString &errString)
{

    int ret = QMessageBox::question(0, trUtf8("暂停"),\
                                    QString("%1\n%2").arg(errString).arg(trUtf8("连接服务器失败!\n使用手动软压板模式,请在完成修改软压板后点击\"忽略\"按钮继续测试")),\
                          QMessageBox::Abort,\
                          QMessageBox::Ignore);

    if (ret == QMessageBox::Abort) {

        emit connectFailure(trUtf8("服务器连接失败"));

    }
    else if (ret == QMessageBox::Ignore) {

        bAutoYb = false;

        //初始化报文模块（开启测试循环,在每次循环开始时询问用户软压板设置是否完成！)
        initPAM300Engine();

    } else {

        emit error(trUtf8("程序异常"));

    }

    return;
}

void QuickTestInterface::pullYbDataSuccess()
{
    PEQU_PARAM pParam = server->data();

    if (!pParam)
    {
        emit error(QObject::trUtf8("从服务器获取数据失败"));
        return;
    }

//    qDebug() << "success";

//    yb_Server.clear();
    foreach(Yb_Data *tmp_yb, yb_Server) {
        delete tmp_yb;
        tmp_yb = NULL;
    }

    for (int i = 0 ; i < pParam->yb_Count ; i++ )
    {
        SETTING_ITEM1 &item = pParam->yb_Item[i];

        Yb_Data *yb = new Yb_Data;
    
        yb->name_cn = QString(item.desc);
        yb->name_ref = QString(item.ref);
        if (QString::fromLocal8Bit(item.val_Str) == "0") {
            yb->state = false;
        } else {
            yb->state = true;
        }

        yb_Server.insert(yb->name_ref, yb);
//        yb_Server.append(yb);
    }

//    qDebug() << "done";

//    foreach(Yb_Data *y, yb_Server) {
//        qDebug() << y->name_cn << y->state << y->name_ref;
//    }


    this->initPAM300Engine();

    return;
}

#include <QMessageBox>
void QuickTestInterface::initPAM300Engine()
{


    QHostAddress hostaddress("192.168.1.3");

#if defined (Q_OS_WIN32)
    QString path = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(WIN_OUTPUT_FILE);
#elif defined (Q_OS_LINUX)
    QString path = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(LINUX_OUTPUT_FILE);
#endif
//    qDebug() << path;
//    QFile file(path);
//    if (!file.open(QFile::ReadOnly|QFile::Text)) {
//        QMessageBox::about(0, "error", "path invalid");
//    } else {
//        file.close();
//    }
//    qDebug() << path.toLocal8Bit().data();
//    char *s1="F:/workspace/PC/QuickTest/build-QuickTest-Desktop_Qt_4_8_5_MinGW_32bit_MinGW_4_4_32bit-Debug/debug/config/quicktest/onekeyconfig/output.xml";
    engine.SetUdpConnect(2300,hostaddress);
//    if (!engine.InitGPP(path.toLocal8Bit().data())) {
    if (!engine.InitGPP(path.toAscii().data())) {
        QMessageBox::about(0, "error", "engine.InitGPP error");
        return;
    }
    engine.SetProtocal(0); //9-2 : is 0 , F3: is 1
    //设置二次值输入，PT: 220 , CT:1000/5
    engine.SetVIInputWay(0,220,200);
    //设置一次值输入，PT, CT： 无意义填写任意值：220，120
//    TEST1.SetVIInputWay(1,220,120);

    emit hint(QObject::trUtf8("准备就绪!\n"));

    curSection = 0;

    this->loopForTest();

    return;
}

void QuickTestInterface::loopForTest()
{

    if (curSection < testData.count()) {

        curProtection = testData.value(testData.keys().at(curSection));

        if (!curProtection) {
            emit error("测试内部错误");
        }

        emit hint(QString("\n******* %1/%2 *******\n").arg(curProtection->typeName).arg(curProtection->testName));

//        ybFlag = true;
//        if (bAutoYb) {
            this->pushYbData();
//        } else {
//            emit hint("连接服务器失败，请手动设置软压板后继续...");
//            this->sendMsg();
//        }


        curSection++;

        return;
    }

    emit hint("\n测试完成!\n");

}




void QuickTestInterface::commitYb(const QList<Yb_Data *> &ybList)
{
    QList<Yb_Data*> modify;

    return ;
}

void QuickTestInterface::pushYbData()
{
    //修改软压板
    m_modify.clear();
    foreach(Yb_Data *yb, curProtection->yb) {

        if (yb_Server.contains(yb->name_ref)) {

//            qDebug() << "contains" << yb->name_cn;

            Yb_Data *temp = yb_Server.value(yb->name_ref);
            if (temp->state != yb->state) {
//                qDebug() << "modify" << yb->name_cn;
                m_modify.append(yb);
            }

        }

    }

//    if (modify.isEmpty()) {
//        emit hint("无修改的软压板项");
//    }

//    bNeedRestoreYb = true;

    PEQU_PARAM pEqu = server->workThread()->getEquipWithAppend(server->serverName().toAscii().data());

    if (pEqu == NULL) {
        //结束该测试，准备下一项测试
        return;
    }

    if (pEqu->edit_yb_item)
    {
        delete[] pEqu->edit_yb_item;
        pEqu->edit_yb_count = 0;
    }

    pEqu->edit_yb_item = new SETTING_ITEM1[m_modify.count()];
    pEqu->edit_yb_count = m_modify.count();

    for (int i = 0 ;i < m_modify.count() ; i++)
    {
        Yb_Data *pItem = m_modify.at(i);

        strcpy(pEqu->edit_yb_item[i].ref,pItem->name_ref.toLocal8Bit().data());
//        qDebug() << "loacl" <<pItem->name_ref.toLocal8Bit().data();
        strcpy(pEqu->edit_yb_item[i].val_Str,pItem->state?"1":"0");


    }

    CMD_GEN cmd;

    cmd.cmdCode = CMD_MODIFY_YB;
    sprintf(cmd.serverName,"%s", server->serverName().toAscii().data());
    server->workThread()->appendCmd(cmd);

    ybFlag = MODIFY;

}

void QuickTestInterface::pushYbDataSuccess()
{

    if (RESTORE == ybFlag) {

        emit hint("软压板恢复成功\n");

        loopForTest();

    } else {

        emit hint("软压板修改成功\n");
        this->sendMsg();
    }

    return;

}

void QuickTestInterface::sendMsg()
{
    /* Control 结构体说明
////跳转从 1 开始，兼容以前的代码，其它都是从 0 开始
typedef struct StatusControl_tag {
    char   flag;         //  设置条件，Bit0: 时间条件,Bit1：开关量条件,Bit2：按键条件 :Bit 3: 硬接点,bit 4:1:按设定相位输出 0:相位连续
    unsigned int  TimeOutCon;   //时间条件 :ms
    char   TimeJump;     //  从 1 开始
    char   TriggerCon;   //  动作条件： bit0 - 3 对应开关量 1-4
    char   TriggerJump;  //  从 1 开始
    char   BinInp;       //  硬接点，1
    char   BinInpJump;  //   满足硬接点条件，跳到第几状态，

} struStatusControl ;
 */
    //V: 顺序 0-7:VA1,VA2,VB1,VB2,VC1,VC2,VN1,VN2
    //I: 顺序 0-7:IA1,IA2,IB1,IB2,IC1,IC2,IN1,IN2
    //状态一、状态三固定时间跳转

//    bool ok;
    engine.StopExec();
    engine.InitSignal();//reset signal , 避免以前测试的影响

    //设置状态1（故障状态）
    engine.TestStep[0].FaultType = curProtection->states.at(0)->faultType;
    for (int i = 0, k = 0; i < 3; ++i) {//i:相位
        engine.TestStep[0].VSignal[k].Value = curProtection->states.at(0)->Vsignal[i].Value;
        engine.TestStep[0].VSignal[k].Phase = curProtection->states.at(0)->Vsignal[i].Phase;
        engine.TestStep[0].ISignal[k].Value = curProtection->states.at(0)->Isignal[i].Value;
        engine.TestStep[0].ISignal[k].Phase = curProtection->states.at(0)->Isignal[i].Phase;
        k += 2;
    }
    //输出接点
    for(int i=0;i<4;i++) {
        engine.TestStep[0].OutBin[i] = (curProtection->states.at(0)->binOut[i])?1:0;
//        qDebug() << "state 1" <<engine.TestStep[0].OutBin[i];
    }
    //控制条件
    engine.TestStep[0].Control.flag=0x11; //时间控制，按设定相位输出
    engine.TestStep[0].Control.TimeJump = curProtection->states.at(0)->timeJumpTo;  //int/unsigned int变量赋值给 unsigned char！！！隐患，9位和高于9位的将被程序自动丢弃
    engine.TestStep[0].Control.TimeOutCon = curProtection->states.at(0)->timeTrigger;
//    qDebug() << "状态一" << "时间跳转节点" << engine.TestStep[0].Control.TimeJump << "跳转时间" << engine.TestStep[0].Control.TimeOutCon;
    //time out 跳转第 2 步

//====================================================================================

    //设置状态2
    //故障状态
    engine.TestStep[1].FaultType = curProtection->states.at(1)->faultType;
    for (int i = 0, k = 0; i < 3; ++i) {//i:相位
        engine.TestStep[1].VSignal[k].Value = curProtection->states.at(1)->Vsignal[i].Value;
        engine.TestStep[1].VSignal[k].Phase = curProtection->states.at(1)->Vsignal[i].Phase;
        engine.TestStep[1].ISignal[k].Value = curProtection->states.at(1)->Isignal[i].Value;
        engine.TestStep[1].ISignal[k].Phase = curProtection->states.at(1)->Isignal[i].Phase;
        k += 2;
    }
    //接点
    for(int i=0;i<4;i++) {
        engine.TestStep[1].OutBin[i] = (curProtection->states.at(1)->binOut[i])?1:0;
//        qDebug() << engine.TestStep[1].OutBin[i];
    }
    //控制条件
    engine.TestStep[1].Control.flag=0x3; //时间和动作条件，按设定相位输出
    if ((engine.TestStep[1].Control.flag&0x10) == 0)//bit4 = 0 相位连续
    {
        //如果相位连续，为了保证在该状态输出指定相位，根据本状态相位，对前一状态的相位进行修正
        engine.ReSetPhase(0,1);
    }
    engine.TestStep[1].Control.TimeJump = curProtection->states.at(1)->timeJumpTo;
    engine.TestStep[1].Control.TimeOutCon= curProtection->states.at(1)->timeTrigger;
//    qDebug() <<"time:" <<engine.TestStep[1].Control.TimeOutCon;
    engine.TestStep[1].Control.TriggerJump = curProtection->states.at(1)->nodeJumpTo;
    unsigned char triggerCon = 0x00;
    if (curProtection->states.at(1)->nodeState[0] == true) {
        triggerCon |= 0x01;
    }
    if (curProtection->states.at(1)->nodeState[1] == true) {
        triggerCon |= 0x02;
    }
    if (curProtection->states.at(1)->nodeState[2] == true) {
        triggerCon |= 0x04;
    }
    if (curProtection->states.at(1)->nodeState[3] == true) {
        triggerCon |= 0x08;
    }
    engine.TestStep[1].Control.TriggerCon = triggerCon; //检查开关量 1， 2 ，满足条件跳转
//    qDebug() << "triggerCon 2" << triggerCon;
//    qDebug() << "状态一" << "时间跳转节点" << engine.TestStep[0].Control.TimeJump << "跳转时间" << engine.TestStep[0].Control.TimeOutCon;

//====================================================================================

    //设置状态3
    //故障状态
    engine.TestStep[2].FaultType = curProtection->states.at(2)->faultType;
    for (int i = 0, k = 0; i < 3; ++i) {//i:相位
        engine.TestStep[2].VSignal[k].Value = curProtection->states.at(2)->Vsignal[i].Value;
        engine.TestStep[2].VSignal[k].Phase = curProtection->states.at(2)->Vsignal[i].Phase;
        engine.TestStep[2].ISignal[k].Value = curProtection->states.at(2)->Isignal[i].Value;
        engine.TestStep[2].ISignal[k].Phase = curProtection->states.at(2)->Isignal[i].Phase;
        k += 2;
    }
    //输出接点
    for(int i=0;i<4;i++) {
        engine.TestStep[2].OutBin[i] = (curProtection->states.at(2)->binOut[i])?1:0;
//        qDebug() << "state 3" <<engine.TestStep[2].OutBin[i];
    }
    //控制条件
    engine.TestStep[2].Control.flag=0x11; //时间控制，按设定相位输出
    engine.TestStep[2].Control.TimeJump = curProtection->states.at(2)->timeJumpTo;;
    engine.TestStep[2].Control.TimeOutCon = curProtection->states.at(2)->timeTrigger;

//====================================================================================

    //设置状态4
     //设置故障状态
    engine.TestStep[3].FaultType = curProtection->states.at(3)->faultType;
    for (int i = 0, k = 0; i < 3; ++i) {//i:相位
        engine.TestStep[3].VSignal[k].Value = curProtection->states.at(3)->Vsignal[i].Value;
        engine.TestStep[3].VSignal[k].Phase = curProtection->states.at(3)->Vsignal[i].Phase;
        engine.TestStep[3].ISignal[k].Value = curProtection->states.at(3)->Isignal[i].Value;
        engine.TestStep[3].ISignal[k].Phase = curProtection->states.at(3)->Isignal[i].Phase;
        k += 2;
    }
    //输出接点
    for(int i=0;i<4;i++) {
        engine.TestStep[3].OutBin[i] = (curProtection->states.at(3)->binOut[i])?1:0;
//        qDebug() << "state 4" <<engine.TestStep[3].OutBin[i];
    }
    //设置控制条件
    engine.TestStep[3].Control.flag = 0x03; //时间和动作条件
    if ((engine.TestStep[1].Control.flag&0x10)==0)
    {
      //如果相位连续，为了保证在该状态输出指定相位，根据本状态相位，对前一状态的相位进行修正
      engine.ReSetPhase(2,3);
    }
    /*
     * BUG:输出到状态四后程序找不到指定跳转的状态
     * 修改输出测试方案，状态四结束后默认跳转状态5
     * fixed 2014-12-10
     */
    engine.TestStep[3].Control.TimeJump = curProtection->states.at(3)->timeJumpTo;
    engine.TestStep[3].Control.TimeOutCon = curProtection->states.at(3)->timeTrigger;
    engine.TestStep[3].Control.TriggerJump = 5 ;
    triggerCon = 0x00;
    if (curProtection->states.at(3)->nodeState[0] == true) {
        triggerCon |= 0x01;
    }
    if (curProtection->states.at(3)->nodeState[1] == true) {
        triggerCon |= 0x02;
    }
    if (curProtection->states.at(3)->nodeState[2] == true) {
        triggerCon |= 0x04;
    }
    if (curProtection->states.at(3)->nodeState[3] == true) {
        triggerCon |= 0x08;
    }
    engine.TestStep[3].Control.TriggerCon = triggerCon; //检查开关量 1， 2 ，满足条件跳转
//    qDebug() << "triggerCon 4" << triggerCon;

    //设置状态5, 关断信号
    engine.OffSignal(4);
    engine.TestStep[4].Control.flag = 1; //
    engine.TestStep[4].Control.TimeJump = 5; //停止在该步
    engine.TestStep[4].Control.TimeOutCon = 0x7fff;
    //设置双 AD,根据第一套设置第2 套
    engine.SetDoubleAD();
    //开始输出

    emit hint("开始输出信号...\n");
    engine.ExecSeq();

//    QMessageBox::about(this,tr("Hint"), tr("Output!"));



    //检查输出情况
    unsigned char result[512];
    int  nowStatus=0,i1,nextStatus;
    bool b1;
    /* 返回数据
 参见文档 0x928c
 */
    //读取测试结果, fgh ,work , 获得该状态输出时间
    nextStatus=0;
    int flag;
    float time1;
    QString s1,s2,s3,s4;

    while (1)
    {
        qApp->processEvents();
        b1=engine.CheckSeqExec(nextStatus,result);

        if (b1)
        {

            //解析反馈
            i1= result[5];
            if (i1>0)
            {
                s1=QString::number(nextStatus+1);
                if (i1&1) //满足时间条件
                {
                    nextStatus=engine.TestStep[nextStatus].Control.TimeJump-1;
                    s2=QString("时间动作");
                }
                else if (i1&2) //满足动作条件
                {
                    flag=result[10];  //动作情况
                    nextStatus=engine.TestStep[nextStatus].Control.TriggerJump-1;
                    s2=QString("接点动作");
                }
                else if (i1&4) //满足硬接点条件
                {
                    flag=result[10];  //动作情况
                    nextStatus=engine.TestStep[nextStatus].Control.BinInpJump-1;
                    s2=QString("硬接点动作");
                }
                // 获得状态的输出时间:
                time1=(result[6]+result[7]*256+result[8]*65536+(result[9]<<24));  //us
                time1=time1/1000000; //sec
                s3=QString::number(time1);
                s4=QString(" %1,%2,%3\n").arg(s1).arg(s2).arg(s3);
                emit hint(s4);

            }
        }
        else
        {
            qDebug()<<"error";
            break; //结束;
        }
        if (nextStatus>=(MAX_STATUS_NUM-1))
            break; //结束
        //当前状态
        nowStatus= result[4];
        //界面切换到相应状态
    }
    engine.StopExec();

    ybFlag = RESTORE;
//    qDebug() << "end";
    loopForTest();
}

