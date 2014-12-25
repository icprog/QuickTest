#include "valuecalc_1102.h"
#include "ui_valuecalc_1102.h"

#include "quicktestcore.h"
extern QuickTestCore core;

#include <QPushButton>

ValueCalc_1102::ValueCalc_1102(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueCalc_1102)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

ValueCalc_1102::~ValueCalc_1102()
{
    delete ui;
}

void ValueCalc_1102::initWindow()
{
    this->setWindowTitle(QObject::tr("线路保护-主保护-零序差动"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

void ValueCalc_1102::initSignals()
{
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_calc(QAbstractButton*)));
}

void ValueCalc_1102::slot_calc(QAbstractButton *btn)
{
    if (btn == ui->buttonBox->button((QDialogButtonBox::Cancel))) {
        return;
    }
    float key = ui->lineEdit->text().toFloat();

    //设置故障类型
    core.templateModel()->currentAction()->smvData[1].faultType = QuickTestCore::faultType(ui->comboBox->currentText());
    core.templateModel()->currentAction()->smvData[3].faultType = QuickTestCore::faultType(ui->comboBox->currentText());

    /*---------- state 2 -----------*/
    // -> A相
    core.templateModel()->currentAction()->smvData[1].Va[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ia[0] = key*0.950;

    // -> B相
    core.templateModel()->currentAction()->smvData[1].Vb[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ib[0] = 0.000;

    // -> C相
    core.templateModel()->currentAction()->smvData[1].Vb[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ic[0] = 0.000;

    /*---------- state 4 -----------*/
    // -> A相
    core.templateModel()->currentAction()->smvData[3].Va[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ia[0] = key*1.050;
    // -> B相
    core.templateModel()->currentAction()->smvData[3].Vb[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ib[0] = 0.000;
    // -> C相
    core.templateModel()->currentAction()->smvData[3].Vc[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ic[0] = 0.000;
}
