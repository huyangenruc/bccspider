#include "networker.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QNetworkReply>

NetWorker::NetWorker(QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this))
{
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    flag = false;
    getService = true;
}

NetWorker::~NetWorker()
{

}

NetWorker *NetWorker::instance()
{
    static NetWorker netWorker;
    return &netWorker;
}


QNetworkReply *NetWorker::post(const QUrl &url, const QByteArray &data)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    return manager->post(request, data);
}


QNetworkReply *NetWorker::get(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    return manager->get(request);
}


void NetWorker::requestFinished(QNetworkReply *reply)
{
    QByteArray ba = reply->readAll();
    QString result(ba);
    qDebug()<<result;
    if(getService){
        getService = false;
        QJsonDocument doc = QJsonDocument::fromJson(ba);
        QVariant replyData = doc.toVariant();
        QMap<QString,QVariant> map = replyData.toMap();
        QMap<QString,QVariant> map1 = map.value("hits").toMap();
        //qDebug()<<map1.value("total");
        if(map1.value("total")>=1){
            this->flag = true;
        }
        qDebug()<<this->flag;
    }
    reply->deleteLater();
}

