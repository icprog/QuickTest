#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "wbuildtemplate.h"
#include "mappinglinkwidget.h"
#include "./test/quicktestwidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void	keyPressEvent ( QKeyEvent * event );

private slots:
    void btn_help_clicked();

    void btn_build_clicked();

    void btn_open_clicked();

    void btn_test_clicked();

    void btn_sysconfig_clicked();

//    void btn_quit_clicked();

    void slot_returnToMainWindow();
private:
    Ui::MainWindow *ui;

    wBuildTemplate   m_wBuildTemplate;
    MappingLinkWidget m_mappingLinkWidget;
    QuickTestWidget m_testWidget;


    void initWindow();
    void initSignals();

    QPushButton *btn_help;
    QPushButton *btn_build;
    QPushButton *btn_open;
    QPushButton *btn_sysconfig;
    QPushButton *btn_test;
    QPushButton *btn_quit;
};

#endif // MAINWINDOW_H
