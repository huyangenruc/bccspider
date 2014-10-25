#include "database.h"
#include <QDebug>
#include <iostream>
#include <QVariant>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include "global.h"


DatabaseManager::DatabaseManager(){}

DatabaseManager::~DatabaseManager(){}

const QString DatabaseManager::connectionName = "xpathconnection";
DatabaseManager * DatabaseManager::instance
            = new DatabaseManager;

DatabaseManager *DatabaseManager::getInstance()
{
    return instance;
}

//create a sqlite database
bool DatabaseManager::openDB(const QString &dbName){
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (db.isOpen())
        return true;

    if (!db.isValid()){
        // Find QSLite driver
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        QString dbFilePath;
#ifdef Q_OS_WIN32
//        dbFilePath = global::DB_FILE_NAME;
        dbFilePath = dbName + ".db";
#else
        // NOTE: store database file into user home folder in Linux
        dbFilePath = QDir::home().path();
//        QString path(global::OUTPUT_PATH);
        dbFilePath.append(QDir::separator()).append(dbName).append(".db");
        dbFilePath = QDir::toNativeSeparators(dbFilePath);
#endif
        db.setDatabaseName(dbFilePath);
    }

    // Open databasee
    return db.open();
}

void DatabaseManager::closeDB(const QString &dbName)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (db.isOpen()) {
        db.close();
    }
}

//deltete sqlite database
bool DatabaseManager::deleteDB(const QString &dbName){
    // Close database
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (db.isOpen())
        db.close();
#ifdef Q_OS_WIN32
    QString dbFilePath = global::taskID+".db";
    bool success = false;
    if (QFile::exists(dbFilePath)) {
        success = QFile::remove(dbFilePath);
        return success;
    } else {
        return true;
    }
#else
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append(dbName);
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#endif
}

bool DatabaseManager::clearDB(const QString *dbName)
{
    QSqlDatabase dataBase = QSqlDatabase::addDatabase("QSQLITE");
    dataBase.setDatabaseName("newXpath-zhong.db");
    qDebug() << dataBase.open();
    QStringList tableList  = dataBase.tables();
    qDebug() << tableList;
    QSqlQuery query;
    for (int i = 0; i < tableList.size(); ++i)
    {
        qDebug() << "DROP TABLE \"" + tableList.at(i) + "\"";
        qDebug() << query.exec("DROP TABLE \"" + tableList.at(i) + "\"");
    }
    dataBase.close();
    return true;
}

//create result table,table name :jobID,table schema: shcema
bool DatabaseManager::createTable(QString jobId,const QList<QMap<QString,QString> > &schema){
//    bool flag = false;
    QList<QMap<QString, QString> >::const_iterator it = schema.cbegin();
    QStringList params;
    for (; it != schema.cend(); it++) {
        params.append((*it).value(QString("name")));
    }
    return createTable(jobId, params);
}

//create result table,table name :jobID,table schema: shcema
bool DatabaseManager::createTable(QString jobId,const QStringList &schema){
    bool flag = false;

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    QString tableName = "\""+jobId+"\"";
    QString tmpSql = "";
    if(schema.size()!=0){
        for(int i = 0;i<schema.size();i++){
            tmpSql += ","+ schema.at(i)+" varchar(255)";
        }
        QString sql = "create table "+tableName+" (id integer primary key"+tmpSql+")";
        if(db.isOpen()){
            flag = query.exec(sql);
        }
        if(flag==false){
            qDebug()<<sql;
            qDebug()<<query.lastError();
        }
    }
    return flag;
}

// insert a new row to  table xpathResult
bool DatabaseManager::insertXpathResult(QString jobId,const QMap<int,QStringList> &schema){
    bool rel =false;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    QString tableName = "\""+jobId+"\"";

    QString tmpSql_1 = "";
    QString tmpSql_2 = "";

    QMapIterator<int, QStringList> i(schema);
    while (i.hasNext()) {
        i.next();
        tmpSql_1 += ","+i.value().at(0)+"";
        tmpSql_2 += ", ?";
       // qDebug() << i.key() << ": " << i.value() ;
    }
    QString sql = "INSERT INTO "+tableName+" (id"+tmpSql_1+") VALUES (?"+tmpSql_2+")";

    if(db.isOpen()){
         query.prepare(sql);
         QMapIterator<int, QStringList> j(schema);
         while (j.hasNext()) {
             j.next();
             query.bindValue(j.key(), j.value().at(1));

         }
         rel = query.exec();
    }
    if(rel==false){

         qDebug()<<query.lastError();
    }
    return rel;
}


//query
QList<QStringList> DatabaseManager::queryXpathTable(QString jobId,const QList<QMap<QString,QString> *> *schema){
    bool ret = false;
    QString tableName = "\""+jobId+"\"";
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    ret = query.exec(QString("select * from "+tableName+" "));
    int length = schema->size();
    QList<QStringList > result;
    while (query.next()){
        QStringList current;
        for(int i=0;i<=length;i++){
             current.append(query.value(i).toString());
        }
        result.append(current);
    }
    return result;
}

QSqlError DatabaseManager::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    return db.lastError();
}

//TRUNCATE TABLE ,here tableName is missionid,
bool DatabaseManager::truncateTable(const QString &tableName){
    bool flag = false;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    QString tmpTableName = "\""+tableName+"\"";
    if(db.isOpen()){
         flag = query.exec(QString("delete from "+tmpTableName));
    }

    if(!flag){
        qDebug()<<query.lastError();
    }
    return flag;

}
