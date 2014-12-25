#ifndef VALUECALC_1203_H
#define VALUECALC_1203_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1203;
}

class ValueCalc_1203 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1203(QWidget *parent = 0);
    ~ValueCalc_1203();
private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1203 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1203_H
