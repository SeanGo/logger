TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../loggerd

LIBS += -lgtest -lgtest_main -lpthread
SOURCES += \
    Tester.cpp \
    ../loggerd/mylog.c \
    ../loggerd/parserthread.c \
    ../loggerd/types.c \
    ../loggerd/utils.c

HEADERS += \
    ../loggerd/mylog.h \
    ../loggerd/parserthread.h \
    ../loggerd/types.h \
    ../loggerd/utils.h

