QT += widgets network

CONFIG += c++17
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    connectiondialog.cpp

HEADERS += \
    mainwindow.h \
    connectiondialog.h

# Default rules for deployment.
qnx:                        target.path = /tmp/$${TARGET}/bin
else: unix:!android:        target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
