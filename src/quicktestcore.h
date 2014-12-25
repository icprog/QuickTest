#ifndef QUICKTESTCORE_H
#define QUICKTESTCORE_H

#include <QObject>

#include "pam300share.h"
#include "PAM300Share_global.h"

#include "Core/cactionmodel.h"
#include "Core/ctemplatemodel.h"
#include "Core/csubstationserver.h"

class QuickTestCore : public QObject
{
    Q_OBJECT
public:
    explicit QuickTestCore(QObject *parent = 0);
    ~QuickTestCore();

    bool initialize(QString *strErr);

    CactionModel* actionModel() {return m_DactionModel;}
    CTemplateModel* templateModel() {return m_DtemplateModel;}
    CSubstationServer* substationServer() {return CSubstationServer::getInstance();}

    static int faultType(const QString faultTypeName);
signals:

public slots:

private:
    CactionModel    *m_DactionModel;
    CTemplateModel  *m_DtemplateModel;
};

#endif // QUICKTESTCORE_H
