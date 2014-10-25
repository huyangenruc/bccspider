#-------------------------------------------------
#
# Project created by QtCreator 2014-07-17T10:55:26
#
#-------------------------------------------------

QT       += core gui sql network widgets webkitwidgets xmlpatterns
QT       -= core


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = spider_main
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    global.cpp \
    database.cpp \
    xpath_parser.cpp \
    node.cpp \
    logger.cpp \
    spider_service.cpp \
    webpage.cpp \
    networker.cpp


HEADERS  += mainwindow.h \
    global.h \
    logger.h \
    database.h \
    filter.h \
    bloom_filter.h \
    xpath_parser.h \
    spider_service.h \
    node.h \
    webpage.h \
    xpath_parser.h \
    networker.h

FORMS    += mainwindow.ui
