#ifndef VALUECALC_1205_H
#define VALUECALC_1205_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1205;
}

class ValueCalc_1205 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1205(QWidget *parent = 0);
    ~ValueCalc_1205();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1205 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1205_H
