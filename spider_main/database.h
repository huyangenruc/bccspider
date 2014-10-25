#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QFile>
#include <QString>
#include <queue>
#include <QSqlError>

class DatabaseManager : public QObject {

public :
    const static QString connectionName;
    static DatabaseManager * instance;
    DatabaseManager();
public :
    static DatabaseManager *getInstance();
    ~DatabaseManager();

public :

    //create a sqlite database,name: dbName
    bool openDB(const QString &dbName);

    void closeDB(const QString &dbName);
    //delete a sqlite database
    bool deleteDB(const QString &dbName);
    bool clearDB(const QString *dbName);

    //create result table,table name :jobID,table schema: shcema
    bool createTable(QString jobId,const QList<QMap<QString,QString> > &schema);
    bool createTable(QString jobId, const QStringList &schema);

    // insert a new row to  table jobID
    bool insertXpathResult(QString jobId,const QMap<int,QStringList> &schema);

    // query
    QList<QStringList> queryXpathTable(QString jobId, const QList<QMap<QString, QString> *> *schema);

    //return last sqlite database error
    QSqlError lastError();

    //TRUNCATE TABLE
    bool truncateTable(const QString &tableName);

};


#endif // DATABASE_H
