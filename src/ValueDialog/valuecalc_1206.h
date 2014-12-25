#ifndef VALUECALC_1206_H
#define VALUECALC_1206_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1206;
}

class ValueCalc_1206 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1206(QWidget *parent = 0);
    ~ValueCalc_1206();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1206 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1206_H
