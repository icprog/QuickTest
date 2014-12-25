#ifndef VALUECALC_1104_H
#define VALUECALC_1104_H

#include <QDialog>

namespace Ui {
class ValueCalc_1104;
}

class ValueCalc_1104 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1104(QWidget *parent = 0);
    ~ValueCalc_1104();

private:
    Ui::ValueCalc_1104 *ui;
};

#endif // VALUECALC_1104_H
