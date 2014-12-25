#ifndef IMPORTDLG_H
#define IMPORTDLG_H

#include <QDialog>

class QFileSystemModel;
class QModelIndex;
namespace Ui {
class importDlg;
}

class importDlg : public QDialog
{
    Q_OBJECT

public:
    explicit importDlg(const QString &title, const QString &dir, const QStringList &nameFilters = QStringList() << "*.*", QWidget *parent = 0);
    ~importDlg();

    QString filePath() const { return exportFilePath; }

private slots:

    void accepted();

private:
    Ui::importDlg *ui;
    QString exportFilePath;


    QFileSystemModel *model;

};

#endif // IMPORTDLG_H
