TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lfcgi -lpthread

SOURCES += main.cpp \
    MessageParser.cpp \
    ControlLogic.cpp \
    base16message.cpp \
    fcgiServer/fcgiserver.cpp \
    debuglog.cpp \
    robotboard2interpreter.cpp

HEADERS += \
    MessageParser.h \
    ControlLogic.h \
    ../CommandList.h \
    base16message.h \
    fcgiServer/fcgiserver.h \
    fcgiServer/fcgiserviceif.h \
    debuglog.h \
    DataExchangeStructures.h \
    robotboard2interpreter.h

