#ifndef MAPPINGLINKWIDGET_H
#define MAPPINGLINKWIDGET_H

#include "mappinglinkwidget_global.h"

#include <QWidget>

namespace Ui {
class MappingLinkWidget;
}

class MAPPINGLINKWIDGETSHARED_EXPORT MappingLinkWidget : public QWidget
{
    Q_OBJECT

signals:
    void quit();

public:
    explicit MappingLinkWidget(QWidget *parent = 0);
    ~MappingLinkWidget();

private slots:
    void iedChanged();

    void backToTop();
private:
    Ui::MappingLinkWidget *ui;
    int map[2];
};

#endif // MAPPINGLINKWIDGET_H
