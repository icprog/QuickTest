#ifndef VALUECALC_1201_H
#define VALUECALC_1201_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1201;
}

class ValueCalc_1201 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1201(QWidget *parent = 0);
    ~ValueCalc_1201();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1201 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1201_H
