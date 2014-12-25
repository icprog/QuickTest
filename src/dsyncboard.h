#ifndef DSYNCBOARD_H
#define DSYNCBOARD_H

#include <QDialog>
#include <QPushButton>
#include <QStandardItemModel>
#include "Core/Client/clientcmdthread.h"
namespace Ui {
class dSyncBoard;
}

class dSyncBoard : public QDialog
{
    Q_OBJECT

public:
    explicit dSyncBoard(QWidget *parent = 0);
    ~dSyncBoard();

    bool  updateThis(bool connected);

signals:
    void signalRequireSoftPlateData();

private slots:
    void slot_accepted(QAbstractButton*);
    void on_btn_import_clicked();

    void slotShowSoftPlateBefore();
    void slotShowSoftPlateAfter();

    void slotSoftPlateItemChanged(QStandardItem*);

    void slotUpdateServerInfo();
    void slotUpdateFromServer();


private:
    Ui::dSyncBoard *ui;

    ClientCmdThread  *m_pThread;                   //命令处理线程
    QString          m_curServerName;
    bool bUpdateSoftPlate;
    QStandardItemModel  m_softPlateModel[2];

    void initWindow();
    void initSignals();

    void showSoftPlateBefore();
    void showSoftPlateAfter();

    bool getItemFlagsOfSoftPlate(QList<MODIFY_ITEM *> &settingList);

    void saveSoftPlateData();

    void updateFromLocal(QString);
};

#endif // DSYNCBOARD_H
