#include "mainwindow.h"
#include "spider_service.h"

#include <QApplication>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTextCodec>

#include "networker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug()<<QDir::currentPath();
    QFile file("C:\\Users\\hu\\Desktop\\liepin\\zhilian00.xml");
    //QFile file(QDir::currentPath()+"/"+"zhilian.xml");
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
        return 0;
    QTextStream txtInput(&file);
    txtInput.setCodec("UTF-8");
    QTextCodec *tc=QTextCodec::codecForName("utf-8");
    QString lineStr;
    QString xml= "";
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine()+"\n";
        xml +=lineStr;
    }
    file.close();
    QString utf8Xml = tc->toUnicode(xml.toUtf8());
    //qDebug()<<utf8Xml;
    qDebug()<<QDir::currentPath();
    Spider *spider = new Spider();
    QObject::connect(spider, SIGNAL(finish()),&a, SLOT(quit()));
    if(spider->initial(utf8Xml)){
        spider->start();
    }

    return a.exec();
}
