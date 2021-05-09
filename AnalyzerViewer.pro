QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += \
    -lopengl32 \
    -lglu32 \
    -lglut \

SOURCES += sources/*

HEADERS += headers/*

FORMS += forms/*

INCLUDEPATH += headers

INCLUDEPATH += matlab


Release:DESTDIR = build/release
Release:OBJECTS_DIR = build/release/.obj/$$TARGET
Release:MOC_DIR = build/release/.moc/$$TARGET
Release:RCC_DIR = build/release/.rcc/$$TARGET
Release:UI_DIR = build/release/.ui/$$TARGET

Debug:DESTDIR = build/debug
Debug:OBJECTS_DIR = build/debug/.obj/$$TARGET
Debug:MOC_DIR = build/debug/.moc/$$TARGET
Debug:RCC_DIR = build/debug/.rcc/$$TARGET
Debug:UI_DIR = build/debug/.ui/$$TARGET

QMAKE_POST_LINK += clean.bat

RC_ICONS = resources/icons/app_icon.ico

#TARGET = "Analyzer"


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc

DISTFILES += \
    style_green.css \
    style_white.css
