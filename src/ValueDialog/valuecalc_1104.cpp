#include "valuecalc_1104.h"
#include "ui_valuecalc_1104.h"

ValueCalc_1104::ValueCalc_1104(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueCalc_1104)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("线路保护-主保护-突变量差动"));
}

ValueCalc_1104::~ValueCalc_1104()
{
    delete ui;
}
