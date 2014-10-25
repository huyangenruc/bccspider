#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>

class QWebFrame;
class QNetworkRequest;
class NavigationType;
class WebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit WebPage(QWidget *parent = 0);
    void setStopEmitSignal(bool stopEmitSignal);
    void setMode(bool configMode = false);
signals:
    void delegateSignal(const QNetworkRequest &request);
    void delegeteLink(const QNetworkRequest &request);
    void delegateOther(const QNetworkRequest &request);
    void addNewUrl(const QUrl &url);

public slots:

protected:
    bool acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, NavigationType type);

    void javaScriptAlert(QWebFrame * frame, const QString & msg);

    void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);

private:
    bool stopEmitSignal;
    bool m_configMode;

public:
    bool flag;


};

#endif // WEBPAGE_H
