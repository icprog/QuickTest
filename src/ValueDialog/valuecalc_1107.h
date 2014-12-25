#ifndef VALUECALC_1107_H
#define VALUECALC_1107_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1107;
}

class ValueCalc_1107 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1107(QWidget *parent = 0);
    ~ValueCalc_1107();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1107 *ui;


    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1107_H
