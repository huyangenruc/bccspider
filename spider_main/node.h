#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>



struct xpath {
    QString value;
    QString startWithFilter;
    QString endWithFilter;
    QString generalFilter;
    bool inclusive;
    xpath():inclusive(true){
    }
};

class Node {

public :
    QString url;
    QList<xpath> loopXpath;
    QList<xpath> regionXpath;
    QList<QMap<QString,QString> > collectData;
    int depth;
    Node(QString url, QList<xpath> loopXpath,QList<xpath> regionXpath,QList<QMap<QString,QString> > collectData,int depth);
    Node();
};


class NodeComparator{

public:
    bool operator()(const Node &node1, const Node &node2){
        if(node1.depth==node2.depth){
            return node1.url.length()<node2.url.length();
        }else{
             return node1.depth<node2.depth;
        }
    }

};



#endif // NODE_H
