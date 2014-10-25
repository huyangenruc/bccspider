#include "spider_service.h"
#include "node.h"
#include "global.h"
#include "logger.h"
#include "xpath_parser.h"
#include "webpage.h"
#include "networker.h"

#include <QWebView>
#include <QWebPage>
#include <QEventLoop>
#include <QTimer>
#include <QWebFrame>
#include <QDir>
#include <QJsonDocument>
#include <QApplication>

Spider::Spider(){
    page = new WebPage();
    page->flag = true;
    isRegionNode = false;
    connect(page, SIGNAL(addNewUrl(QUrl)), this, SLOT(addUrlToQueue(QUrl)));
    server = new QLocalServer();
    shared = new QSharedMemory(global::KEY_SHARED_MEMORY,0);
    instance = new NetWorker();
}

Spider::~Spider(){
    delete page;
}

void Spider::addUrlToQueue(const QUrl &url){
    //qDebug()<<"slots: "<<url.toString();
    if(!global::bf->contains(url.toString().toStdString())){
        //region url filter
        if(this->isRegionNode){

            Node newNode  = (*(global::nodeMap))[this->node.depth+1];
            if(this->currentSubNode.inclusive){
                if(url.toString().startsWith(this->currentSubNode.startWithFilter)){
                    newNode.url = url.toString();
                    if(newNode.url.startsWith("http://")){
                        global::unVisitedUrl->push(newNode);
                    }
                    qDebug()<<"From region Xpath: push url to queue :"<<url;
                }
            }
        }else{
            this->node.url = url.toString();
            if(this->node.url.startsWith("http://")){
                global::unVisitedUrl->push(this->node);
            }
            //global::unVisitedUrl->push(this->node);
            qDebug()<<"From loop xpath :push url to queue :"<<url;
        }
    }
}

bool Spider::initial(const QString &xml){
    global::xml = xml;
    XpathUtil *util = new XpathUtil(this->page);
    QString id;
    //提取id号
    QStringList ids = util->xmlParser(xml,"//id");
    if(ids.size()==1){
        id = ids.at(0);
        qDebug()<<"taskID :"<<id;
    }else if(ids.size()==0){
        Logger::initial("error","",QDir::currentPath());
        logInfo("\n"+xml);
        logError("id error");
        return false;
    }

    global::taskID = id;
    global::mainServer = global::mainServer+id;
    global::slaveServer = global::slaveServer+id;

    Logger::initial(global::taskID,"",QDir::currentPath());

    logInfo("\n"+xml);

    //提取所有node节点
    QStringList nodes = util->xmlParser(xml,"//node");
    for(int i=0;i<nodes.length();i++){
        QString urlXpath = "//node["+QString::number(i+1)+"]/url";
        QString depthXpath = "//node["+QString::number(i+1)+"]/depth";
        QString regionXpath = "//node["+QString::number(i+1)+"]/regionXpath/property_group";
        //QString regionXpath = "//node["+QString::number(i+1)+"]/regionXpath/property[@name='value']";
        //QString regionstartWithFilterXpath = "//node["+QString::number(i+1)+"]/regionXpath/property[@name='startWithFilter']";
        QString loopXpath = "//node["+QString::number(i+1)+"]/loopXpath/property[@name='value']";

        QString collectDataXpath = "//node["+QString::number(i+1)+"]/collectDataXpath/property_group";

        Node node;
        QStringList urlist = util->xmlParser(xml,urlXpath);
        QStringList depthlist = util->xmlParser(xml,depthXpath);
        //QStringList filterlist = util->xmlParser(xml,regionstartWithFilterXpath);
        QStringList collectDatalist = util->xmlParser(xml,collectDataXpath);
        QStringList loopXpathlist = util->xmlParser(xml,loopXpath);
        QStringList regionXpathlist = util->xmlParser(xml,regionXpath);

        if(urlist.size()==1){
            node.url = urlist.at(0).trimmed();
            qDebug()<<"url :"<<node.url;
        }
        if(depthlist.size()==1){
            node.depth = depthlist.at(0).trimmed().toInt();
            qDebug()<<"depth :"<<node.depth;
        }
        qDebug()<<"loopXpath------------";
        if(loopXpathlist.size()!=0){
            for(int j=0;j<loopXpathlist.size();j++){
                xpath subNode;
                subNode.value = loopXpathlist.at(j).trimmed();
                node.loopXpath.append(subNode);
                qDebug()<<"loopXpath :"<<loopXpathlist.at(j).trimmed();
            }
        }
        qDebug()<<"regionXpath------------";
        if(regionXpathlist.size()!=0){

            for(int j=0;j<regionXpathlist.size();j++){
                xpath subNode;
                QString startWithFilterXpath = regionXpath+"["+QString::number(j+1)+"]/property[@name='startWithFilter']";
                QString endWithFilterXpath = regionXpath+"["+QString::number(j+1)+"]/property[@name='endWithFilter']";
                QString generalFilterXpath = regionXpath+"["+QString::number(j+1)+"]/property[@name='generalFilter']";
                QString inclusiveXpath = regionXpath+"["+QString::number(j+1)+"]/property[@name='inclusive']";
                QString valueXpath = regionXpath+"["+QString::number(j+1)+"]/property[@name='value']";
                QStringList startWithFilter = util->xmlParser(xml,startWithFilterXpath);
                QStringList endWithFilter = util->xmlParser(xml,endWithFilterXpath);
                QStringList generalFilter = util->xmlParser(xml,generalFilterXpath);
                QStringList inclusive = util->xmlParser(xml,inclusiveXpath);
                QStringList value = util->xmlParser(xml,valueXpath);
                if(startWithFilter.size()!=0){
                    subNode.startWithFilter = startWithFilter.at(0).trimmed();
                }
                if(endWithFilter.size()!=0){
                    subNode.endWithFilter = endWithFilter.at(0).trimmed();
                }
                if(generalFilter.size()!=0){
                    subNode.generalFilter = generalFilter.at(0).trimmed();
                }
                if(inclusive.size()!=0){
                    if(inclusive.at(0).trimmed()=="true"){
                        subNode.inclusive = true;
                    }
                }
                if(value.size()!=0){
                    subNode.value = value.at(0).trimmed();
                }
                node.regionXpath.append(subNode);
                qDebug()<<"startWith Filter:"<<subNode.startWithFilter;
                qDebug()<<"endWith Filter:"<<subNode.endWithFilter;
                qDebug()<<"general Filter:"<<subNode.generalFilter;
                qDebug()<<"inclusive :"<<subNode.inclusive;
                qDebug()<<"value :"<<subNode.value;
            }
        }

        //提取采集数据节点
        qDebug()<<"collectData ---------";
        if(collectDatalist.size()!=0){
            for(int j=0;j<collectDatalist.size();j++){
                QString collectDataTypeXpath = collectDataXpath+"["+QString::number(j+1)+"]/property[@name='type']";
                QString collectDataNameXpath = collectDataXpath+"["+QString::number(j+1)+"]/property[@name='name']";
                QString collectDataXpathXpath = collectDataXpath+"["+QString::number(j+1)+"]/property[@name='xpath']";
                QString collectDataDescXpath = collectDataXpath+"["+QString::number(j+1)+"]/property[@name='desc']";
                QMap<QString,QString> tmp;
                tmp.insert("type",util->xmlParser(xml,collectDataTypeXpath).at(0).trimmed());
                tmp.insert("name",util->xmlParser(xml,collectDataNameXpath).at(0).trimmed());
                tmp.insert("xpath",util->xmlParser(xml,collectDataXpathXpath).at(0).trimmed());
                tmp.insert("desc",util->xmlParser(xml,collectDataDescXpath).at(0).trimmed());
                node.collectData.append(tmp);
                qDebug()<<"type : "<<util->xmlParser(xml,collectDataTypeXpath).at(0).trimmed();
                qDebug()<<"name : "<<util->xmlParser(xml,collectDataNameXpath).at(0).trimmed();
                qDebug()<<"xpath : "<<util->xmlParser(xml,collectDataXpathXpath).at(0).trimmed();
                qDebug()<<"desc : "<<(util->xmlParser(xml,collectDataDescXpath).at(0).trimmed());
            }
        }
        global::unVisitedUrl->push(node);
        global::nodeMap->insert(node.depth,node);
        //qDebug()<<util->xmlParser(xml,urlXpath);
        //qDebug()<<util->xmlParser(xml,depthXpath);
        //qDebug()<<util->xmlParser(xml,regionXpath);
        //qDebug()<<util->xmlParser(xml,loopXpath).size();
        //qDebug()<<util->xmlParser(xml,collectDataXpath);
        qDebug()<<"----------------------------------------------------------------";
    }
    global::maxDepth = global::unVisitedUrl->top().depth;
    global::sqlite->openDB(global::taskID);
    global::sqlite->createTable(global::taskID,global::unVisitedUrl->top().collectData);
    return true;
}


void Spider::start(){

    XpathUtil *util = new XpathUtil(page);
    int count = 0;
    global::unVisitedUrl->pop();
    global::unVisitedUrl->pop();
    while (!global::unVisitedUrl->empty())
    {
        Node node = global::unVisitedUrl->top();
        this->node = node;
        //判断url是否本地采集过
        if(global::bf->contains(node.url.toStdString())){
            qDebug()<< node.url<<" already crawled before !";
            global::unVisitedUrl->pop();
            continue;
        }
        count++;
        qDebug()<<count;
        //判断数据采集节点是否服务器已经采集过
        if(node.depth == global::maxDepth){
            instance->flag = false;
            instance->getService = true;
            if(this->get(instance,node.url)){
                qDebug()<<"elasticesearch indexed :" +node.url;
                logInfo(QString::number(count)+" elasticesearch indexed :"+node.url);
                global::bf->insert(node.url.toStdString());
                global::unVisitedUrl->pop();
                continue;
            }
        }
        logInfo(QString::number(count)+" load url :"+node.url);
        qDebug()<<"load url :"+node.url;
        page->flag = true;
        page->mainFrame()->load(QUrl(node.url));
        QEventLoop eventLoop;
        QObject::connect(page, SIGNAL(loadFinished(bool)), &eventLoop, SLOT(quit()));
        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer,SIGNAL(timeout()),&eventLoop,SLOT(quit()));
        //5s
        timer.start(5000);
        eventLoop.exec();       //block until finish
        if(timer.isActive()){
            timer.stop();
        }
        //采集数据节点
        int emptyCount = 0;
        if(node.depth == global::maxDepth){

            page->flag = true;
            QMap<int,QStringList> result;
            QMap<QString, QVariant> map;
            emptyCount=0;
            for(int j=0;j<node.collectData.size()-3;j++){
                QStringList tmpResult;
                QString nameFiled = node.collectData.at(j)["name"];
                QString resultFiled = util->parserXpath(page,node.collectData.at(j)["xpath"],node.collectData.at(j)["type"].toInt());
                //logInfo("[collect data ] nameFiled : "+nameFiled +" resultFiled :"+resultFiled);
                //logInfo("debug : "+node.collectData.at(j)["xpath"]+ " debug :"+node.collectData.at(j)["type"]);
                tmpResult.append(nameFiled);
                tmpResult.append(resultFiled);
                result.insert(j+1,tmpResult);
                map.insert(nameFiled,resultFiled);
                if(resultFiled.trimmed()==""){
                    emptyCount++;
                }
            }
            QStringList tmpResult;
            tmpResult.append("url");
            tmpResult.append(node.url);
            result.insert(node.collectData.size()-2,tmpResult);
            map.insert("url",node.url);
            QStringList tmpResult2;
            tmpResult2.append("来源");
            tmpResult2.append(global::taskID);
            result.insert(node.collectData.size()-1,tmpResult2);
            map.insert("来源",global::taskID);
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            QStringList tmpResult3;
            tmpResult3.append("采集时间");
            tmpResult3.append(current_date);
            map.insert("采集时间",current_date);
            result.insert(node.collectData.size(),tmpResult3);
            if(emptyCount<7){
                global::sqlite->insertXpathResult(global::taskID,result);
                this->post(map,instance);

            }

        }else{
            //非采集节点
            page->flag = false;

            //qDebug()<<page->mainFrame()->toHtml();

            for(int j=0;j<node.regionXpath.size();j++){
                qDebug()<<"region xpath";
                this->node = (*(global::nodeMap))[node.depth];
                this->isRegionNode = true;
                this->currentSubNode = node.regionXpath.at(j);

                qDebug()<<"--------------------------";
                qDebug()<<"REGION :GET URL FROM XPATH";

                //QStringList regionUrlResult = util->getUrlFromXpath(page,node.regionXpath.at(j).value);
                QString html = util->parserXpath(page,node.regionXpath.at(j).value,2);
                qDebug()<<"------------------html";
                int time=1;
                while(html==""){
                    QTime t;
                    t.start();
                    while(t.elapsed()<time*1000){
                        QCoreApplication::processEvents();
                    }
                    time++;
                    html = util->parserXpath(page,node.regionXpath.at(j).value,2);
                    if(time>=5){
                        break;
                    }
                }
                qDebug()<<"time***********";
                qDebug()<<time;
                QStringList regionUrlResult = util->getUrl(html);

//                for(int i=0;i<regionUrlResult.length();i++){
//                    qDebug()<< "GET URL FROM XPATH";
//                    qDebug()<< regionUrlResult.at(i);
//                    addUrlToQueue(regionUrlResult.at(i));
//                }
                int count = util->getANodeNumFromXpath(page,node.regionXpath.at(j).value);
                qDebug()<<"**********************";
                qDebug()<<count;
                qDebug()<<"**********************";
                if(count==0){
                    count = util->getANodeNumFromXpath(page,node.regionXpath.at(j).value);
                }
                for(int m=0;m<count;m++){
                    util->clickNodeAtCount(page,node.regionXpath.at(j).value,m);
                }
            }

            for(int j=0;j<node.loopXpath.size();j++){
                qDebug()<<"loop xpath";
                this->node = (*(global::nodeMap))[node.depth];
                this->isRegionNode = false;
                qDebug()<<"--------------------------";
                qDebug()<<"LOOP :GET URL FROM XPATH";
                QStringList loopUrlResult = util->getUrlFromXpath(page,node.loopXpath.at(j).value);
                for(int i=0;i<loopUrlResult.length();i++){
                    qDebug()<< "GET URL FROM XPATH";
                    qDebug()<< loopUrlResult.at(i);
                    addUrlToQueue(loopUrlResult.at(i));
                }

                int count = util->getANodeNumFromXpath(page,node.loopXpath.at(j).value);

                for(int m=0;m<count;m++){
                    util->clickNodeAtCount(page,node.loopXpath.at(j).value,m);
                }
            }
        }
        global::bf->insert(node.url.toStdString());
        global::unVisitedUrl->pop();
    }
    logInfo("spider finished");
    emit finish();
    qDebug()<<"spider finished";
}


void Spider::post(const QMap<QString, QVariant> &map,NetWorker *instance){
    QString address1 = "http://192.168.60.131:9200/bcc/1";
    QString address2 = "http://192.168.60.132:9200/bcc/1";
    QString address3 = "http://192.168.60.133:9200/bcc/1";
    QString address4 = "http://192.168.60.134:9200/bcc/1";
    QJsonDocument doc=QJsonDocument::fromVariant(QVariant(map));
    QByteArray j=doc.toJson();
    QString result(j);
    qDebug()<<result;
    int count = rand();
    if(count%4==0){
        instance->post(QUrl(address1),j);
    }else if(count%4==1){
        instance->post(QUrl(address2),j);
    }else if(count%4==2){
        instance->post(QUrl(address3),j);
    }else {
        instance->post(QUrl(address4),j);
    }
    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(instance, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QObject::connect(&timer,SIGNAL(timeout()),&eventLoop,SLOT(quit()));
    timer.start(2000);
    eventLoop.exec();       //block until finish
    if(timer.isActive()){
        timer.stop();
    }
}

bool Spider::get(NetWorker *instance,QString url){
    QString address1 = "http://192.168.60.131:9200/bcc/_search?q=url:\"";
    QString address2 = "http://192.168.60.132:9200/bcc/_search?q=url:\"";
    QString address3 = "http://192.168.60.133:9200/bcc/_search?q=url:\"";
    QString address4 = "http://192.168.60.134:9200/bcc/_search?q=url:\"";
    //QString url = "http://192.168.60.134:9200/bcc/_search?q=url:\"http://www.lagou.co/jobs/218581.html?source=search\"";
    int count = rand();
    qDebug()<<address1+url+"\"";
    if(count%4==0){
        instance->get(address1+url+"\"");
    }else if(count%4==1){
        instance->get(address2+url+"\"");
    }else if(count%4==2){
        instance->get(address3+url+"\"");
    }else {
        instance->get(address4+url+"\"");
    };
    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(instance, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QObject::connect(&timer,SIGNAL(timeout()),&eventLoop,SLOT(quit()));
    timer.start(3000);
    eventLoop.exec();       //block until finish
    if(timer.isActive()){
        timer.stop();
        //return false;
    }
    return instance->flag;
}




