#ifndef VALUECALC_1207_H
#define VALUECALC_1207_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ValueCalc_1207;
}

class ValueCalc_1207 : public QDialog
{
    Q_OBJECT

public:
    explicit ValueCalc_1207(QWidget *parent = 0);
    ~ValueCalc_1207();

private slots:
    void slot_calc(QAbstractButton*);
private:
    Ui::ValueCalc_1207 *ui;

    void initWindow();
    void initSignals();
};

#endif // VALUECALC_1207_H
