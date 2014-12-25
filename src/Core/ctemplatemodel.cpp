#include "ctemplatemodel.h"

CCircuit::CCircuit()
    :bState(true)
{}
CCircuit::~CCircuit()
{}

void CCircuit::setEnable(bool state)
{
    this->bState = state;
    return;
}


CCircuitGroup::CCircuitGroup()
    :nCircuitCount(4)
{}
CCircuitGroup::~CCircuitGroup()
{}

CCircuit* CCircuitGroup::circuit(int index)
{
    if (0 > index || 4 < index) {
        return false;
    }
    return &m_circuit[index];
}

CSoftPlate::CSoftPlate()
{}
CSoftPlate::CSoftPlate(QString desc, int value, QString reference)
{
    this->nValue = value;
    this->strDescribe = desc;
    this->strReference = reference;
}

CActionSoftPlate::CActionSoftPlate()
{}
CActionSoftPlate::~CActionSoftPlate()
{
    m_lSoftPlatesBefor.clear();
    m_lSoftPlatesAfter.clear();
}
void CActionSoftPlate::append(eSoftPlate position, QString desc, int value, QString reference)
{
    if (position == Each) {
        m_lSoftPlatesBefor.append(new CSoftPlate(desc,value,reference));
        m_lSoftPlatesAfter.append(new CSoftPlate(desc,value,reference));
    }
    else if (position == Before) {
        m_lSoftPlatesBefor.append(new CSoftPlate(desc,value,reference));
    }
    else if (position == After) {
        m_lSoftPlatesAfter.append(new CSoftPlate(desc,value,reference));
    }

}
void CActionSoftPlate::clearBefore()
{
    m_lSoftPlatesBefor.clear();
}
void CActionSoftPlate::clearAfter()
{
    m_lSoftPlatesBefor.clear();
}
void CActionSoftPlate::clear()
{
    m_lSoftPlatesBefor.clear();
    m_lSoftPlatesAfter.clear();
}

CStateControl::CStateControl()
{
    this->setActionDisable(0);
    this->setActionDisable(2);
}
CStateControl::~CStateControl()
{}

void CStateControl::setTimeEnable(int index,int value)
{
    this->m_stateContorl[index].bTimeControl = true;
    this->m_stateContorl[index].timeValue = value;
}

void CStateControl::setTimeDisable(int index)
{
    this->m_stateContorl[index].bTimeControl = false;
}

void CStateControl::setActionEnable(int index,bool p1, bool p2, bool p3, bool p4)
{
    this->m_stateContorl[index].bActionControl = true;
    this->m_stateContorl[index].bJumpPoint[0] = p1;
    this->m_stateContorl[index].bJumpPoint[1] = p2;
    this->m_stateContorl[index].bJumpPoint[2] = p3;
    this->m_stateContorl[index].bJumpPoint[3] = p4;
}

void CStateControl::setActionDisable(int index)
{
    this->m_stateContorl[index].bActionControl = false;
}

CTestAction::CTestAction(QString className, QString statusName, QString actionName, int actonNum, bool autoBoard, bool autoTest)
    :m_className(className),
      m_statusName(statusName),
      m_actionName(actionName),
      m_actionNum(actonNum),
      bAutoBoard(autoBoard),
      bAutoTest(autoTest),
      bCofiged(false)
{
    this->setDefaultValue();//初始化smv
}

CCircuitGroup* CTestAction::circuit_state(int index)
{
    if (0 > index || index > 4) {
        return NULL;
    }

    return &m_circuit[index];
}

void CTestAction::setDefaultValue()
{
    float deValue = 57.735;
    float deph = 0;
    float deseq = 50.000;
    int i;

    //设置默认故障类型
    for(int i=0;i<4;++i) {
        smvData[i].faultType = 11;
    }

    //state 1-4 -> A相
    for (i=0;i<4;i++)
    {
    smvData[i].Va[0] = deValue;
    smvData[i].Va[1] = deph;
    smvData[i].Va[2] = deseq;
    smvData[i].Ia[0] = 0.000;
    smvData[i].Ia[1] = 0.000;
    smvData[i].Ia[2] = deseq;
    }
    for (i=0;i<4;i++)
    {
    //state 1-4 -> B相
    smvData[i].Vb[0] = deValue;
    smvData[i].Vb[1] = -120;
    smvData[i].Vb[2] = deseq;
    smvData[i].Ib[0] = 0.000;
    smvData[i].Ib[1] = 0.000;
    smvData[i].Ib[2] = deseq;
    }
    for (i=0;i<4;i++)
    {
    //state 1-4 -> C相
    smvData[i].Vc[0] = deValue;
    smvData[i].Vc[1] = 120;
    smvData[i].Vc[2] = deseq;
    smvData[i].Ic[0] = 0.000;
    smvData[i].Ic[1] = 0.000;
    smvData[i].Ic[2] = deseq;
    }
    //state 1-4 -> N相
   for(i=0;i<4;i++)
   {
    smvData[i].Vn[0] = 0.000;
    smvData[i].Vn[1] = 0.000;
    smvData[i].Vn[2] = deseq;
    smvData[i].In[0] = 0.000;
    smvData[i].In[1] = 0.000;
    smvData[i].In[2] = deseq;
   }
}

CTemplateModel::CTemplateModel()
{
    this->clear();
}

void CTemplateModel::clear()
{
    m_list.clear();
}

void CTemplateModel::deleteAction(int testNum)
{
    foreach (CTestAction *action, this->m_list) {
        if (action->id() == testNum) {
            this->m_list.removeOne(action);
            delete action; action = NULL;
        }
    }
}

bool CTemplateModel::appendAction(CTestAction* action)
{
    foreach (CTestAction *actionItem, this->m_list) {
        if (actionItem->name() == action->name()) {
            return false;
        }
    }

    this->m_list.append(action);
    return true;
}

CTestAction* CTemplateModel::findActionByName(QString name)
{
    foreach (CTestAction *action, this->m_list) {
        if (action->name().last() == name)
            return action;
    }
    return NULL;
}
#include <QDir>
#include <QXmlStreamWriter>
#include <QTime>
#include <QDebug>
#include <QApplication>

#include "filepath.h"
bool CTemplateModel::save(QString name)
{

#if defined (Q_OS_WIN32)
    QString path = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(WIN_SCHEME_SAVE_DIR);
#else
    QString path = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(LINUX_SCHEME_SAVE_DIR);
#endif

    QDir dir(path);
    if (!dir.exists()) {
        if (QDir().mkpath(path)) {
            return false;
        }
    }

    QFile file(QString("%1/%2").arg(path).arg(name));
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }


    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    //Top
    writer.writeStartElement("TestTemplate");
    writer.writeAttribute("build", QTime().currentTime().toString());

    writer.writeStartElement("Sequence");

    foreach (CTestAction *action, this->actions()) {

        writer.writeStartElement("Test");
        writer.writeAttribute("name", action->name().last());
        QString fullName = QString("%1|%2|%3").arg(action->name().first())
                .arg(action->name().at(1))
                .arg(action->name().last());
        writer.writeAttribute("fullName", fullName);
        writer.writeAttribute("type", QString::number(action->id()));
        if (action->isAutoTest())
            writer.writeAttribute("waite", "true");
        else
            writer.writeAttribute("waite", "false");
        if (action->isAutoBoard())
            writer.writeAttribute("autoplate", "true");
        else
            writer.writeAttribute("autoplate", "false");

        //softplate
        writer.writeStartElement("SoftPlate");

        writer.writeStartElement("Before");
        foreach (CSoftPlate* yb, action->softPlate()->beforList()) {
            writer.writeStartElement("Plate");
            writer.writeAttribute("desc", yb->desc());
            writer.writeAttribute("value", QString::number(yb->value()));
            writer.writeAttribute("ref", yb->reference());
            writer.writeEndElement();
        }
        writer.writeEndElement();//Before
        writer.writeStartElement("After");
        foreach (CSoftPlate* yb, action->softPlate()->afterList()) {
            writer.writeStartElement("Plate");
            writer.writeAttribute("desc", yb->desc());
            writer.writeAttribute("value", QString::number(yb->value()));
            writer.writeAttribute("ref", yb->reference());
            writer.writeEndElement();
        }
        writer.writeEndElement();//After

        writer.writeEndElement();//SoftPlate

        for (int i=0; i<4; i++) {
            writer.writeStartElement("State");
            writer.writeAttribute("num", QString::number(i+1));
            writer.writeAttribute("faultType", QString::number(action->smvData[i].faultType));

            //smv
            writer.writeStartElement("SMV");

            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "i");
            writer.writeAttribute("no", "1");
            writer.writeAttribute("value", QString::number(action->smvData[i].Ia[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Ia[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Ia[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn
            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "i");
            writer.writeAttribute("no", "2");
            writer.writeAttribute("value", QString::number(action->smvData[i].Ib[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Ib[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Ib[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn
            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "i");
            writer.writeAttribute("no", "3");
            writer.writeAttribute("value", QString::number(action->smvData[i].Ic[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Ic[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Ic[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn
            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "i");
            writer.writeAttribute("no", "4");
            writer.writeAttribute("value", QString::number(action->smvData[i].In[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].In[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].In[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn

            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "v");
            writer.writeAttribute("no", "1");
            writer.writeAttribute("value", QString::number(action->smvData[i].Va[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Va[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Va[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn
            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "v");
            writer.writeAttribute("no", "2");
            writer.writeAttribute("value", QString::number(action->smvData[i].Vb[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Vb[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Vb[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn
            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "v");
            writer.writeAttribute("no", "3");
            writer.writeAttribute("value", QString::number(action->smvData[i].Vc[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Vc[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Vc[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn
            writer.writeStartElement("Chn");
            writer.writeAttribute("class", "v");
            writer.writeAttribute("no", "4");
            writer.writeAttribute("value", QString::number(action->smvData[i].Vn[0]));
            writer.writeAttribute("phase", QString::number(action->smvData[i].Vn[1]));
            writer.writeAttribute("frequency", QString::number(action->smvData[i].Vn[2]));
            writer.writeAttribute("type", "AC");
            writer.writeEndElement();//Chn

            writer.writeEndElement();//SMV

            //goose ouput
            writer.writeStartElement("BinOut");
            CCircuitGroup *group = action->circuit_state(i);
            for (int k=0; k<4; k++) {
                CCircuit *circuit = group->circuit(k);
                if (circuit->isEnable()) {
                    writer.writeStartElement("Which");
                    writer.writeAttribute("no", QString::number(k+1));
                    writer.writeAttribute("value", "0");
                    writer.writeEndElement();//Which
                }
            }
            writer.writeEndElement();//BinOut

            //condition
            QString jumpNa = QString::number(i+2);
            if (i == 3) {
                jumpNa = "5";
            }
            writer.writeStartElement("Condition");
            if (action->states()->timeState(i)) {
                writer.writeStartElement("Time");
                writer.writeAttribute("value", QString::number(action->states()->timeValue(i)));
                writer.writeAttribute("jump", jumpNa);
                writer.writeEndElement();//Time
            }
            if (action->states()->actionState(i)) {
                writer.writeStartElement("Action");
                for (int m = 0; m<4; m++) {
                    QString value;
                    if (action->states()->actionPointState(i, m)) {
                        value = "true";
                    } else {
                        value = "false";
                    }
                    writer.writeAttribute(QString("bin%1").arg(m+1), value);
                }
                writer.writeAttribute("jump", jumpNa);
                writer.writeEndElement();//Action
            }
            writer.writeEndElement();//Condition


            writer.writeEndElement();//State
        }

        writer.writeEndElement();//Test
    }

    writer.writeEndElement();//Sequence

    writer.writeEndElement();//TestTemplate
    writer.writeEndDocument();
    file.close();
    return true;
}
