#ifndef CONSOLECONTAINER_H
#define CONSOLECONTAINER_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>

class ConsoleContainer : public QWidget
{
    Q_OBJECT

public:
    ConsoleContainer(QWidget *parent = 0);
    ~ConsoleContainer();

    void addButton(QPushButton *pBtn, bool isLast = false);
    void addButtons(QList<QPushButton*> &btnList);

private:
    void initWidget();
    void updateOriBtnSate();

private slots:
    void slotOriBtnClicked(int);

private:
    enum orientation{up,down};

    int m_startIndex;
    QVBoxLayout     *m_pBtnLayout;
    QButtonGroup    *m_pOriBtnGroup;
    QList<QPushButton*> m_btnList;
};

#endif // CONSOLECONTAINER_H
