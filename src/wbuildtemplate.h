#ifndef WBUILDTEMPLATE_H
#define WBUILDTEMPLATE_H

#include <QWidget>
#include <QStandardItemModel>

#include "daddaction.h"
#include "wactionconfig.h"
#include "dconnectserver.h"
namespace Ui {
class wBuildTemplate;
}

class wBuildTemplate : public QWidget
{
    Q_OBJECT

public:
    explicit wBuildTemplate(QWidget *parent = 0);
    ~wBuildTemplate();

    bool updateWidgets();

protected:
    virtual void	keyPressEvent ( QKeyEvent * event );

signals:
    void signal_return();

private slots:
    void btn_help_clicked();

    void btn_add_clicked();

    void btn_config_clicked();

    void btn_client_clicked();

    void btn_delete_clicked();

    void btn_save_clicked();

//    void btn_cancel_clicked();

    void slot_updateActionTree();

    void slot_selectedAction(QModelIndex);

    void slot_configAccepted();
private:
    Ui::wBuildTemplate *ui;

    QStandardItemModel  m_DactionsModel;

    dAddAction          m_DialogAddAction;

    wActionConfig       *m_pActionConfig;

    int m_curIndexRow;

    void initWindow();
    void initSignals();

    dConnectServer server;

    QPushButton *btn_help;
    QPushButton *btn_add;
    QPushButton *btn_config;
    QPushButton *btn_client;
    QPushButton *btn_delete;
    QPushButton *btn_save;
    QPushButton *btn_cancel;
};

#endif // WBUILDTEMPLATE_H
