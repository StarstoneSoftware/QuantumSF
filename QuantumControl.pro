QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += debug_and_release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Sorry Microsoft...
DEFINES += _CRT_SECURE_NO_WARNINGS

linux-g++:			QMAKE_LFLAGS += -no-pie


SOURCES += \
    dlgabout.cpp \
    main.cpp \
    mainwindow.cpp \
    quantumdevice.cpp \
    quantumgui.cpp \
    serialchooser.cpp \
    wavelengthgraph.cpp

HEADERS += \
    dlgabout.h \
    mainwindow.h \
    quantumdevice.h \
    quantumgui.h \
    serialchooser.h \
    wavelengthgraph.h

FORMS += \
    dlgabout.ui \
    mainwindow.ui \
    quantumgui.ui \
    serialchooser.ui

RESOURCES += \
    DesktopResources.qrc

TRANSLATIONS += \
    QuantumControl_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macx: {
#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
QMAKE_INFO_PLIST = Resources/Info.plist
RC_FILE = Resources/QuantumIcon.icns
}

