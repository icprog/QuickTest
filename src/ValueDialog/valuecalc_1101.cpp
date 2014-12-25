#include "valuecalc_1101.h"
#include "ui_valuecalc_1101.h"

#include "../quicktestcore.h"
extern QuickTestCore core;

ValueCalc_1101::ValueCalc_1101(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueCalc_1101)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

ValueCalc_1101::~ValueCalc_1101()
{
    delete ui;
}

void ValueCalc_1101::initWindow()
{
    this->setWindowTitle(QObject::tr("线路保护-主保护-纵联差动"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->lineEdit_value->setPlaceholderText("0.000");//默认参数值

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}
void ValueCalc_1101::initSignals()
{
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_calc(QAbstractButton*)));
}

void ValueCalc_1101::slot_calc(QAbstractButton* btn)
{
    if (btn == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        return;
    }

    float key = ui->lineEdit_value->text().toFloat();

    //state 2 -> A相
    core.templateModel()->currentAction()->smvData[1].Va[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ia[0] = key*0.950;

    //state 4 -> A相
    core.templateModel()->currentAction()->smvData[3].Va[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ia[0] = key*1.050;

    //state 2 -> B相
    core.templateModel()->currentAction()->smvData[1].Vb[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ib[0] = key*0.950;

    //state 4 -> B相
    core.templateModel()->currentAction()->smvData[3].Vb[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ib[0] = key*1.050;

    //state 2 -> C相
    core.templateModel()->currentAction()->smvData[1].Vc[0] = 0.000;
    core.templateModel()->currentAction()->smvData[1].Ic[0] = key*0.950;

    //state 4 -> C相
    core.templateModel()->currentAction()->smvData[3].Vc[0] = 0.000;
    core.templateModel()->currentAction()->smvData[3].Ic[0] = key*1.050;
}
