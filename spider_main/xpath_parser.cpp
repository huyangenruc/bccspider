#include <QXmlItem>
#include <QXmlResultItems>
#include <QString>
#include <QStringList>
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#include <QXmlQuery>
#include <QXmlNodeModelIndex>
#include <QRegExp>
#include <QObject>
#include <QTextCodec>
#include <QXmlFormatter>
#include <QBuffer>
#include <QDebug>


#include "global.h"
#include "xpath_parser.h"

XpathUtil::XpathUtil(QWebPage *page){
    this->page = page;
    connect(page->mainFrame(),&QWebFrame::urlChanged,this,&XpathUtil::urlChanged);
}

XpathUtil::~XpathUtil(){

}

QStringList XpathUtil::xmlParser(const QString &xmlContent, const QString &xpath){
    QXmlQuery query ;
    QXmlResultItems  results;
    QStringList final;
    query.setFocus(xmlContent);
    query.setQuery(xpath);
    if(!query.isValid()){
        qDebug()<<QString("error xpath argments");
        return final;
    }
    query.evaluateTo(&results);
    QXmlItem item(results.next());
    while (!item.isNull()) {
        QString r;
        query.setFocus(item);
        query.setQuery("./text()");
        query.evaluateTo(&r);
        final << r.trimmed();
        item = results.next();
    }
    return final;
}



//parser xpath in a target url webpage
QStringList XpathUtil::parser(QWebPage *page,const QString &xpath){
    QStringList tmp;
    QString js =  "var results=document.evaluate('"+xpath+"', document, null,XPathResult.ANY_TYPE, null);"
            "var result=results.iterateNext();"
            "var message = '';"
            "while(result){"
            "message+=result.textContent;result=results.iterateNext();message+='@@@@';} "
            "message;";

    QString result = page->mainFrame()->evaluateJavaScript(js).toString().trimmed();

    if(result!=NULL){
        tmp = result.split("@@@@");
    }
    //delete empty element
    tmp.removeOne("");
    return tmp;

}

//&amp; == &
QString XpathUtil::parserXpath(QWebPage *page, const QString &xpath, const int type){
    QString js;
    if(type==1){
        js =  "var results=document.evaluate('"+xpath+"', document, null,XPathResult.ANY_TYPE, null);var result=results.iterateNext();var message = '';"
                "while(result){"
                "message+=result.textContent;result=results.iterateNext();} message;";
    }else if(type==2){
        js =  "var results=document.evaluate('"+xpath+"', document, null,XPathResult.ANY_TYPE, null);var result=results.iterateNext();var message = '';"
                "while(result){"
                "message+=result.innerHTML;result=results.iterateNext();} message;";

    }else if(type==3){
        js =  "var results=document.evaluate('"+xpath+"', document, null,XPathResult.ANY_TYPE, null);var result=results.iterateNext();var message = '';"
                "while(result){"
                "message+=result.outerHTML;result=results.iterateNext();message=message.replace(/&amp;/g,'&');} message;";

    }else if(type==4){
        js =  "var results=document.evaluate('"+xpath+"', document, null,XPathResult.ANY_TYPE, null);var result=results.iterateNext();var message = '';"
                "while(result){"
                "message+=result.getAttribute('href');result=results.iterateNext();message+='@@@@';} message;";
    }else if(type==5){
        js =  "var results=document.evaluate('"+xpath+"', document, null,XPathResult.ANY_TYPE, null);var result=results.iterateNext();var message = '';"
                "while(result){"
                "message+=result.getAttribute('src');result=results.iterateNext();message+='@@@@';} message;";
    }else{
        return false;
    }
    //qDebug()<<js;
    QString result = page->mainFrame()->evaluateJavaScript(js).toString().trimmed();
    //qDebug()<<result;
    return result;
}


QStringList XpathUtil::getUrl(QString html){
    QStringList url;
    QRegExp rx(QString("http://(.*)\""));
    rx.setMinimal(true);
    int pos=html.indexOf(rx);
    //qDebug()<<pos;
    while((pos = rx.indexIn(html, pos)) != -1){
        pos += rx.matchedLength();
        QString tmpUrl = rx.cap(0).trimmed();
        //qDebug()<<tmpUrl.mid(0,tmpUrl.length()-1);
        url.append(tmpUrl.mid(0,tmpUrl.length()-1));
    }
    return url;
}

QStringList XpathUtil::getUrlFromXpath(QWebPage *page,const QString xpath){
    QStringList urlist;
    QString script =
            "var r = document.evaluate('"+xpath+"', document, null, XPathResult.ANY_UNORDERED_NODE_TYPE, null);"
            "var node = r.singleNodeValue;"
            "var hrefs = document.evaluate('.//a', node, null,XPathResult.ANY_TYPE, null);var result=hrefs.iterateNext();"
            "var message = result.href;var myArray=new Array();"
            "while(result){myArray.push(result.href);result=hrefs.iterateNext();}myArray;" ;
    QVariant v = page->mainFrame()->evaluateJavaScript(script);
    //qDebug()<<script;
    //qDebug()<<v;
    QList<QVariant> tmp = v.toList();
    for(int i=0;i<tmp.size();i++){
        //qDebug()<<tmp.at(i).toString();
        urlist.append(tmp.at(i).toString());
    }
    return urlist;
}

//single node
void XpathUtil::clickNode(QWebPage *page, const QString &xpath){
    qDebug()<<"origin url:";
    qDebug()<<page->mainFrame()->url();
    QString script =
            "var r = document.evaluate('"+xpath+"', document, null, XPathResult.ANY_UNORDERED_NODE_TYPE, null);"
            "var node = r.singleNodeValue;"
            "var hrefs = document.evaluate('.//a', node, null,XPathResult.ANY_TYPE, null);var result=hrefs.iterateNext();"
            "var message = '';"
            "result.click();";
    //        "while(result){message+=result.href;result=hrefs.iterateNext();}message;" ;
    QString v = page->mainFrame()->evaluateJavaScript(script).toString();
}

//如果在新的taget ='_blank' 重置为'_self'   "if(node.target){node.target='_self';}"
QString XpathUtil::triggerNodeClick(QWebPage *page,const QString &xpath){
    //qDebug()<<page->mainFrame()->toHtml();
    QString script =
            "var bccNodeClick = function() { " \
            "var r = document.evaluate('" + xpath + "', document, null, XPathResult.ANY_UNORDERED_NODE_TYPE, null); " \
            "var node = r.singleNodeValue; " \
            "if(node) { " \
            "if(node.target){node.target='_self';}" \
            "node.click(); " \
            "}" \
            "}; " \
            "bccNodeClick();1;";
    qDebug()<<script;
    QString v = page->mainFrame()->evaluateJavaScript(script).toString();
    qDebug()<<v;
    return v;
}

void XpathUtil::urlChanged(const QUrl &url){
    //qDebug()<<"url changed,new url: "<<url.toString();
}

//从选定xpath区域中获取所有的a节点的数量
int XpathUtil::getANodeNumFromXpath(QWebPage *page,const QString &xpath){
    //qDebug()<<page->mainFrame()->toHtml();
    int aCount = 0;
    QString script =
            "var r = document.evaluate('"+xpath+"', document, null, XPathResult.ANY_UNORDERED_NODE_TYPE, null);"
            "var node = r.singleNodeValue;var count=0;"
            "var hrefs = document.evaluate('.//a', node, null,XPathResult.ANY_TYPE, null);var result=hrefs.iterateNext();"
            "while(result){result=hrefs.iterateNext();count++;}count;" ;
    //qDebug()<<script;
    QVariant v = page->mainFrame()->evaluateJavaScript(script);
    aCount = v.toString().toInt();
    //qDebug()<<v;
    //qDebug()<<aCount;
    return aCount;
}

//从选定xpath区域中点击第count个节点
void XpathUtil::clickNodeAtCount(QWebPage *page,const QString &xpath,int count){
    //qDebug()<< "click nodes :"<<page->mainFrame()->url();
    QString flag = QString::number(count);
    //forbid alert function
    QString script =
            "window.alert=function(){};"
            "var r = document.evaluate('"+xpath+"', document, null, XPathResult.ANY_UNORDERED_NODE_TYPE, null);"
            "var node = r.singleNodeValue;var count=0;var tmpflag="+flag+";var flag=parseInt(tmpflag);"
            "var hrefs = document.evaluate('.//a', node, null,XPathResult.ANY_TYPE, null);var result=hrefs.iterateNext();"
            "while(result){if(count==flag){if(result.target){result.target='_self';}result.click();break;} result=hrefs.iterateNext();count++;}count;" ;
    QVariant v = page->mainFrame()->evaluateJavaScript(script);
    qDebug()<<script;
    qDebug()<<v;
    return ;
}
