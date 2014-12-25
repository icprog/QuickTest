#include "valuecalc_1107.h"
#include "ui_valuecalc_1107.h"

#include "../quicktestcore.h"
extern QuickTestCore core;

ValueCalc_1107::ValueCalc_1107(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueCalc_1107)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

ValueCalc_1107::~ValueCalc_1107()
{
    delete ui;
}

void ValueCalc_1107::initWindow()
{
    this->setWindowTitle(QObject::tr("线路保护-主保护-纵联零序"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

void ValueCalc_1107::initSignals()
{
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_calc(QAbstractButton*)));
}


void ValueCalc_1107::slot_calc(QAbstractButton *btn)
{
    if (btn == ui->buttonBox->button((QDialogButtonBox::Cancel))) {
        return;
    }
    float dianya = ui->lineEdit_dianya->text().toFloat();
    float dianliu = ui->lineEdit_dianliu->text().toFloat();

    /*--------- state 1 --------------*/
    core.templateModel()->currentAction()->smvData[0].Ia[0] = dianliu;
    core.templateModel()->currentAction()->smvData[0].Ib[0] = dianliu;
    core.templateModel()->currentAction()->smvData[0].Ic[0] = dianliu;
    core.templateModel()->currentAction()->smvData[0].In[0] = 0.000;

    core.templateModel()->currentAction()->smvData[0].Va[0] = dianya;
    core.templateModel()->currentAction()->smvData[0].Vb[0] = dianya;
    core.templateModel()->currentAction()->smvData[0].Vc[0] = dianya;
    core.templateModel()->currentAction()->smvData[0].Vn[0] = 0.000;

    /*--------- state 2 --------------*/
    core.templateModel()->currentAction()->smvData[1].Ia[0] = dianliu*0.95;
    core.templateModel()->currentAction()->smvData[1].Ib[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ic[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].In[0] = 0.000;

    core.templateModel()->currentAction()->smvData[1].Va[0] = 30.000;
    core.templateModel()->currentAction()->smvData[1].Vb[0] = 30.000;
    core.templateModel()->currentAction()->smvData[1].Vc[0] = 30.000;
    core.templateModel()->currentAction()->smvData[1].Vn[0] = 0.000;

    /*--------- state 3 --------------*/
    core.templateModel()->currentAction()->smvData[2].Ia[0] = dianliu;
    core.templateModel()->currentAction()->smvData[2].Ib[0] = dianliu;
    core.templateModel()->currentAction()->smvData[2].Ic[0] = dianliu;
    core.templateModel()->currentAction()->smvData[2].In[0] = 0.000;

    core.templateModel()->currentAction()->smvData[2].Va[0] = dianya;
    core.templateModel()->currentAction()->smvData[2].Vb[0] = dianya;
    core.templateModel()->currentAction()->smvData[2].Vc[0] = dianya;
    core.templateModel()->currentAction()->smvData[2].Vn[0] = 0.000;

    /*--------- state 4 --------------*/
    core.templateModel()->currentAction()->smvData[3].Ia[0] = dianliu*1.050;
    core.templateModel()->currentAction()->smvData[3].Ib[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ic[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].In[0] = 0.000;

    core.templateModel()->currentAction()->smvData[3].Va[0] = 30.000;
    core.templateModel()->currentAction()->smvData[3].Vb[0] = 30.000;
    core.templateModel()->currentAction()->smvData[3].Vc[0] = 30.000;
    core.templateModel()->currentAction()->smvData[3].Vn[0] = 0.000;
}
