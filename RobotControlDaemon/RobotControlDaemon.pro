TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    MessageParser.cpp \
    ControlLogic.cpp \
    base16message.cpp \
    pipemessager.cpp \
    fcgiServer/fcgiserver.cpp

HEADERS += \
    MessageParser.h \
    ControlLogic.h \
    ../CommandList.h \
    base16message.h \
    pipemessager.h \
    fcgiServer/fcgiserver.h \
    fcgiServer/fcgiserviceif.h

