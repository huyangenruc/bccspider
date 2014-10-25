#include "webpage.h"
#include <QNetworkRequest>
#include <QDebug>
#include <QWebFrame>

WebPage::WebPage(QWidget *parent) :
    QWebPage(parent),
    stopEmitSignal(false),
    m_configMode(false),
    flag(false)
{
}

void WebPage::setMode(bool configMode){
    this->m_configMode = configMode;
}

bool WebPage::acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, NavigationType type)
{
    //qDebug()<<"NavigationType :"<<type;
    //qDebug()<<"[acceptNavigationRequest] url : "<<request.url();

    if(!flag){
        emit addNewUrl(request.url());
        qDebug()<< "emit singal:"<<request.url();
    }
    return flag;
}

void WebPage::javaScriptAlert(QWebFrame *frame, const QString &msg){
    return;
}

void WebPage::javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID){
//    qDebug()<< "***********************";
//    qDebug()<<lineNumber;
//    qDebug()<<sourceID;
//    qDebug()<<message;
//    qDebug()<< "***********************";
    return;
}

void WebPage::setStopEmitSignal(bool stopEmitSignal)
{
    this->stopEmitSignal = stopEmitSignal;
}
