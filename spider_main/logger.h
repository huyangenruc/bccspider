#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <map>

#include <QString>


class Logger
{
private:
    static Logger *logger_;
    Logger();
    QString id_;
    QString tip_;
    QString outputPath_;
public:
    enum LogType {
        DEBUG_LOG = 1,
        INFO_LOG  = 2,
        WARN_LOG  = 3,
        ERROR_LOG = 4,
        NO_LOG    = 5
    };

    inline QString getId() { return id_; }
    inline const QString getOutputPath() const { return outputPath_; }
    static void initial(QString id_, QString tip, QString path);
    static Logger *getLogger();
    void printLog(const char *content, const LogType logType);
    void printLog(const std::string& content, const LogType logType);
    void printLog(const QString& content, const LogType logType);
    void printLog(const int content, const LogType logType);
};

inline void logDebug(const QString& content) {
    Logger::getLogger()->printLog(content, Logger::DEBUG_LOG);
}
inline void logInfo(const QString& content) {
    Logger::getLogger()->printLog(content, Logger::INFO_LOG);
}
inline void logWarn(const QString& content) {
    Logger::getLogger()->printLog(content, Logger::WARN_LOG);
}
inline void logError(const QString& content) {
    Logger::getLogger()->printLog(content, Logger::ERROR_LOG);
}

#endif // LOGGER_H
