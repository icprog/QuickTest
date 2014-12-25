#ifndef VALUECALC_1105_H
#define VALUECALC_1105_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class ValueCalc_1105;
}

class ValueCalc_1105 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1105(QWidget *parent = 0);
    ~ValueCalc_1105();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1105 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1105_H
