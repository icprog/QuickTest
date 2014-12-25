#ifndef VALUECALC_1103_H
#define VALUECALC_1103_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1103;
}

class ValueCalc_1103 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1103(QWidget *parent = 0);
    ~ValueCalc_1103();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1103 *ui;


    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1103_H
