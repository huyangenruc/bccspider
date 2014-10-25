#include "global.h"

#include <iostream>

global::global(){

}

global::~global(){

}

bloom_filter* const global::bf = (new Filter(MAX_URL,FALSE_POSITIVE_PROBABILITY))->bf;

DatabaseManager* const global::sqlite = new  DatabaseManager();

std::priority_queue<Node, std::vector<Node>, NodeComparator>* const global::unVisitedUrl = new std::priority_queue<Node, std::vector<Node>, NodeComparator>();

QMap<int,Node>* const global::nodeMap = new QMap<int,Node>();

int global::maxDepth = 0;

QString global::taskID = "";

QString const global::KEY_SHARED_MEMORY = "Shared";

QString global::xml = "";

QString global::mainServer = "spider_main";

QString global::slaveServer = "spider_slave";
