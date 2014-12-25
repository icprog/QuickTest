#include <QLabel>
#include <QDebug>
#include <QBitmap>
#include "consolecontainer.h"

ConsoleContainer::ConsoleContainer(QWidget *parent)
    : QWidget(parent)
{
    m_startIndex = 0;
    initWidget();
}

ConsoleContainer::~ConsoleContainer()
{

}

void ConsoleContainer::initWidget()
{
    setFixedWidth(82);
    setMinimumHeight(400);

    QFrame *pBGFrame = new QFrame(this);
    pBGFrame->setObjectName("consoleBGFrame");
    pBGFrame->setGeometry(0,0,82,height());
    QVBoxLayout *pMainVLayout = new QVBoxLayout(this);
    pMainVLayout->setSpacing(0);
    pMainVLayout->setContentsMargins(0,0,0,0);
    pMainVLayout->addWidget(pBGFrame);

    m_pOriBtnGroup = new QButtonGroup(pBGFrame);

    QPushButton *pUnBtn = new QPushButton(pBGFrame);
    pUnBtn->setObjectName("upbtn");
    pUnBtn->setFixedSize(62,20);
    pUnBtn->setEnabled(false);
    QPixmap upPixmap(":/xqd/icon/up_mask.png");
    pUnBtn->setMask(upPixmap.mask());
    m_pOriBtnGroup->addButton(pUnBtn,up);

    QPushButton *pDownBtn = new QPushButton(pBGFrame);
    pDownBtn->setObjectName("downbtn");
    pDownBtn->setFixedSize(62,20);
    pDownBtn->setEnabled(false);
    QPixmap downPixmap(":/xqd/icon/down_mask.png");
    pDownBtn->setMask(downPixmap.mask());
    m_pOriBtnGroup->addButton(pDownBtn,down);
    connect(m_pOriBtnGroup, SIGNAL(buttonClicked(int)),this,SLOT(slotOriBtnClicked(int)));

    QWidget *pBtnPane = new QWidget(pBGFrame);
    pBtnPane->setObjectName("btnpane");
    m_pBtnLayout = new QVBoxLayout(pBtnPane);
    m_pBtnLayout->setSpacing(5);
    m_pBtnLayout->setContentsMargins(0,5,0,5);

    QVBoxLayout *pVLayout = new QVBoxLayout(pBGFrame);
    pVLayout->setSpacing(0);
    pVLayout->setContentsMargins(10,0,10,0);
    pVLayout->addWidget(pUnBtn);
    pVLayout->addWidget(pBtnPane);
    pVLayout->addWidget(pDownBtn);
}

void ConsoleContainer::addButton(QPushButton *pBtn, bool isLast)
{
    pBtn->setFixedSize(62,64);
    m_btnList.append(pBtn);

    if(pBtn->text().trimmed() == QObject::trUtf8("帮助"))
        pBtn->setObjectName("helpBtn");

    if(pBtn->text().trimmed().contains(QObject::trUtf8("开始")))
        pBtn->setObjectName("testBtn");

    if(m_btnList.count() > 6)
    {
        pBtn->hide();
        m_pOriBtnGroup->button(down)->setEnabled(true);
    }
    else
    {
        m_pBtnLayout->addWidget(pBtn);

        if(isLast)
            m_pBtnLayout->addStretch();
    }

}

void ConsoleContainer::addButtons(QList<QPushButton*> &btnList)
{
    for(int i=0; i<btnList.count(); i++)
        addButton(btnList.at(i));
}

void ConsoleContainer::updateOriBtnSate()
{
    int end = m_startIndex+6;
    int btnCount = m_btnList.count();

    if(0 == m_startIndex)
        m_pOriBtnGroup->button(up)->setEnabled(false);
    else
        m_pOriBtnGroup->button(up)->setEnabled(true);

    if(btnCount == end)
        m_pOriBtnGroup->button(down)->setEnabled(false);
    else
        m_pOriBtnGroup->button(down)->setEnabled(true);

}

void ConsoleContainer::slotOriBtnClicked(int index)
{
    if(index == up)
    {
        m_pBtnLayout->removeWidget(m_btnList.at(m_startIndex+5));
        m_btnList.at(m_startIndex+5)->hide();
        m_startIndex--;
        m_btnList.at(m_startIndex)->show();
        m_pBtnLayout->insertWidget(0,m_btnList.at(m_startIndex));
    }
    else
    {
        m_pBtnLayout->removeWidget(m_btnList.at(m_startIndex));
        m_btnList.at(m_startIndex)->hide();
        m_startIndex++;
        m_btnList.at(m_startIndex+5)->show();
        m_pBtnLayout->addWidget(m_btnList.at(m_startIndex+5));
    }

    updateOriBtnSate();
}
