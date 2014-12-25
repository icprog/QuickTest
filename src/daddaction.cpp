#include "daddaction.h"
#include "ui_daddaction.h"
#include <QPushButton>
#include <QStandardItemModel>

#include "quicktestcore.h"
#include <QMessageBox>

extern QuickTestCore core;

dAddAction::dAddAction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dAddAction),
    m_actionNum(1101)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

dAddAction::~dAddAction()
{
    delete ui;
}

void dAddAction::initWindow()
{
    this->setWindowTitle(QObject::tr("添加测试项目"));
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

    CactionModel *model = core.actionModel();
    if (!model) {
        QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("获取测试模型失败!"), QMessageBox::Ok);
    }

    ui->comboBox_class->addItems(model->classNames());

    if (ui->comboBox_class->count() > 0) {

        ui->comboBox_status->addItems(model->classList().at(0)->statusNames());

        if (ui->comboBox_status->count() > 0) {

            foreach (CactionItem *item, model->classList().at(0)->statusList().at(0)->actionList()) {
                ui->comboBox_action->addItem(item->name(), item->id());
            }
//            ui->comboBox_action->addItems(model->classList().at(0)->statusList().at(0)->actionNames());

        }

    }

    ui->comboBox_count->addItems(QStringList() << "1" << "2" << "3" << "4");
    ui->comboBox_count->setCurrentIndex(3);
    ui->checkBox_auto->setChecked(true);
    ui->checkBox_board->setDisabled(true);
    ui->comboBox_count->setDisabled(true);

    //if client is connected
//    ui->checkBox_board->setEnabled(true);
}

void dAddAction::initSignals()
{
    this->connect(ui->comboBox_class, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_classIndexChanged(QString)));
    this->connect(ui->comboBox_status, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_statusIndexChanged(QString)));
    this->connect(ui->comboBox_action, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_actionIndexChanged(QString)));

    this->connect(ui->comboBox_count, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_countIndexChanged(int)));

    this->connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_accepted(QAbstractButton*)));
}

void dAddAction::slot_classIndexChanged(QString curClassName)
{
    if (0 == ui->comboBox_class->count()) {
        ui->comboBox_class->setDisabled(true);
        ui->comboBox_status->setDisabled(true);
        ui->comboBox_action->setDisabled(true);
        return;
    }
    foreach (CclassItem  *item, core.actionModel()->classList()) {
        if (item->name() == curClassName) {
            ui->comboBox_status->blockSignals(true);
            ui->comboBox_status->clear();
            ui->comboBox_status->blockSignals(false);
            ui->comboBox_status->addItems(item->statusNames());
            break;
        }
    }
}
void dAddAction::slot_statusIndexChanged(QString curStatusName)
{
    if (0 == ui->comboBox_status->count()) {
        ui->comboBox_class->setDisabled(true);
        ui->comboBox_status->setDisabled(true);
        ui->comboBox_action->setDisabled(true);
        return;
    }

    foreach (CclassItem *classItem, core.actionModel()->classList()) {
        if (classItem->name() == ui->comboBox_class->currentText()) {

            foreach (CstatusItem *status, classItem->statusList()) {

                if (status->name() == curStatusName) {

                    ui->comboBox_action->blockSignals(true);
                    ui->comboBox_action->clear();
                    ui->comboBox_action->blockSignals(false);
                    ui->comboBox_action->addItems(status->actionNames());
                    break;
                }
            }
            break;
        }
    }
}
void dAddAction::slot_actionIndexChanged(QString curActionName)
{
    if (0 == ui->comboBox_action->count()) {
        ui->comboBox_class->setDisabled(true);
        ui->comboBox_status->setDisabled(true);
        ui->comboBox_action->setDisabled(true);
        return;
    }

    foreach (CclassItem *classItem, core.actionModel()->classList()) {
        if (classItem->name() == ui->comboBox_class->currentText()) {

            foreach (CstatusItem *status, classItem->statusList()) {

                if (status->name() == ui->comboBox_status->currentText()) {

                    foreach (CactionItem *action, status->actionList()) {

                        if (action->name() == curActionName) {
                            m_actionNum = action->id();
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
}
void dAddAction::slot_countIndexChanged(int)
{}
#include <QDebug>
void dAddAction::slot_accepted(QAbstractButton* btn)
{
    if (btn == ui->buttonBox->button(QDialogButtonBox::Ok)) {   //

        //íãâÿÅÒØˆæ—Ø£¡—øÄ¡ÇÅå¥šýÕ
//        core.sysTempalte()->appendAction(m_testNo, ui->checkBox_board->isChecked(), ui->checkBox_auto->isChecked(),
//                                         ui->comboBox_class->currentText(), ui->comboBox_status->currentText(), ui->comboBox_test->currentText());
        CTestAction *action = new CTestAction(ui->comboBox_class->currentText(),
                                      ui->comboBox_status->currentText(),
                                      ui->comboBox_action->currentText(),
                                      m_actionNum,
                                      ui->checkBox_board->isChecked(),
                                      ui->checkBox_auto->isChecked());
        if (!core.templateModel()->appendAction(action)) {
            QMessageBox::warning(this, QObject::tr("添加失败"), QObject::tr("测试项目重复"), QMessageBox::Ok);
            delete action;
        }
    } else if(btn == ui->buttonBox->button(QDialogButtonBox::Cancel)) { //
//        qDebug() << "cancel";
        //do nohting.
    } else {
        //do nothing.
        return ;
    }
}
