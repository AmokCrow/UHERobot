TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    MessageParser.cpp \
    ControlLogic.cpp \
    base16message.cpp

HEADERS += \
    MessageParser.h \
    ControlLogic.h \
    ../CommandList.h \
    base16message.h

