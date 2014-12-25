#ifndef WACTIONCONFIG_H
#define WACTIONCONFIG_H

#include <QWidget>


#include "./ValueDialog/valuecalc.h"
#include "dsyncboard.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#define ACTION_MAX_CIRCUIT_COUNT 4
#define ACTION_MAX_STATE_MODEL_COUNT 4

namespace Ui {
class wActionConfig;
}

class wActionConfig : public QWidget
{
    Q_OBJECT

public:
    explicit wActionConfig(int, QWidget *parent = 0);
    ~wActionConfig();

protected:
    virtual void	keyPressEvent ( QKeyEvent * event );

signals:
    void signal_accepted();
private slots:
    void btn_help_clicked();

    void btn_calc_clicked();

    void btn_control_clicked();

    void btn_yb_clicked();

//    void btn_return_clicked();

    void slot_tabChanged(int);

    void slot_circuit_1(int);
    void slot_circuit_2(int);
    void slot_circuit_3(int);
    void slot_circuit_4(int);
private:
    Ui::wActionConfig *ui;

    int m_actionId;
    QGridLayout *m_centerLayout;
    QTableView  m_tableView;
    QCheckBox   m_circuit[ACTION_MAX_CIRCUIT_COUNT];

    QStandardItemModel  m_stateModel[ACTION_MAX_STATE_MODEL_COUNT];

    dSyncBoard m_boardInfo;

    int nCurrentState;

    void initWindow();
    void initSignals();

    void updateTableData();
    void loadCircuitGroupState(int);

private:
    QPushButton *btn_help;
    QPushButton *btn_calc;
    QPushButton *btn_control;
    QPushButton *btn_yb;
    QPushButton *btn_return;
};

#endif // WACTIONCONFIG_H
