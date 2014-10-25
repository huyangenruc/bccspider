/* This class contains all global variables */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <queue>
#include <vector>
#include <queue>
#include <iostream>

#include "database.h"
#include "filter.h"
#include "node.h"

#define  MAX_URL 10000000
#define FALSE_POSITIVE_PROBABILITY  0.0001


class global : QObject {
    Q_OBJECT

public:
     //database
     static  DatabaseManager * const sqlite;

     //unVisited url list
     static  std::priority_queue<Node, std::vector<Node>, NodeComparator>* const unVisitedUrl;

     //bloom filter ,contain visited url list
     static bloom_filter* const bf;

     static QMap<int,Node>* const nodeMap;

     static int maxDepth;

     static QString taskID;

     static const QString KEY_SHARED_MEMORY;

     static QString xml;

     static QString mainServer;

     static QString slaveServer;

     //static QList<QMap<QString,QString>> schema;

public :
     global();

    ~global();

};



#endif // GLOBAL_H
