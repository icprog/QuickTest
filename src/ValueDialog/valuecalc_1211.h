#ifndef VALUECALC_1211_H
#define VALUECALC_1211_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1211;
}

class ValueCalc_1211 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1211(QWidget *parent = 0);
    ~ValueCalc_1211();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1211 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1211_H
