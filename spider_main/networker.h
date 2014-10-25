#ifndef NETWORKER_H
#define NETWORKER_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QString>
#include <QNetworkAccessManager>

class QNetworkReply;
class QNetworkAccessManager;

class NetWorker : public QObject
{
    Q_OBJECT
public:
    explicit NetWorker(QObject *parent = 0);
    static NetWorker *instance();
    virtual ~NetWorker();
    QNetworkReply *post(const QUrl &url, const QByteArray &data);
    QNetworkReply *get(const QUrl &url);
    bool flag;
    bool getService;
public slots:
    void requestFinished(QNetworkReply *reply);

signals:
    void finished(QNetworkReply *);
private:
    QNetworkAccessManager *manager;

};



#endif // NETWORK_H
