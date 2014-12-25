#include "wactionconfig.h"
#include "ui_wactionconfig.h"
#include <QMessageBox>

#include "./quicktestcore.h"
#include <QDebug>
extern QuickTestCore core;

wActionConfig::wActionConfig(int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wActionConfig),
    m_actionId(id),
    nCurrentState(0)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

wActionConfig::~wActionConfig()
{
    delete m_centerLayout;
    m_centerLayout = 0;
    delete ui;
}

void wActionConfig::initWindow()
{
//    if () {
//        ui->btn_board->setDisabled(true);
//    }

    btn_help  = new QPushButton(QObject::trUtf8("帮助"), this);
    connect(btn_help, SIGNAL(clicked()), SLOT(btn_help_clicked()));
    ui->navi->addButton(btn_help);

    btn_calc  = new QPushButton(QObject::trUtf8("定值\n输入"), this);
    connect(btn_calc, SIGNAL(clicked()), SLOT(btn_calc_clicked()));
    ui->navi->addButton(btn_calc);

    btn_control  = new QPushButton(QObject::trUtf8("控制\n条件"), this);
    connect(btn_control, SIGNAL(clicked()), SLOT(btn_control_clicked()));
    ui->navi->addButton(btn_control);

    btn_yb  = new QPushButton(QObject::trUtf8("配置\n软压板"), this);
    connect(btn_yb, SIGNAL(clicked()), SLOT(btn_yb_clicked()));
    ui->navi->addButton(btn_yb, true);

//    btn_return  = new QPushButton(QObject::trUtf8("返回"), this);
//    connect(btn_return, SIGNAL(clicked()), SLOT(btn_return_clicked()));
//    ui->navi->addButton(btn_return, true);


    m_centerLayout = new QGridLayout();
    m_centerLayout->setContentsMargins(3,3,3,3);

    QHBoxLayout *hbox = new QHBoxLayout();
    QLabel *label = new QLabel("开关量输出：");
    label->setWordWrap(false);
    label->setAlignment(Qt::AlignCenter);
    hbox->addWidget(label);

    for(int i = 0; i < ACTION_MAX_CIRCUIT_COUNT; i++) {
        m_circuit[i].setText(QString::number(i+1));
        m_circuit[i].setMinimumSize(60, 50);
        hbox->addWidget(&m_circuit[i]);
    }

    m_tableView.setMaximumSize(640, 270);
    m_centerLayout->addWidget(&m_tableView);
    m_centerLayout->addLayout(hbox, 1, 0);

    ui->tab_state1->setLayout(m_centerLayout);

    for(int i=0; i < 4; i++) {
        this->m_stateModel[i].setHorizontalHeaderLabels (QStringList()<< QObject::tr ("幅值 (V/A)") << QObject::tr ("相位 (度)") << QObject::tr ("频率 (Hz)"));
        this->m_stateModel[i].setVerticalHeaderLabels (QStringList() << QObject::tr ("Va") << QObject::tr ("Vb") << QObject::tr ("Vc")\
                                                       << QObject::tr("Vn") << QObject::tr ("Ia") << QObject::tr ("Ib") << QObject::tr ("Ic") << QObject::tr ("In"));
    }

    this->m_tableView.setAlternatingRowColors (true);
    this->m_tableView.setModel (&m_stateModel[0]);
    loadCircuitGroupState(0);
    ui->tabWidget->setCurrentWidget (ui->tab_state1);
}

void wActionConfig::initSignals()
{
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slot_tabChanged(int)));

    connect(&m_circuit[0], SIGNAL(stateChanged(int)), this, SLOT(slot_circuit_1(int)));
    connect(&m_circuit[1], SIGNAL(stateChanged(int)), this, SLOT(slot_circuit_2(int)));
    connect(&m_circuit[2], SIGNAL(stateChanged(int)), this, SLOT(slot_circuit_3(int)));
    connect(&m_circuit[3], SIGNAL(stateChanged(int)), this, SLOT(slot_circuit_4(int)));

//    for(int i = 0; i < 4; i++) {
//        connect(&m_circuit[i], SIGNAL(stateChanged(int)), this, SLOT(slot_updateCircuitGroupState(int)));
//    }
}
#include <QKeyEvent>
void wActionConfig::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        event->accept();
        emit signal_accepted();
        break;
    default:
        QWidget::keyPressEvent(event);
    }

    return ;
}

void wActionConfig::btn_help_clicked()
{

}

void wActionConfig::btn_calc_clicked()
{
    switch (m_actionId) {
    case 1101: {
        ValueCalc_1101 vc1101(this);
        vc1101.exec();
    }
        break;
    case 1102: {
        ValueCalc_1102 vc1102(this);
        vc1102.exec();
    }
        break;
    case 1103: {
        ValueCalc_1103 vc1103(this);
        vc1103.exec();
    }
        break;
//    case 1104: {
//        ValueCalc_1104 vc1104(this);
//        vc1104.exec();
//    }
    case 1105: {
        ValueCalc_1105 vc1105(this);
        vc1105.exec();
    }
        break;
    case 1106: {
        ValueCalc_1106 vc1106(this);
        vc1106.exec();
    }
        break;
    case 1107: {
        ValueCalc_1107 vc1107(this);
        vc1107.exec();
    }
        break;
    case 1201: {
        ValueCalc_1201 vc1201(this);
        vc1201.exec();
    }
        break;
    case 1202: {
        ValueCalc_1202 vc1202(this);
        vc1202.exec();
    }
        break;
    case 1203: {
        ValueCalc_1203 vc1203(this);
        vc1203.exec();
    }
        break;
    case 1204: {
        ValueCalc_1204 vc1204(this);
        vc1204.exec();
    }
        break;
    case 1205: {
        ValueCalc_1202 vc1205(this);
        vc1205.exec();
    }
        break;
    case 1206: {
        ValueCalc_1206 vc1206(this);
        vc1206.exec();
    }
        break;
    case 1207: {
        ValueCalc_1207 vc1207(this);
        vc1207.exec();
    }
        break;
    case 1208: {
        ValueCalc_1208 vc1208(this);
        vc1208.exec();
    }
        break;
    case 1209: {
        ValueCalc_1209 vc1209(this);
        vc1209.exec();
    }
        break;
    case 1210: {
        ValueCalc_1210 vc1210(this);
        vc1210.exec();
    }
        break;
    case 1211: {
        ValueCalc_1211 vc1211(this);
        vc1211.exec();
    }
        break;
    case 1212: {
        ValueCalc_1212 vc1212(this);
        vc1212.exec();
    }
        break;
    case 1213: {
        ValueCalc_1213 vc1213(this);
        vc1213.exec();
    }
        break;
    default:
        QMessageBox::warning(this, QObject::tr("计算错误"), QObject::tr("无效的测试项目"), QMessageBox::Ok);
        return;
    }

    this->updateTableData();
}

#include "dactioncondition.h"
void wActionConfig::btn_control_clicked()
{
    dActionCondition condition;
    if (condition.updateThis (ui->tabWidget->currentIndex())) {
        condition.exec ();
    } else {
        QMessageBox::warning (this, QObject::tr ("警告"), QObject::tr ("控制条件更新失败"), QMessageBox::Ok);
    }
}

void wActionConfig::btn_yb_clicked()
{
//    bool isConnected = false;
//    if (!core.templateModel()->currentAction()->board()->isServerConnect()) {
//        isConnected = false;
//    } else {
//        isConnected = true;
//    }
    if (m_boardInfo.updateThis(core.substationServer()->isConnected())) {
        m_boardInfo.exec();
    } else {
        QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("软压板更新失败"), QMessageBox::Ok);
    }
}

/*
void wActionConfig::btn_return_clicked()
{
    emit signal_accepted();
}
*/

void wActionConfig::updateTableData()
{
    for(int k=0; k < 4; k++) {
        this->m_stateModel[k].removeRows (0, this->m_stateModel[k].rowCount ());
    }

    QList<QStandardItem*> vaList;
    QList<QStandardItem*> vbList;
    QList<QStandardItem*> vcList;
    QList<QStandardItem*> vnList;
    QList<QStandardItem*> iaList;
    QList<QStandardItem*> ibList;
    QList<QStandardItem*> icList;
    QList<QStandardItem*> inList;
    for(int i = 0; i < 4; i++) {
         vaList.clear ();
         vbList.clear ();
         vcList.clear ();
         vnList.clear ();
         iaList.clear ();
         ibList.clear ();
         icList.clear ();
         inList.clear ();
        for(int j = 0; j < 3; j++) {
            QStandardItem *vaItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Va[j]));
            vaList.append(vaItem);
            QStandardItem *vbItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Vb[j]));
            vbList.append (vbItem);
            QStandardItem *vcItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Vc[j]));
            vcList.append (vcItem);
            QStandardItem *vnItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Vn[j]));
            vnList.append (vnItem);
            QStandardItem *iaItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Ia[j]));
            iaList.append (iaItem);
            QStandardItem *ibItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Ib[j]));
            ibList.append (ibItem);
            QStandardItem *icItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].Ic[j]));
            icList.append (icItem);
            QStandardItem *inItem = new QStandardItem(QString::number (core.templateModel ()->currentAction ()->smvData[i].In[j]));
            inList.append (inItem);
        }
        this->m_stateModel[i].appendRow (vaList);
        this->m_stateModel[i].appendRow (vbList);
        this->m_stateModel[i].appendRow (vcList);
        this->m_stateModel[i].appendRow (vnList);
        this->m_stateModel[i].appendRow (iaList);
        this->m_stateModel[i].appendRow (ibList);
        this->m_stateModel[i].appendRow (icList);
        this->m_stateModel[i].appendRow (inList);

        this->m_stateModel[i].setVerticalHeaderLabels (QStringList() << QObject::tr ("Va") << QObject::tr ("Vb") << QObject::tr ("Vc")\
                                                       << QObject::tr("Vn") << QObject::tr ("Ia") << QObject::tr ("Ib") << QObject::tr ("Ic") << QObject::tr ("In"));
    }

}

void wActionConfig::loadCircuitGroupState(int index)
{

    CTestAction* action = core.templateModel()->currentAction();
    if (!action) {
        return;
    }
    for(int i = 0; i < 4; i++) {
        if (action->circuit_state(index)->circuit(i)->isEnable()) {
            m_circuit[i].setCheckState(Qt::Checked);
        } else {
            m_circuit[i].setCheckState(Qt::Unchecked);
        }
    }

}

void wActionConfig::slot_circuit_1(int checkSate)
{
    CTestAction* action = core.templateModel()->currentAction();
    if (!action) {
        return;
    }

    if (Qt::Checked == checkSate) {
        action->circuit_state(nCurrentState)->circuit(0)->setEnable(true);
    } else {
        action->circuit_state(nCurrentState)->circuit(0)->setEnable(false);
    }

}
void wActionConfig::slot_circuit_2(int checkSate)
{
    CTestAction* action = core.templateModel()->currentAction();
    if (!action) {
        return;
    }

    if (Qt::Checked == checkSate) {
        action->circuit_state(nCurrentState)->circuit(1)->setEnable(true);
    } else {
        action->circuit_state(nCurrentState)->circuit(1)->setEnable(false);
    }

}
void wActionConfig::slot_circuit_3(int checkSate)
{
    CTestAction* action = core.templateModel()->currentAction();
    if (!action) {
        return;
    }

    if (Qt::Checked == checkSate) {
        action->circuit_state(nCurrentState)->circuit(2)->setEnable(true);
    } else {
        action->circuit_state(nCurrentState)->circuit(2)->setEnable(false);
    }

}
void wActionConfig::slot_circuit_4(int checkSate)
{
    CTestAction* action = core.templateModel()->currentAction();
    if (!action) {
        return;
    }

    if (Qt::Checked == checkSate) {
        action->circuit_state(nCurrentState)->circuit(3)->setEnable(true);
    } else {
        action->circuit_state(nCurrentState)->circuit(3)->setEnable(false);
    }

}

void wActionConfig::slot_tabChanged(int index)
{
    nCurrentState = index;
    ui->tabWidget->currentWidget()->setLayout(m_centerLayout);
    this->m_tableView.setModel (&m_stateModel[index]);

    this->loadCircuitGroupState(index);

    ui->tabWidget->setCurrentIndex(index);
}
