#ifndef VALUECALC_1102_H
#define VALUECALC_1102_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class ValueCalc_1102;
}

class ValueCalc_1102 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1102(QWidget *parent = 0);
    ~ValueCalc_1102();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1102 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1102_H
