#ifndef XPATH_PARSER_H
#define XPATH_PARSER_H

#include <QObject>
#include <QWebPage>


class XpathUtil : public QObject{

public :
    XpathUtil(QWebPage *page);

    ~XpathUtil();

    QWebPage *page;

    //xml xpath parser
    QStringList xmlParser(const QString &xmlContent,const QString &xpath);



    //parser xpath in a target url webpage
    QStringList parser(QWebPage *page, const QString &xpath);

    //parser xpath in a target url webpage and push it to database,type: 1(textContent),2(innerHTML),3(outerHTML),4(a.href),5(img.src)
    QString parserXpath(QWebPage *page, const QString &xpath, const int type);




    QStringList getUrl(QString html);

    QString triggerNodeClick(QWebPage *page,const QString &xpath);
    void clickNode(QWebPage *page,const QString &xpath);
    QStringList getUrlFromXpath(QWebPage *page, const QString xpath);


    int getANodeNumFromXpath(QWebPage *page,const QString &xpath);

    void clickNodeAtCount(QWebPage *page,const QString &xpath,int count);

public slots :

    void urlChanged(const QUrl & url);

};


#endif // XPATH_PARSER_H
