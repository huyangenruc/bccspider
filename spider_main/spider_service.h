#ifndef SPIDER_SERVICE_H
#define SPIDER_SERVICE_H
#include <QString>
#include <QObject>
#include <QWebPage>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedMemory>

#include "node.h"
#include "webpage.h"
#include "networker.h"

class Spider : public QObject
{
    Q_OBJECT
public :
    Spider();
    ~Spider();
    bool initial(const QString &xml);
    void start();
    void stop();
    void post(const QMap<QString, QVariant> &map, NetWorker *instance);
    bool get(NetWorker *instance, QString url);
public :
    WebPage *page;
    Node node;
    bool isRegionNode;
    xpath currentSubNode;
private :
    QLocalServer *server;
    QSharedMemory *shared;
    NetWorker *instance;
public slots :
    void addUrlToQueue(const QUrl &url);
signals:
    void finish();

};



#endif // SPIDER_SERVICE_H
