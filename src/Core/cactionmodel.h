#ifndef CACTIONMODEL_H
#define CACTIONMODEL_H


#include <QXmlStreamReader>
#include <QList>
#include <QStringList>
class CactionItemBase
{
public:
    CactionItemBase(QString name, int num);
    QString name() {return m_name;}
    int id() {return m_num;}
protected:
    QString m_name;
    int m_num;
};

class CactionItem : public CactionItemBase
{
public:
    CactionItem(QString name, int num);
};

class CstatusItem : public CactionItemBase
{
public:
    CstatusItem(QString name, int num);

    void appendAction(CactionItem* item) {return m_list.append(item);}

    QList<CactionItem*> actionList() {return m_list;}
    QStringList actionNames();
private:
    QList<CactionItem*> m_list;
};

class CclassItem : public CactionItemBase
{
public:
    CclassItem(QString name, int num);

    void appendStatus(CstatusItem* item) {return m_list.append(item);}

    QList<CstatusItem*> statusList() {return m_list;}
    QStringList statusNames();

private:
    QList<CstatusItem*> m_list;
};

class CactionModel
{
public:
    CactionModel();

    bool loadData(QString path);

    QList<CclassItem*> classList() {return m_list;}
    QStringList classNames();

private:

    QList<CclassItem*>  m_list;

    bool parseClass(QXmlStreamReader &classNode);
    bool parseStatus(QXmlStreamReader &StatusNode);
    bool parseAction(QXmlStreamReader &TestNode);
};

#endif // CACTIONMODEL_H
