#ifndef DACTIONCONDITION_H
#define DACTIONCONDITION_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class dActionCondition;
}

class dActionCondition : public QDialog
{
    Q_OBJECT

public:
    explicit dActionCondition(QWidget *parent = 0);
    ~dActionCondition();

    bool updateThis(int index);
private slots:
    void slot_timeStateChanged(int);
    void slot_timeValueChanged(QString);
    void slot_actionStateChanged(int);
    void slot_statePointChanged(int);

    void slot_accepted(QAbstractButton*);
private:
    Ui::dActionCondition *ui;
    int m_curIndex;

    void initWindow();
    void initSignals();
};

#endif // DACTIONCONDITION_H
