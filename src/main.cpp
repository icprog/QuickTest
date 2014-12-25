#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <QStyle>
#include <QFile>
#include <QDebug>
#include "quicktestcore.h"
#include <QUdpSocket>
QuickTestCore core;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTextCodec *locale_codec = QTextCodec::codecForLocale();
//    if (!locale_codec)
//    {
//        return 1;
//    }

    //设置字符节
    QTextCodec::setCodecForLocale (QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForCStrings (QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr ( QTextCodec::codecForName("UTF-8") );
//#endif

    QFile file(":/xqd/qss/global.qss");
    if (file.open(QFile::ReadOnly)) {
        a.setStyleSheet(file.readAll().data());
        file.close();
    } else {
        QMessageBox::warning(0, "警告", QObject::tr("界面元素读取失败"), QMessageBox::Ok);
    }

    QString strErr;


    if (!core.initialize(&strErr)) {
        QMessageBox::warning(0, "错误", strErr, QMessageBox::Ok);
        return -1;
    }

    MainWindow w;


#if defined (Q_OS_WIN32)
//   w.resize(641, 400);
//    w.setWindowFlags( Qt::WindowTitleHint );
#elif defined ( Q_OS_LINUX )
    w.setFixedSize(640,440);
    w.setWindowFlags( Qt::FramelessWindowHint );
#endif

    QUdpSocket *sender = new QUdpSocket();
    QByteArray data = QByteArray::fromHex("a6a6a6a60401");//打开所有光口
    sender->writeDatagram(data, HostAddress, HostPort);

    delete sender;
    sender = NULL;

    w.move(0, 40);
    w.show();

    return a.exec();
}
