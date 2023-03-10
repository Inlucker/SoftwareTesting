QT += testlib sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_usercontrollertest.cpp \
    ../../common/CanvasBLBuilder.cpp \
    ../../common/CanvasMother.cpp \
    ../../common/DataBaseBuilder.cpp \
    ../../common/HeightsMapBuilder.cpp

HEADERS += \
    ../../common/CanvasBLBuilder.h \
    ../../common/CanvasMother.h \
    ../../common/DataBaseBuilder.h \
    ../../common/HeightsMapBuilder.h \
    ../../common/common.h

INCLUDEPATH += $$PWD/../../common

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

#libpq for windows
win32:CONFIG(release, debug|release): LIBS += -L'C:/Program Files/PostgreSQL/14/lib/' -llibpq
else:win32:CONFIG(debug, debug|release): LIBS += -L'C:/Program Files/PostgreSQL/14/lib/' -llibpq

INCLUDEPATH += 'C:/Program Files/PostgreSQL/14/include'
DEPENDPATH += 'C:/Program Files/PostgreSQL/14/include'

#libpq for ubuntu
unix:!macx: LIBS += -L'/usr/include/postgresql' -lpq

INCLUDEPATH += '/usr/include/postgresql'
DEPENDPATH += '/usr/include/postgresql'

unix:!macx: PRE_TARGETDEPS += '/usr/include/postgresql/libpq'

