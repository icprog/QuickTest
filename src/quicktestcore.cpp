#include "quicktestcore.h"

QuickTestCore::QuickTestCore(QObject *parent) :
    QObject(parent)
{
    m_DactionModel = new CactionModel();

    m_DtemplateModel = new CTemplateModel();
}

QuickTestCore::~QuickTestCore()
{
    delete m_DactionModel;
    m_DactionModel = 0;
    delete m_DtemplateModel;
    m_DtemplateModel = 0;
}

#include <QDebug>
#include "filepath.h"

bool QuickTestCore::initialize(QString *strErr)
{

#if defined (Q_OS_WIN32)
    if (!m_DactionModel->loadData(QString("%1/%2").arg(QApplication::applicationDirPath()).arg(WIN_ACTION_MODEL_FILE))) {
        *strErr = QString("测试项目表加载失败");
        return false;
    }
#elif defined (Q_OS_LINUX)
    if (!m_DactionModel->loadData(QString("%1/%2").arg(QApplication::applicationDirPath()).arg(LINUX_ACTION_MODEL_FILE))) {
        *strErr = QString("测试项目表加载失败");
        return false;
    }
#endif

//    m_DtemplateModel->clear();


        if (!this->substationServer()->init()) {
            *strErr = QString("MMS初始化失败！");
            return false;
        }

    return true;
}

//bool QuickTestCore::buildTemplate()
//{
//    if (m_DtemplateModel) {
//        return false;
//    } else {
//        m_DtemplateModel = new CTemplateModel();
//    }
//    return true;
//}


int QuickTestCore::faultType(const QString faultTypeName)
{

    if (faultTypeName == "A相接地") {
        return Fault_AE;
    } else if (faultTypeName == "B相接地") {
        return Fault_BE;
    } else if (faultTypeName == "C相接地") {
        return Fault_CE;
    } else if (faultTypeName == "AB短路"){
        return Fault_AB;
    } else if (faultTypeName == "BC短路"){
        return Fault_BC;
    } else if (faultTypeName == "CA短路"){
        return Fault_CA;
    } else if (faultTypeName == "AB接地"){
        return Fault_ABE;
    } else if (faultTypeName == "BC接地"){
        return Fault_BCE;
    } else if (faultTypeName == "CA接地"){
        return Fault_CAE;
    } else if (faultTypeName == "三相接地"){
        return Fault_ABC;
    } else {
        //never.
    }

    return Fault_ANY;
}
