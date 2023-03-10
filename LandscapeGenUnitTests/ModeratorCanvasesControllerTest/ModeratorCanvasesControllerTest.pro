include(gtest_dependency.pri)

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_moderatorcanvasescontrollertest.cpp \
    LandscapeCanvasMock.cpp

HEADERS += \
    LandscapeCanvasMock.h

#LandscapeGenLib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../LandscapeGenLib/release/ -lLandscapeGenLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../LandscapeGenLib/debug/ -lLandscapeGenLib
else:unix: LIBS += -L$$PWD/../../LandscapeGenLib/ -lLandscapeGenLib

INCLUDEPATH += $$PWD/../../LandscapeGenLib
DEPENDPATH += $$PWD/../../LandscapeGenLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../LandscapeGenLib/release/libLandscapeGenLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../LandscapeGenLib/debug/libLandscapeGenLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../LandscapeGenLib/release/LandscapeGenLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../LandscapeGenLib/debug/LandscapeGenLib.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../LandscapeGenLib/libLandscapeGenLib.a
