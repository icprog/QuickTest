#-------------------------------------------------
#
# Project created by QtCreator 2014-10-20T09:23:37
#
#-------------------------------------------------

QT       += core gui network xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

DESTDIR += $$_PRO_FILE_PWD_/bin/

message($$_PRO_FILE_PWD_)

Debug{
    TARGET = QuickTest_Debug
}
Release{
    TARGET = QuickTest
}



CONFIG += qt warn_on

DEPENDPATH += src
DEPENDPATH += customWidget

INCLUDEPATH += src
INCLUDEPATH += customWidget

win32{
        #程序编译时依赖的相关路径
        DEPENDPATH += 3rdparty/win32

        #编译时头文件包含路径
        INCLUDEPATH += 3rdparty/win32

        HEADERS += 3rdparty/win32/autotest_global.h

        LIBS += -L$$PWD/3rdparty/win32/ -lmms_client
        LIBS += -L$$PWD/3rdparty/win32/ -lPAM300Share
        LIBS += -L$$PWD/3rdparty/win32/ -lPAM300Engine
        LIBS += -L$$PWD/3rdparty/win32/ -lMappingLinkWidget
}
unix{
        #程序编译时依赖的相关路径
        DEPENDPATH += 3rdparty/linux
        #编译时头文件包含路径
        INCLUDEPATH += 3rdparty/linux
		
        HEADERS += 3rdparty/linux/autotest_global.h

        LIBS += -L$$PWD/3rdparty/linux/ -liec61850_dll
        LIBS += -L$$PWD/3rdparty/linux/ -lPAM300Share
        LIBS += -L$$PWD/3rdparty/linux/ -lPAM300Engine
        LIBS += -L$$PWD/3rdparty/linux/ -lMappingLinkWidget
}

debug{
        #DEPENDPATH += $$PWD/build/Debug/
        #INCLUDEPATH += $$PWD/build/Debug/

        #UI_DIR += $$PWD/build/Debug/
        #RCC_DIR += $$PWD/build/Debug/
        #MOC_DIR += $$PWD/build/Debug/
        #OBJECTS_DIR += $$PWD/build/Debug/
}
release{
        #DEPENDPATH += $$PWD/build/Release/
        #INCLUDEPATH += $$PWD/build/Release/

        #UI_DIR += $$PWD/build/Release/
        #RCC_DIR += $$PWD/build/Release/
        #MOC_DIR += $$PWD/build/Release/
        #OBJECTS_DIR += $$PWD/build/Release/
}


#=========================================================================


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/wbuildtemplate.cpp \
    src/daddaction.cpp \
    src/quicktestcore.cpp \
    src/Core/cactionmodel.cpp \
    src/Core/ctemplatemodel.cpp \
    src/wactionconfig.cpp \
    src/ValueDialog/valuecalc_1101.cpp \
    src/ValueDialog/valuecalc_1102.cpp \
    src/ValueDialog/valuecalc_1103.cpp \
    src/dactioncondition.cpp \
    src/dsyncboard.cpp \
    src/ValueDialog/valuecalc_1104.cpp \
    src/ValueDialog/valuecalc_1105.cpp \
    src/ValueDialog/valuecalc_1106.cpp \
    src/ValueDialog/valuecalc_1107.cpp \
    src/ValueDialog/valuecalc_1201.cpp \
    src/dconnectserver.cpp \
    src/Core/csubstationserver.cpp \
    src/Core/Client/clientcmdthread.cpp \
    src/Core/Client/clientdatadefine.cpp \
    src/Core/Client/clientscdphase.cpp \
    src/Core/Client/clienttoolfun.cpp \
    src/Core/Client/clientxmlimpl.cpp \
    src/Core/csoftplate.cpp \
    src/ValueDialog/valuecalc_1202.cpp \
    src/ValueDialog/valuecalc_1203.cpp \
    src/ValueDialog/valuecalc_1204.cpp \
    src/ValueDialog/valuecalc_1205.cpp \
    src/ValueDialog/valuecalc_1206.cpp \
    src/ValueDialog/valuecalc_1207.cpp \
    src/ValueDialog/valuecalc_1208.cpp \
    src/ValueDialog/valuecalc_1209.cpp \
    src/ValueDialog/valuecalc_1210.cpp \
    src/ValueDialog/valuecalc_1211.cpp \
    src/ValueDialog/valuecalc_1212.cpp \
    src/ValueDialog/valuecalc_1213.cpp \
    src/test/quicktestwidget.cpp \
    src/test/quicktestinterface.cpp \
    src/test/protection.cpp \
    src/customWidget/consolecontainer.cpp \
    src/customWidget/importdlg.cpp

HEADERS  += src/mainwindow.h \
    src/wbuildtemplate.h \
    src/daddaction.h \
    src/quicktestcore.h \
    src/Core/cactionmodel.h \
    src/Core/ctemplatemodel.h \
    src/wactionconfig.h \
    src/ValueDialog/valuecalc_1101.h \
    src/ValueDialog/valuecalc_1102.h \
    src/ValueDialog/valuecalc_1103.h \
    src/dactioncondition.h \
    src/dsyncboard.h \
    src/ValueDialog/valuecalc_1104.h \
    src/ValueDialog/valuecalc.h \
    src/ValueDialog/valuecalc_1105.h \
    src/ValueDialog/ValueStruct.h \
    src/ValueDialog/valuecalc_1106.h \
    src/ValueDialog/valuecalc_1107.h \
    src/ValueDialog/valuecalc_1201.h \
    src/dconnectserver.h \
    src/Core/csubstationserver.h \
    src/Core/Client/clientcmdthread.h \
    src/Core/Client/clientdatadefine.h \
    src/Core/Client/clientscdheader.h \
    src/Core/Client/clientscdphase.h \
    src/Core/Client/clienttoolfun.h \
    src/Core/Client/clientxmlimpl.h \
    src/Core/csoftplate.h \
    src/ValueDialog/valuecalc_1202.h \
    src/ValueDialog/valuecalc_1203.h \
    src/ValueDialog/valuecalc_1204.h \
    src/ValueDialog/valuecalc_1205.h \
    src/ValueDialog/valuecalc_1206.h \
    src/ValueDialog/valuecalc_1207.h \
    src/ValueDialog/valuecalc_1208.h \
    src/ValueDialog/valuecalc_1209.h \
    src/ValueDialog/valuecalc_1210.h \
    src/ValueDialog/valuecalc_1211.h \
    src/ValueDialog/valuecalc_1212.h \
    src/ValueDialog/valuecalc_1213.h \
    src/test/quicktestwidget.h \
    src/test/quicktestinterface.h \
    src/test/protection.h \
    src/filepath.h \
    src/customWidget/consolecontainer.h \
    src/customWidget/importdlg.h

FORMS    += src/mainwindow.ui \
    src/wbuildtemplate.ui \
    src/daddaction.ui \
    src/wactionconfig.ui \
    src/ValueDialog/valuecalc_1101.ui \
    src/ValueDialog/valuecalc_1102.ui \
    src/ValueDialog/valuecalc_1103.ui \
    src/dactioncondition.ui \
    src/dsyncboard.ui \
    src/ValueDialog/valuecalc_1104.ui \
    src/ValueDialog/valuecalc_1105.ui \
    src/ValueDialog/valuecalc_1106.ui \
    src/ValueDialog/valuecalc_1107.ui \
    src/ValueDialog/valuecalc_1201.ui \
    src/dconnectserver.ui \
    src/ValueDialog/valuecalc_1202.ui \
    src/ValueDialog/valuecalc_1203.ui \
    src/ValueDialog/valuecalc_1204.ui \
    src/ValueDialog/valuecalc_1205.ui \
    src/ValueDialog/valuecalc_1206.ui \
    src/ValueDialog/valuecalc_1207.ui \
    src/ValueDialog/valuecalc_1208.ui \
    src/ValueDialog/valuecalc_1209.ui \
    src/ValueDialog/valuecalc_1210.ui \
    src/ValueDialog/valuecalc_1211.ui \
    src/ValueDialog/valuecalc_1212.ui \
    src/ValueDialog/valuecalc_1213.ui \
    src/test/quicktestwidget.ui \
    src/test/quicktestconfigdialog.ui \
    src/customWidget/importdlg.ui

RESOURCES += \
    res/skin.qrc \
    res/icon.qrc \
    res/image.qrc

