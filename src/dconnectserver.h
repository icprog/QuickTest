#ifndef DCONNECTSERVER_H
#define DCONNECTSERVER_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class dConnectServer;
}

class dConnectServer : public QDialog
{
    Q_OBJECT

public:
    explicit dConnectServer(QWidget *parent = 0);
    ~dConnectServer();

    bool updateThis();

signals:
    void signalConnect();
    void signalDisconnect();

private slots:
    void slotConnect();
    void slotConnectStateChanged(int);
//    void slotConnectFaild(QString);
    void slotHint(QString);

    void on_btn_setIP_clicked();

    void on_btn_savesoftplate_clicked();

    void slotExportDone(bool);
private:
    Ui::dConnectServer *ui;

    void initWindow();
    void initSignals();
};

#endif // DCONNECTSERVER_H
