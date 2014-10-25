#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLocalServer *server;
    QLocalSocket *socket;
    QLocalSocket *slaveSocket;
    QProcess *slave;
    int count;
public slots:
    void newConnection();
    void readyRead();
    void slaveProcess();
public:
    void init();
    void quit();
signals:
    void startSlaveProcess();
    void stop();
};

#endif // MAINWINDOW_H
