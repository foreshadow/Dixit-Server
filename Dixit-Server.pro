#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T15:19:30
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = Dixit-Server
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tcpserver.cpp \
    tcpsocket.cpp \
    server.cpp \
    player.cpp \
    playerlist.cpp \
    clientdata.cpp \
    serverdata.cpp \
    deck.cpp \
    timeline.cpp \
    declarer.cpp

HEADERS += \
    tcpserver.h \
    tcpsocket.h \
    server.h \
    player.h \
    playerlist.h \
    clientdata.h \
    serverdata.h \
    deck.h \
    timeline.h \
    declarer.h
