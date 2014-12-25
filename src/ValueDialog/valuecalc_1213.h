#ifndef VALUECALC_1213_H
#define VALUECALC_1213_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1213;
}

class ValueCalc_1213 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1213(QWidget *parent = 0);
    ~ValueCalc_1213();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1213 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1213_H
