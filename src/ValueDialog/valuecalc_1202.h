#ifndef VALUECALC_1202_H
#define VALUECALC_1202_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1202;
}

class ValueCalc_1202 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1202(QWidget *parent = 0);
    ~ValueCalc_1202();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1202 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1202_H
