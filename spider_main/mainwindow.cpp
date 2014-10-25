#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "logger.h"

#include <QString>
#include <QProcess>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new QLocalServer(this);
    slaveSocket = new QLocalSocket(this);
    slave = new QProcess();
    count =1;
    connect(this,SIGNAL(startSlaveProcess()),this,SLOT(slaveProcess()));
}

void MainWindow::quit(){
    server->close();
    this->close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slaveProcess(){
    //QProcess *slave = new QProcess();
    QStringList arguments ;
    arguments.append(global::xml);
    QString slavePath = QDir::currentPath()+"/spider_slave.exe";
    //slave->start(slavePath,arguments);
    qDebug()<<"start slave process";
    slave->startDetached(slavePath,arguments);
}


void MainWindow::newConnection(){
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void MainWindow::init(){
    QLocalServer::removeServer(global::mainServer);
    server->listen(global::mainServer);
    connect(server,SIGNAL(newConnection()),this,SLOT(newConnection()));
    emit startSlaveProcess();
}

void MainWindow::readyRead(){
    QLocalSocket *local = static_cast<QLocalSocket *>(sender());
    if(!local){
        return;
    }
    QTextStream in(local);
    QString     readMsg;
    readMsg = in.readAll();
    qDebug()<<readMsg;

    if(readMsg=="slaveIsReady"){

        if(!global::unVisitedUrl->empty()){

            QString url = global::unVisitedUrl->top().url;

            logInfo(QString::number(this->count++));
            
            while(global::bf->contains(url.toStdString())){
                global::unVisitedUrl->pop();
                url = global::unVisitedUrl->top().url;
            }

            slaveSocket->close();
            slaveSocket->connectToServer(global::slaveServer);

            if(slaveSocket->waitForConnected()){

                QTextStream ts(slaveSocket);
                QString message = "node\n"+url+"\n"+QString::number(global::unVisitedUrl->top().depth);
                ts<<message;
                ts.flush();
                slaveSocket->waitForBytesWritten();

            }

            global::bf->insert(url.toStdString());
            global::unVisitedUrl->pop();
        }else{
            qDebug()<<"main process finished;";
            slaveSocket->close();
            slaveSocket->connectToServer(global::slaveServer);
            if(slaveSocket->waitForConnected()){
                QTextStream ts(slaveSocket);
                QString message = "finished";
                ts<<message;
                ts.flush();
                slaveSocket->waitForBytesWritten();
            }
            emit stop();
        }
    //slave提取的url加入到url队列中
    }else if(readMsg.startsWith("newUrlFromSlave")){
        QStringList message = readMsg.split("\n");
        if(message.size()==3){
            QString url = message.at(1);
            int depth = readMsg.split("\n").at(2).toInt();
            Node node = (*(global::nodeMap))[depth];
            node.url = url;
            if(!global::bf->contains(node.url.toStdString())){
                global::unVisitedUrl->push(node);
            }
        }
        qDebug()<<readMsg;

    }else if(readMsg=="slaveIsFull"){
        emit stop();
    }

}
