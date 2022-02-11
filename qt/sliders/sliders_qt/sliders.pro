TEMPLATE = app
TARGET = name_of_app

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    window.cpp \
    slidersgroup.cpp \
    main.cpp

HEADERS += \
    slidersgroup.h \
    window.h
