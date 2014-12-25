#ifndef VALUECALC_1210_H
#define VALUECALC_1210_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1210;
}

class ValueCalc_1210 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1210(QWidget *parent = 0);
    ~ValueCalc_1210();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1210 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1210_H
