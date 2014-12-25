#include "dactioncondition.h"
#include "ui_dactioncondition.h"
#include <QDebug>
#include "quicktestcore.h"
extern QuickTestCore core;

dActionCondition::dActionCondition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dActionCondition),
    m_curIndex(-1)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

dActionCondition::~dActionCondition()
{
    delete ui;
}

void dActionCondition::initWindow ()
{
    this->setWindowTitle(QObject::tr("控制条件"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QObject::tr("确定"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QObject::tr("取消"));

    ui->lineEdit->setDisabled (true);
    ui->checkBox_p1->setDisabled (true);
    ui->checkBox_p2->setDisabled (true);
    ui->checkBox_p3->setDisabled (true);
    ui->checkBox_p4->setDisabled (true);
    ui->lineEdit->setPlaceholderText (QString("0"));
}

void dActionCondition::initSignals ()
{
    connect (ui->checkBox_time, SIGNAL(stateChanged(int)), this, SLOT(slot_timeStateChanged(int)));
    connect (ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slot_timeValueChanged(QString)));
    connect (ui->checkBox_action, SIGNAL(stateChanged(int)), this, SLOT(slot_actionStateChanged(int)));
    connect (ui->checkBox_p1, SIGNAL(stateChanged(int)), this, SLOT(slot_statePointChanged(int)));
    connect (ui->checkBox_p2, SIGNAL(stateChanged(int)), this, SLOT(slot_statePointChanged(int)));
    connect (ui->checkBox_p3, SIGNAL(stateChanged(int)), this, SLOT(slot_statePointChanged(int)));
    connect (ui->checkBox_p4, SIGNAL(stateChanged(int)), this, SLOT(slot_statePointChanged(int)));

    connect (ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_accepted(QAbstractButton*)));
}

bool dActionCondition::updateThis (int index)
{
    m_curIndex = index;
    switch (index) {
    case 0:
        this->setWindowTitle(QObject::tr("状态一"));
        ui->checkBox_action->setDisabled(true);
        ui->checkBox_p1->setDisabled (true);
        ui->checkBox_p2->setDisabled (true);
        ui->checkBox_p3->setDisabled (true);
        ui->checkBox_p4->setDisabled (true);
        break;
    case 1:
        this->setWindowTitle(QObject::tr("状态二"));
        break;
    case 2:
        this->setWindowTitle(QObject::tr("状态三"));
        ui->checkBox_action->setDisabled(true);
        ui->checkBox_p1->setDisabled (true);
        ui->checkBox_p2->setDisabled (true);
        ui->checkBox_p3->setDisabled (true);
        ui->checkBox_p4->setDisabled (true);
        break;
    case 3:
        this->setWindowTitle(QObject::tr("状态四"));
        break;
    default:
        return false;
    }

//    disconnect (ui->checkBox_time, SIGNAL(stateChanged(int)), this, SLOT(slot_timeStateChanged(int)));
//    disconnect (ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slot_timeValueChanged(QString)));
//    disconnect (ui->checkBox_action, SIGNAL(stateChanged(int)), this, SLOT(slot_actionStateChanged(int)));

    CTestAction *testAction =  core.templateModel()->currentAction();

    if (testAction->states()->timeState(m_curIndex)) {
        ui->checkBox_time->setCheckState(Qt::Checked);
        ui->lineEdit->setText(QString::number(testAction->states()->timeValue(m_curIndex)));
        ui->checkBox_time->setEnabled(true);
        ui->lineEdit->setEnabled(true);
    } else {
//        ui->checkBox_time->setCheckState(Qt::Unchecked);
//        ui->checkBox_time->setEnabled(false);
//        ui->lineEdit->setEnabled(false);
    }

    if (testAction->states()->actionState(m_curIndex)) {
        ui->checkBox_action->setCheckState(Qt::Checked);
        if (testAction->states()->actionPointState(m_curIndex, 0)) {
            ui->checkBox_p1->setChecked(Qt::Checked);
        } else {
            ui->checkBox_p1->setChecked(Qt::Unchecked);
        }
        if (testAction->states()->actionPointState(m_curIndex, 1)) {
            ui->checkBox_p2->setChecked(Qt::Checked);
        } else {
            ui->checkBox_p2->setChecked(Qt::Unchecked);
        }
        if (testAction->states()->actionPointState(m_curIndex, 2)) {
            ui->checkBox_p3->setChecked(Qt::Checked);
        } else {
            ui->checkBox_p3->setChecked(Qt::Unchecked);
        }
        if (testAction->states()->actionPointState(m_curIndex, 3)) {
            ui->checkBox_p4->setChecked(Qt::Checked);
        } else {
            ui->checkBox_p4->setChecked(Qt::Unchecked);
        }
    } else {
        // do nothing.
    }

//    connect (ui->checkBox_time, SIGNAL(stateChanged(int)), this, SLOT(slot_timeStateChanged(int)));
//    connect (ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slot_timeValueChanged(QString)));
//    connect (ui->checkBox_action, SIGNAL(stateChanged(int)), this, SLOT(slot_actionStateChanged(int)));
    return true;
}

void dActionCondition::slot_timeStateChanged (int state)
{
    if (state == Qt::Checked) {
        ui->lineEdit->setEnabled (true);
    } else {
        ui->lineEdit->setDisabled (true);
    }
}

void dActionCondition::slot_timeValueChanged(QString)
{
}

void dActionCondition::slot_actionStateChanged (int state)
{
    if (state == Qt::Checked) {
        ui->checkBox_p1->setEnabled (true);
        ui->checkBox_p2->setEnabled (true);
        ui->checkBox_p3->setEnabled (true);
        ui->checkBox_p4->setEnabled (true);
    } else {
        ui->checkBox_p1->setDisabled (true);
        ui->checkBox_p2->setDisabled (true);
        ui->checkBox_p3->setDisabled (true);
        ui->checkBox_p4->setDisabled (true);
    }
}

void dActionCondition::slot_statePointChanged(int)
{

}

void dActionCondition::slot_accepted(QAbstractButton *btn)
{
    if (btn == ui->buttonBox->button(QDialogButtonBox::Ok)) {

        if (ui->checkBox_time->isChecked()) {
            core.templateModel()->currentAction()->states()->setTimeEnable(m_curIndex, ui->lineEdit->text().toInt());
        } else {
            core.templateModel()->currentAction()->states()->setTimeDisable(m_curIndex);
        }

        if (ui->checkBox_action->isChecked()) {
            core.templateModel()->currentAction()->states()->setActionEnable(m_curIndex, ui->checkBox_p1->checkState(),
                                                                            ui->checkBox_p2->checkState(),
                                                                            ui->checkBox_p3->checkState(),
                                                                            ui->checkBox_p4->checkState());
        } else {
            core.templateModel()->currentAction()->states()->setActionDisable(m_curIndex);
        }

    } else {
        //do nothing.
    }
}
