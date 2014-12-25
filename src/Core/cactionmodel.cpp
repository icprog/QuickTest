#include "cactionmodel.h"

#include <QFile>

CactionItemBase::CactionItemBase(QString name, int num)
    :m_name(name),m_num(num)
{
}

CactionItem::CactionItem(QString name, int num)
    :CactionItemBase(name,num)
{}

CstatusItem::CstatusItem(QString name, int num)
    :CactionItemBase(name,num)
{}

QStringList CstatusItem::actionNames()
{
    QStringList names;
    foreach (CactionItem *item, this->m_list) {
        names << item->name();
    }
    return names;
}

CclassItem::CclassItem(QString name, int num)
    :CactionItemBase(name,num)
{
}

QStringList CclassItem::statusNames()
{
    QStringList names;
    foreach (CstatusItem *item, this->m_list) {
        names << item->name();
    }
    return names;
}

CactionModel::CactionModel()
{
}

QStringList CactionModel::classNames()
{
    QStringList names;
    foreach (CclassItem *item, this->m_list) {
        names << item->name();
    }
    return names;
}
#include <QDebug>
bool CactionModel::loadData(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        /* 致命错误，打开方案脚本错误 */
        qDebug() << path << file.errorString ();
        return false;
    }

    /* 解析sys_testmodel */
    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        //读取下一个element.
        QXmlStreamReader::TokenType token = reader.readNext();

        //如果获取的仅为StartDocument,则进行下一个
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        } else if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "class") {

                if (!parseClass(reader)) {
                        return false;
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (reader.name() == "model") {
                break;
            }
        }
    }

    if (reader.hasError()) {
        //write error msg in log.
        return false;
    }

    file.close();
    return true;
}

CclassItem  *g_pClass = NULL;
CstatusItem *g_pStatus = NULL;
CactionItem *g_pAction = NULL;

bool CactionModel::parseClass(QXmlStreamReader &classNode)
{
    if (classNode.tokenType() != QXmlStreamReader::StartElement &&
            classNode.name() != "class") {
        return false;
    }

    /***/
    QXmlStreamAttributes atts =  classNode.attributes();
    g_pClass = new CclassItem(atts.value("name").toString(), atts.value("no").toString().toInt());
    this->m_list.append(g_pClass);
    /***/

    while(!classNode.atEnd() && !classNode.hasError()) {
        QXmlStreamReader::TokenType token = classNode.readNext();

        if (token == QXmlStreamReader::StartElement) {

            if (classNode.name() == "status") {

                if (!parseStatus(classNode)) {
                    return false;
                }

            }

        } else if ( token == QXmlStreamReader::EndElement) {

            if (classNode.name() == "class") {
                break;
            }

        }

        if (classNode.hasError()) {
            //write err in log.
            return false;
        }
    }

    return true;
}
bool CactionModel::parseStatus(QXmlStreamReader &statusNode)
{
    if (statusNode.tokenType() != QXmlStreamReader::StartElement &&
            statusNode.name() != "status") {
        return false;
    }

    /***/
    QXmlStreamAttributes atts = statusNode.attributes();
    if (g_pClass) {
        g_pStatus = new CstatusItem(atts.value("name").toString(), atts.value("no").toString().toInt());
        g_pClass->appendStatus(g_pStatus);
    } else {
        //... do nothing;
    }
    /***/

    while(!statusNode.atEnd() && !statusNode.hasError()) {
        QXmlStreamReader::TokenType token = statusNode.readNext();

        if (token == QXmlStreamReader::StartElement) {

            if (statusNode.name() == "test") {

                if (!parseAction(statusNode)) {
                    return false;
                }

            }

        } else if ( token == QXmlStreamReader::EndElement) {

            if (statusNode.name() == "status") {
                break;
            }

        }

        if (statusNode.hasError()) {
            //write err in log.
            return false;
        }
    }

    return true;
}
bool CactionModel::parseAction(QXmlStreamReader &actionNode)
{
    if (actionNode.tokenType() != QXmlStreamReader::StartElement &&
            actionNode.name() != "test") {
        return false;
    }

    /***/
    QXmlStreamAttributes atts = actionNode.attributes();
    if (g_pStatus) {
        g_pAction = new CactionItem(atts.value("name").toString(), atts.value("no").toString().toInt());
        g_pStatus->appendAction(g_pAction);
    } else {
        //...do nothing;
    }
    /***/

    return true;
}
