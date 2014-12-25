#ifndef VALUECALC_1204_H
#define VALUECALC_1204_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1204;
}

class ValueCalc_1204 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1204(QWidget *parent = 0);
    ~ValueCalc_1204();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1204 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1204_H
