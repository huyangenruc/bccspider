#include "node.h"

Node::Node(QString url, QList<xpath> loopXpath,QList<xpath> regionXpath,QList<QMap<QString,QString> > collectData,int depth) :
    url(url),loopXpath(loopXpath),regionXpath(regionXpath),collectData(collectData),depth(-1){
    this->depth = depth;

}


Node::Node() :
    url(),loopXpath(),regionXpath(),collectData(),depth(-1){

}


