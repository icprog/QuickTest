#include "valuecalc_1211.h"
#include "ui_valuecalc_1211.h"

#include "quicktestcore.h"
extern QuickTestCore core;

ValueCalc_1211::ValueCalc_1211(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueCalc_1211)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

ValueCalc_1211::~ValueCalc_1211()
{
    delete ui;
}

void ValueCalc_1211::initWindow()
{
    this->setWindowTitle(QObject::tr("线路保护-后备保护-零序电流3段"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

}

void ValueCalc_1211::initSignals()
{
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_calc(QAbstractButton*)));
}

void ValueCalc_1211::slot_calc(QAbstractButton *btn)
{
    if (btn == ui->buttonBox->button((QDialogButtonBox::Cancel))) {
        return;
    }
    float dianliu = ui->lineEdit_dianliu->text().toFloat();
    float dianya = ui->lineEdit_dianya->text().toFloat();


    PAM300Share mydll;
    StruInputPara input;
    StruOutPara output;

    input.Type = 1211;

//    input.FaultType = ui->comboBox->currentIndex();
    input.Para[0] = dianliu;
    input.Para[1] = dianya;
//    input.Para[2] = zukangjiao;
//    input.Para[3] = 0;

    /*--------- state 2 --------------*/
    input.Para[0] = dianliu*0.950;
    input.Para[1] = dianya*0.950;
    output = mydll.CalcVIValue(input);

    core.templateModel()->currentAction()->smvData[1].Ia[0] = output.I[0];
    core.templateModel()->currentAction()->smvData[1].Ib[0] = output.I[1];
    core.templateModel()->currentAction()->smvData[1].Ic[0] = output.I[2];
    core.templateModel()->currentAction()->smvData[1].In[0] = output.I[3];
    core.templateModel()->currentAction()->smvData[1].Ia[1] = output.IPhase[0];
    core.templateModel()->currentAction()->smvData[1].Ib[1] = output.IPhase[1];
    core.templateModel()->currentAction()->smvData[1].Ic[1] = output.IPhase[2];
    core.templateModel()->currentAction()->smvData[1].In[1] = output.IPhase[3];

    core.templateModel()->currentAction()->smvData[1].Va[0] = output.V[0];
    core.templateModel()->currentAction()->smvData[1].Vb[0] = output.V[1];
    core.templateModel()->currentAction()->smvData[1].Vc[0] = output.V[2];
    core.templateModel()->currentAction()->smvData[1].Vn[0] = output.V[3];
    core.templateModel()->currentAction()->smvData[1].Va[1] = output.VPhase[0];
    core.templateModel()->currentAction()->smvData[1].Vb[1] = output.VPhase[1];
    core.templateModel()->currentAction()->smvData[1].Vc[1] = output.VPhase[2];
    core.templateModel()->currentAction()->smvData[1].Vn[1] = output.VPhase[3];

    /*--------- state 4 --------------*/
    input.Para[0] = dianliu*1.050;
    input.Para[1] = dianya*1.050;
    output = mydll.CalcVIValue(input);
    core.templateModel()->currentAction()->smvData[3].Ia[0] = output.I[0];
    core.templateModel()->currentAction()->smvData[3].Ib[0] = output.I[1];
    core.templateModel()->currentAction()->smvData[3].Ic[0] = output.I[2];
    core.templateModel()->currentAction()->smvData[3].In[0] = output.I[3];
    core.templateModel()->currentAction()->smvData[3].Ia[1] = output.IPhase[0];
    core.templateModel()->currentAction()->smvData[3].Ib[1] = output.IPhase[1];
    core.templateModel()->currentAction()->smvData[3].Ic[1] = output.IPhase[2];
    core.templateModel()->currentAction()->smvData[3].In[1] = output.IPhase[3];

    core.templateModel()->currentAction()->smvData[3].Va[0] = output.V[0];
    core.templateModel()->currentAction()->smvData[3].Vb[0] = output.V[1];
    core.templateModel()->currentAction()->smvData[3].Vc[0] = output.V[2];
    core.templateModel()->currentAction()->smvData[3].Vn[0] = output.V[3];
    core.templateModel()->currentAction()->smvData[3].Va[1] = output.VPhase[0];
    core.templateModel()->currentAction()->smvData[3].Vb[1] = output.VPhase[1];
    core.templateModel()->currentAction()->smvData[3].Vc[1] = output.VPhase[2];
    core.templateModel()->currentAction()->smvData[3].Vn[1] = output.VPhase[3];
}
