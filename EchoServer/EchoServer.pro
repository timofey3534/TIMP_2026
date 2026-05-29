QT += network sql gui

CONFIG += c++17 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

unix:  LIBS += -lssl -lcrypto
win32: LIBS += -lssl -lcrypto

SOURCES += \
    main.cpp \
    mytcpserver.cpp \
    functionsforserver.cpp

HEADERS += \
    mytcpserver.h \
    functionsforserver.h \
    dataBase.h

# Default rules for deployment.
qnx:                           target.path = /tmp/$${TARGET}/bin
else: unix:!android:           target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
