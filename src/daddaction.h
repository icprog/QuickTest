#ifndef DADDACTION_H
#define DADDACTION_H

#include <QDialog>
#include <QPushButton>

typedef struct sturct_ActionUnit{
    sturct_ActionUnit(){reset();}
    void reset()
    {
        testName = "None";
        testNum = 0;
        boardFlag = false;
        autoFlag = false;
    }
    QString testName;
    int testNum;
    bool boardFlag;
    bool autoFlag;
}ACTION;

namespace Ui {
class dAddAction;
}

class dAddAction : public QDialog
{
    Q_OBJECT

public:
    explicit dAddAction(QWidget *parent = 0);
    ~dAddAction();

private slots:
    void slot_classIndexChanged(QString);
    void slot_statusIndexChanged(QString);
    void slot_actionIndexChanged(QString);

    void slot_countIndexChanged(int);

    void slot_accepted(QAbstractButton*);

private:
    Ui::dAddAction *ui;

    int m_actionNum;

    void initWindow();
    void initSignals();
};

#endif // DADDACTION_H
