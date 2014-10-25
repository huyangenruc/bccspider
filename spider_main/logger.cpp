#include <iostream>
#include <fstream>
#include <sstream>

#include <QCoreApplication>
#include <QDir>
#include <QTime>
#include <QDebug>

#include "logger.h"
#include "global.h"
//#include "fileutil.h"

Logger *Logger::logger_ = NULL;

Logger::Logger()
    : id_("0")
{
}

void Logger::initial(QString id, QString tip,QString path)
{
    logger_ = new Logger();
    logger_->id_ = id;
    logger_->tip_ = tip;
    QString outPath = path;
    logger_->outputPath_ = outPath + "/" + id + ".log.txt";
//    FileUtil::deleteFile(logger_->outputPath_);
}

Logger *Logger::getLogger()
{
    if (logger_ == NULL) {
        logError("Logger has not been initialed");
    }
    return logger_;
}

void Logger::printLog(const char *content, const LogType logType) {
    if (logType >= Logger::DEBUG_LOG) {
        std::ofstream out;

//        QString outPath = QString(global::OUTPUT_PATH);
//        outPath = outPath + "/" + id_ + ".log.txt";
        out.open(QDir::toNativeSeparators(outputPath_).toStdString().c_str(), std::ofstream::out | std::ofstream::app);
        std::string msg("");
        switch (logType) {
        case Logger::DEBUG_LOG:
            msg = "[DEBUG]";
            break;
        case Logger::INFO_LOG:
            msg = "[INFO]";
            break;
        case Logger::WARN_LOG:
            msg = "[WARN]";
            break;
        case Logger::ERROR_LOG:
            msg = "[ERROR]";
            break;
        default:
            break;
        }
        if (!tip_.isEmpty()) {
            msg = msg.append("[").append(tip_.toStdString()).append("]");
        }
        QTime time;
        out << time.currentTime().toString("hh:mm:ss:zzz").toStdString()
            << " : "
            << msg
            << " "
            << content
            << std::endl;
        out.close();
    }
    if (logType == Logger::ERROR_LOG) {
        QCoreApplication::exit();
    }
}

void Logger::printLog(const std::string& content, const LogType logType) {
    printLog(content.c_str(), logType);
}

void Logger::printLog(const QString& content, const LogType logType) {
    printLog(content.toStdString(), logType);
}

void Logger::printLog(const int content, const LogType logType) {
    std::stringstream ss;
    ss << content;
    printLog(ss.str(), logType);
}

