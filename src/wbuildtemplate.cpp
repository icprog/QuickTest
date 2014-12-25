#include "wbuildtemplate.h"
#include "ui_wbuildtemplate.h"

#include "quicktestcore.h"
extern QuickTestCore core;

#include <QMessageBox>

wBuildTemplate::wBuildTemplate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wBuildTemplate),
    m_pActionConfig(NULL)
{
    ui->setupUi(this);

    initWindow();
    initSignals();
}

wBuildTemplate::~wBuildTemplate()
{
    delete m_pActionConfig;
    m_pActionConfig = 0;
    delete ui;
}
#include "customWidget/consolecontainer.h"
void wBuildTemplate::initWindow()
{
    m_DactionsModel.setColumnCount(4);
    m_DactionsModel.setHeaderData(0, Qt::Horizontal, QObject::tr(""));
    m_DactionsModel.setHeaderData(1, Qt::Horizontal, QObject::tr("软压板自动投退"));
    m_DactionsModel.setHeaderData(2, Qt::Horizontal, QObject::tr("测试等待"));
    m_DactionsModel.setHeaderData(3, Qt::Horizontal, QObject::tr("数据已配置"));

    ui->treeView->setModel(&m_DactionsModel);
    ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setDefaultSectionSize(80);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);//关闭修改

    QList<QPushButton*> btnList;
    btn_help = new QPushButton(QObject::trUtf8("帮助"), ui->navi);
    connect(btn_help, SIGNAL(clicked()), SLOT(btn_help_clicked()));
    btnList.append(btn_help);

    btn_add = new QPushButton(QObject::trUtf8("添加\n项目"), ui->navi);
    connect(btn_add, SIGNAL(clicked()), SLOT(btn_add_clicked()));
    btnList.append(btn_add);

    btn_config = new QPushButton(QObject::trUtf8("参数\n设置"), ui->navi);
    connect(btn_config, SIGNAL(clicked()), SLOT(btn_config_clicked()));
    btnList.append(btn_config);

    btn_client = new QPushButton(QObject::trUtf8("模拟\n客户端"), ui->navi);
    connect(btn_client, SIGNAL(clicked()), SLOT(btn_client_clicked()));
    btnList.append(btn_client);

    btn_delete = new QPushButton(QObject::trUtf8("删除\n项目"), ui->navi);
    connect(btn_delete, SIGNAL(clicked()), SLOT(btn_delete_clicked()));
    btnList.append(btn_delete);

    btn_save = new QPushButton(QObject::trUtf8("保存\n模板"), ui->navi);
    connect(btn_save, SIGNAL(clicked()), SLOT(btn_save_clicked()));
    btnList.append(btn_save);

//    btn_cancel = new QPushButton(QObject::trUtf8("返回"), ui->navi);
//    connect(btn_cancel, SIGNAL(clicked()), SLOT(btn_cancel_clicked()));
//    btnList.append(btn_cancel);

    ui->navi->addButtons(btnList);

    btn_config->setDisabled(true);
    btn_delete->setDisabled(true);
}

void wBuildTemplate::initSignals()
{
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_selectedAction(QModelIndex)));

    connect(&m_DialogAddAction, SIGNAL(accepted()), this, SLOT(slot_updateActionTree()));
}
#include <QKeyEvent>
void wBuildTemplate::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        emit signal_return();
        break;
    default:
        QWidget::keyPressEvent(event);
    }

    return ;
}

bool wBuildTemplate::updateWidgets()
{
    core.templateModel()->clear();
    m_DactionsModel.removeRows(0, m_DactionsModel.rowCount());
    return true;
}

void wBuildTemplate::btn_help_clicked()
{
    //btn_help clicked.
}

void wBuildTemplate::btn_add_clicked()
{
    m_DialogAddAction.exec();
}

void wBuildTemplate::btn_config_clicked()
{

    m_pActionConfig = new wActionConfig(core.templateModel()->currentAction()->id(), 0);
    if (m_pActionConfig) {
        connect(m_pActionConfig, SIGNAL(signal_accepted()), this, SLOT(slot_configAccepted()));
    } else {
        disconnect(m_pActionConfig, SIGNAL(signal_accepted()), this, SLOT(slot_configAccepted()));
    }
    ui->stackedWidget->addWidget(m_pActionConfig);
    ui->stackedWidget->setCurrentWidget(m_pActionConfig);
}

void wBuildTemplate::btn_client_clicked()
{
    if (server.updateThis()) {
        server.exec();
    } else {
        QMessageBox::warning(this, QObject::tr("客户端连接失败"), QObject::tr("客户端模块加载本地数据失败"), QMessageBox::Ok);
    }
}

void wBuildTemplate::btn_delete_clicked()
{
    core.templateModel()->deleteAction(core.templateModel()->currentAction()->id());
    this->slot_updateActionTree();
    btn_config->setDisabled(true);
    btn_delete->setDisabled(true);
}

#include <QDateTime>
void wBuildTemplate::btn_save_clicked()
{
    QString detail;

    QString fileName = QDateTime::currentDateTime().toString("yy-M-d-hh_mm_ss");
    if (core.templateModel()->save(QString("%1.prj").arg(fileName))) {
        detail = QObject::tr("%1 保存成功").arg(fileName);
    } else {
        detail = QObject::tr("%1 保存失败").arg(fileName);
    }
    QMessageBox::warning(this, QObject::tr("保存"), detail, QMessageBox::Ok);
}

/*
void wBuildTemplate::btn_cancel_clicked()
{
    //clear & quit.
    emit signal_return();
}
*/

void wBuildTemplate::slot_updateActionTree()
{
    m_DactionsModel.removeRows(0, m_DactionsModel.rowCount());

    CTemplateModel *model = core.templateModel();
//    QStandardItem *classItem = NULL;

//    foreach (CTestAction *action, model->actions()) {
//        QString className = action->name().first();
//        QString statusName = action->name().at(1);
//        QString actionName = action->name().last();

//        QList<QStandardItem*> classNode = this->m_DactionsModel.findItems(className, Qt::MatchRecursive, 0);
//        if (classNode.isEmpty()) {
//            QStandardItem *classItem = new QStandardItem(className);
//            QStandardItem *statusItem = new QStandardItem(statusName);
//            classItem->appendRow(statusItem);
//            QStandardItem *actionItem = new QStandardItem(actionName);
//            //        actionItem->setData(action, Qt::UserRole+2);
//            statusItem->appendRow(actionItem);

//            m_DactionsModel.appendRow(classItem);

//            QStandardItem *boardItem = new QStandardItem();
//            if(action->isAutoBoard()) {
//                boardItem->setCheckState(Qt::Checked);
//            } else {
//                boardItem->setCheckState(Qt::Unchecked);
//            }
//            statusItem->setChild(actionItem->row(), 1, boardItem);

//            QStandardItem *autoItem = new QStandardItem();
//            autoItem->setCheckable(true);
//            if(action->isAutoTest()) {
//                autoItem->setCheckState(Qt::Checked);
//            } else {
//                autoItem->setCheckState(Qt::Unchecked);
//            }
//            statusItem->setChild(actionItem->row(), 2, autoItem);

//            QStandardItem *configItem = new QStandardItem();
//            configItem->setCheckable(true);
//            if(action->isConfiged()) {
//                configItem->setCheckState(Qt::Checked);
//            } else {
//                configItem->setCheckState(Qt::Unchecked);
//            }
//            statusItem->setChild(actionItem->row(), 3, configItem);
//        } else {
//            QList<QStandardItem*> statusNode = this->m_DactionsModel.findItems(statusName, Qt::MatchRecursive, 0);
//            if (statusNode.isEmpty()) {
//                QStandardItem *statusItem = new QStandardItem(statusName);
//                classNode.first()->appendRow(statusItem);
//                QStandardItem *actionItem = new QStandardItem(actionName);
//                //        actionItem->setData(action, Qt::UserRole+2);
//                statusItem->appendRow(actionItem);

////                m_DactionsModel.appendRow(classNode.first());

//                QStandardItem *boardItem = new QStandardItem();
//                if(action->isAutoBoard()) {
//                    boardItem->setCheckState(Qt::Checked);
//                } else {
//                    boardItem->setCheckState(Qt::Unchecked);
//                }
//                statusItem->setChild(actionItem->row(), 1, boardItem);

//                QStandardItem *autoItem = new QStandardItem();
//                autoItem->setCheckable(true);
//                if(action->isAutoTest()) {
//                    autoItem->setCheckState(Qt::Checked);
//                } else {
//                    autoItem->setCheckState(Qt::Unchecked);
//                }
//                statusItem->setChild(actionItem->row(), 2, autoItem);

//                QStandardItem *configItem = new QStandardItem();
//                configItem->setCheckable(true);
//                if(action->isConfiged()) {
//                    configItem->setCheckState(Qt::Checked);
//                } else {
//                    configItem->setCheckState(Qt::Unchecked);
//                }
//                statusItem->setChild(actionItem->row(), 3, configItem);
//            } else {
//                QStandardItem *actionItem = new QStandardItem(actionName);
//                //        actionItem->setData(action, Qt::UserRole+2);
//                statusNode.first()->appendRow(actionItem);

////                m_DactionsModel.appendRow(classNode.first());

//                QStandardItem *boardItem = new QStandardItem();
//                if(action->isAutoBoard()) {
//                    boardItem->setCheckState(Qt::Checked);
//                } else {
//                    boardItem->setCheckState(Qt::Unchecked);
//                }
//                statusNode.first()->setChild(actionItem->row(), 1, boardItem);

//                QStandardItem *autoItem = new QStandardItem();
//                autoItem->setCheckable(true);
//                if(action->isAutoTest()) {
//                    autoItem->setCheckState(Qt::Checked);
//                } else {
//                    autoItem->setCheckState(Qt::Unchecked);
//                }
//                statusNode.first()->setChild(actionItem->row(), 2, autoItem);

//                QStandardItem *configItem = new QStandardItem();
//                configItem->setCheckable(true);
//                if(action->isConfiged()) {
//                    configItem->setCheckState(Qt::Checked);
//                } else {
//                    configItem->setCheckState(Qt::Unchecked);
//                }
//                statusNode.first()->setChild(actionItem->row(), 3, configItem);
//            }
//        }

//    }

    foreach (CTestAction *action, model->actions()) {

        QStandardItem *itemType = NULL;
        QStandardItem *itemInst = NULL;
        QStandardItem *itemTest = NULL;

        QList<QStandardItem*> listType = m_DactionsModel.findItems(action->name().first(), Qt::MatchRecursive, 0);
        if (listType.isEmpty()) {
            //新建Type节点
            itemType = new QStandardItem(action->name().first());
            itemType->setSelectable(false);
            itemType->setEditable(false);
            m_DactionsModel.appendRow(itemType);
        } else {
            itemType = listType.first();
            if (!itemType) {
                continue;
            }
        }

        if (!itemType) {
            continue;
        }
        QList<QStandardItem*> listInst = m_DactionsModel.findItems(action->name().at(1), Qt::MatchRecursive, 0);
        if (listInst.isEmpty()) {
            //新建Inst节点
            itemInst = new QStandardItem(action->name().at(1));
            itemInst->setSelectable(false);
            itemInst->setEditable(false);
            itemType->appendRow(itemInst);
        } else {

            foreach (QStandardItem*match, listInst) {
                QStandardItem *matchType = match->parent();
                if (matchType && (matchType->data(Qt::DisplayRole).toString() == action->name().first())) {
                    itemInst = match;
                }
            }
            if (!itemInst) {
                //新建Inst节点
                itemInst = new QStandardItem(action->name().at(1));
                itemInst->setSelectable(false);
                itemInst->setEditable(false);
                itemType->appendRow(itemInst);
            }

        }

        if (!itemInst) {
            continue;
        }
        itemTest = new QStandardItem(action->name().last());
        itemTest->setEditable(false);
        itemInst->appendRow(itemTest);

        QStandardItem *boardItem = new QStandardItem();
        boardItem->setCheckable(true);
        if(action->isAutoBoard()) {
            boardItem->setCheckState(Qt::Checked);
        } else {
            boardItem->setCheckState(Qt::Unchecked);
        }
        itemInst->setChild(itemTest->index().row(), 1, boardItem);

        QStandardItem *autoItem = new QStandardItem();
        autoItem->setCheckable(true);
        if(action->isAutoTest()) {
            autoItem->setCheckState(Qt::Checked);
        } else {
            autoItem->setCheckState(Qt::Unchecked);
        }
        itemInst->setChild(itemTest->index().row(), 2, autoItem);

        QStandardItem *configItem = new QStandardItem();
        configItem->setCheckable(true);
        if(action->isConfiged()) {
            configItem->setCheckState(Qt::Checked);
        } else {
            configItem->setCheckState(Qt::Unchecked);
        }
        itemInst->setChild(itemTest->index().row(), 3, configItem);
    }

    ui->treeView->expandAll();


//        QList<QStandardItem*> items =  m_DactionsModel.findItems(className, Qt::MatchRecursive, 0);
//        if (!items.isEmpty()) {
//            classItem = items.first();
//        }

//        if (m_DactionsModel.findItems())
//        QStandardItem *classItem = new QStandardItem(className);

}
#include <QDebug>
void wBuildTemplate::slot_selectedAction(QModelIndex index)
{

    CTestAction *action = core.templateModel()->findActionByName(index.sibling(index.row(), 0).data(Qt::DisplayRole).toString());
    if (action) {
        btn_config->setEnabled(true);
        btn_delete->setEnabled(true);
    } else {
        btn_config->setDisabled(true);
        btn_delete->setDisabled(true);
    }
    core.templateModel()->setCurrentAction(action);
    m_curIndexRow = index.row();
}

void wBuildTemplate::slot_configAccepted()
{
    //updateTree configed.
//    m_DactionsModel.item(m_curIndexRow, 2)->setCheckState(Qt::Checked);
//    m_treeIndex.sibling(m_treeIndex.row(), 3)
    ui->page->setFocus();
    //更新tree
    ui->stackedWidget->removeWidget(m_pActionConfig);
    this->clearFocus();
    btn_config->setDisabled(true);
    btn_delete->setDisabled(true);
    ui->stackedWidget->setCurrentWidget(ui->page);
}
