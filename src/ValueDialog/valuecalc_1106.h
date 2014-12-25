#ifndef VALUECALC_1106_H
#define VALUECALC_1106_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1106;
}

class ValueCalc_1106 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1106(QWidget *parent = 0);
    ~ValueCalc_1106();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1106 *ui;


    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1106_H
