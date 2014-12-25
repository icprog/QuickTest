#ifndef VALUECALC_1101_H
#define VALUECALC_1101_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1101;
}

class ValueCalc_1101 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1101(QWidget *parent = 0);
    ~ValueCalc_1101();

private slots:
    void slot_calc(QAbstractButton*);

private:
    Ui::ValueCalc_1101 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1101_H
