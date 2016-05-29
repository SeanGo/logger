TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
    client.c \
    mylog.c \
    parserthread.c \
    server.c \
    types.c \
    utils.c

HEADERS += \
    client.h \
    mylog.h \
    parserthread.h \
    types.h \
    utils.h

