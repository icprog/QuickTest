#ifndef VALUECALC_1209_H
#define VALUECALC_1209_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1209;
}

class ValueCalc_1209 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1209(QWidget *parent = 0);
    ~ValueCalc_1209();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1209 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1209_H
