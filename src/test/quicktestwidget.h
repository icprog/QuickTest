#ifndef QUICKTESTWIDGET_H
#define QUICKTESTWIDGET_H

#include <QWidget>
#include "quicktestinterface.h"
#include <QtNetwork/QUdpSocket>
#include <QPushButton>
#define HostAddress QHostAddress("192.168.1.3")
#define HostPort 2003

namespace Ui {
class QuickTestWidget;
class QuickTestConfigDialog;
}

class QuickTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickTestWidget(QWidget *parent = 0);
    ~QuickTestWidget();

    bool initWidget();

protected:
    virtual void	keyPressEvent ( QKeyEvent * event );

signals:
    void start();
    void stop();

    void quit();

private slots:
    void btn_help_clicked();

    void btn_scheme_clicked();

    void btn_start_clicked();

    void btn_report_clicked();

    void btn_config_clicked();

    void updateUI();

//    void updateHint(QString);
    void interrupt(QString);

    void connectFailure(QString);

private:
    Ui::QuickTestWidget *ui;
    Ui::QuickTestConfigDialog *ui_configDialog;
    QDialog *configDialog;

    QuickTestInterface  *d;

    QPushButton *btn_help;
    QPushButton *btn_scheme;
    QPushButton *btn_start;
    QPushButton *btn_report;
    QPushButton *btn_config;
    QPushButton *btn_quit;
private:

    void stopTest();

    bool isTestLasting();
};

#endif // QUICKTESTWIDGET_H
