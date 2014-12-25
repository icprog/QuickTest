#ifndef VALUECALC_1208_H
#define VALUECALC_1208_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1208;
}

class ValueCalc_1208 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1208(QWidget *parent = 0);
    ~ValueCalc_1208();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1208 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1208_H
