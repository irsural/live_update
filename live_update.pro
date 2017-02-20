#-------------------------------------------------
#
# Project created by QtCreator 2013-04-25T14:17:36
#
#-------------------------------------------------

VERSION = 0.1.0.2
#DEFINES += VER=\'\"$${VERSION}"\'
VERSTR = '\\"$${VERSION}\\"'
DEFINES += VER=\"$${VERSTR}\"

DEFINES += IRS_USE_HID_WIN_API=1

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = live_update
TEMPLATE = app

INCLUDEPATH += .
INCLUDEPATH += ../boost/
INCLUDEPATH += ../mxsrclib/
INCLUDEPATH += ../mxsrclib/arch/qt/
INCLUDEPATH += ../extern_libs/winapi/hid

win32-g++ {
  QMAKE_CXXFLAGS += -finput-charset=CP1251
  QMAKE_CXXFLAGS += -fno-strict-aliasing
  QMAKE_CXXFLAGS += -Wno-deprecated
  QMAKE_CXXFLAGS += -Wno-unused-parameter
  QMAKE_CXXFLAGS += -Wextra
  QMAKE_CXXFLAGS += -Woverloaded-virtual
  QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
  QMAKE_CXXFLAGS += -Wno-unused-function
  QMAKE_CXXFLAGS += -std=c++11
}

win32-msvc2008 {
  QMAKE_CXXFLAGS -= -Zc:wchar_t-
  QMAKE_CXXFLAGS -= -w34100
  QMAKE_CXXFLAGS += -wd4800
  QMAKE_CXXFLAGS += -w44355
  QMAKE_CXXFLAGS += -wd4996
  QMAKE_CXXFLAGS += -MP8
  QMAKE_CFLAGS_RELEASE = -Zi -MDd
}

win32-msvc2010 {
  QMAKE_CXXFLAGS -= -Zc:wchar_t-
  QMAKE_CXXFLAGS -= -w34100
  QMAKE_CXXFLAGS += -wd4800
  QMAKE_CXXFLAGS += -w44355
  QMAKE_CXXFLAGS += -wd4996
  QMAKE_CFLAGS_RELEASE = -Zi -MDd
  QMAKE_CXXFLAGS += -MP8
}

win32-msvc2012 {
  QMAKE_CXXFLAGS -= -Zc:wchar_t-
  QMAKE_CXXFLAGS -= -w34100
  QMAKE_CXXFLAGS += -wd4250
  QMAKE_CXXFLAGS += -wd4800
  QMAKE_CXXFLAGS += -w44355
  QMAKE_CXXFLAGS += -wd4996
  QMAKE_CFLAGS_RELEASE = -Zi -MDd
  QMAKE_CXXFLAGS += -MP8
}

LIBS += -lws2_32
LIBS += -lwinmm
LIBS += -lHid
LIBS += -lSetupapi
LIBS += -L"../boost/stage/lib/"

SOURCES += main.cpp\
        mainwindow.cpp \
    ../mxsrclib/timer.cpp \
    ../mxsrclib/niusbgpib_hardflow.cpp \
    ../mxsrclib/niusbgpib.cpp \
    ../mxsrclib/mxnetr.cpp \
    ../mxsrclib/mxnetc.cpp \
    ../mxsrclib/mxnet.cpp \
    ../mxsrclib/mxifar.cpp \
    ../mxsrclib/mxextchart.cpp \
    ../mxsrclib/mxextbase.cpp \
    ../mxsrclib/mxdata.cpp \
    ../mxsrclib/meassup.cpp \
    ../mxsrclib/measmul.cpp \
    ../mxsrclib/irsvariant.cpp \
    ../mxsrclib/irstime.cpp \
    ../mxsrclib/irstcpip.cpp \
    ../mxsrclib/irssysutils.cpp \
    ../mxsrclib/irsstrm.cpp \
    ../mxsrclib/irsstring.cpp \
    ../mxsrclib/irsstd.cpp \
    ../mxsrclib/irsprop.cpp \
    ../mxsrclib/irsmenu.cpp \
    ../mxsrclib/irsmem.cpp \
    ../mxsrclib/irsmcutil.cpp \
    ../mxsrclib/irsmbus.cpp \
    ../mxsrclib/irsmatrix.cpp \
    ../mxsrclib/irsmath.cpp \
    ../mxsrclib/irslwip.cpp \
    ../mxsrclib/irslocale.cpp \
    ../mxsrclib/irskbd.cpp \
    ../mxsrclib/irsint.cpp \
    ../mxsrclib/irsinit.cpp \
    ../mxsrclib/irshfftp.cpp \
    ../mxsrclib/irsgpio.cpp \
    ../mxsrclib/irsfunnel.cpp \
    ../mxsrclib/irsfilter.cpp \
    ../mxsrclib/irsexcept.cpp \
    ../mxsrclib/irserror.cpp \
    ../mxsrclib/irsdsp.cpp \
    ../mxsrclib/irsdev.cpp \
    ../mxsrclib/irsdbgutil.cpp \
    ../mxsrclib/irscpu.cpp \
    ../mxsrclib/irscalc.cpp \
    ../mxsrclib/irsalg.cpp \
    ../mxsrclib/irsadc.cpp \
    ../mxsrclib/hardflowg.cpp \
    ../mxsrclib/dbgprn.cpp \
    ../mxsrclib/csvwork.cpp \
    ../mxsrclib/correct_alg.cpp \
    ../mxsrclib/arch/qt/mxifa.cpp \
    ../mxsrclib/arch/qt/counter.cpp \
    settings.cpp \
    ../mxsrclib/irsdcp.cpp \
    updatedialog.cpp

HEADERS  += mainwindow.h \
    ../mxsrclib/tstlan4abs.h \
    ../mxsrclib/timer.h \
    ../mxsrclib/niusbgpib_hardflow.h \
    ../mxsrclib/niusbgpib.h \
    ../mxsrclib/mxnetr.h \
    ../mxsrclib/mxnetd.h \
    ../mxsrclib/mxnetc.h \
    ../mxsrclib/mxnet.h \
    ../mxsrclib/mxifar.h \
    ../mxsrclib/mxifa.h \
    ../mxsrclib/mxextchart.h \
    ../mxsrclib/mxextbase.h \
    ../mxsrclib/mxdatastd.h \
    ../mxsrclib/mxdata.h \
    ../mxsrclib/meassup.h \
    ../mxsrclib/measmul.h \
    ../mxsrclib/measdef.h \
    ../mxsrclib/lwipopts.h \
    ../mxsrclib/irsvariant.h \
    ../mxsrclib/irstree.h \
    ../mxsrclib/irstime.h \
    ../mxsrclib/irstest.h \
    ../mxsrclib/irstcpip.h \
    ../mxsrclib/irstable.h \
    ../mxsrclib/irssysutils.h \
    ../mxsrclib/irsstrmstd.h \
    ../mxsrclib/irsstrm.h \
    ../mxsrclib/irsstring.h \
    ../mxsrclib/irsstrdefs.h \
    ../mxsrclib/irsstrconv.h \
    ../mxsrclib/irsstd.h \
    ../mxsrclib/irsspi.h \
    ../mxsrclib/irsrect.h \
    ../mxsrclib/irsprop.h \
    ../mxsrclib/irspch.h \
    ../mxsrclib/irsparamabs.h \
    ../mxsrclib/irsnetdefs.h \
    ../mxsrclib/irsmenu.h \
    ../mxsrclib/irsmem.h \
    ../mxsrclib/irsmcutil.h \
    ../mxsrclib/irsmbus.h \
    ../mxsrclib/irsmatrix.h \
    ../mxsrclib/irsmath.h \
    ../mxsrclib/irslwip.h \
    ../mxsrclib/irslocale.h \
    ../mxsrclib/irslimits.h \
    ../mxsrclib/irskbd.h \
    ../mxsrclib/irsint.h \
    ../mxsrclib/irsinit.h \
    ../mxsrclib/irshfftp.h \
    ../mxsrclib/irsgpio.h \
    ../mxsrclib/irsfunnel.h \
    ../mxsrclib/irsfinal.h \
    ../mxsrclib/irsfilter.h \
    ../mxsrclib/irsexcept.h \
    ../mxsrclib/irserror.h \
    ../mxsrclib/irsdsp.h \
    ../mxsrclib/irsdev.h \
    ../mxsrclib/irsdefsbase.h \
    ../mxsrclib/irsdefs.h \
    ../mxsrclib/irsdbgutil.h \
    ../mxsrclib/irsdacabs.h \
    ../mxsrclib/irscpu.h \
    ../mxsrclib/irscpp.h \
    ../mxsrclib/irsconsolestd.h \
    ../mxsrclib/irsconsole.h \
    ../mxsrclib/irsconfig.h \
    ../mxsrclib/irschartwin.h \
    ../mxsrclib/irscalc.h \
    ../mxsrclib/irsalg.h \
    ../mxsrclib/irsadcabs.h \
    ../mxsrclib/irsadc.h \
    ../mxsrclib/hardflowg.h \
    ../mxsrclib/dbgprn.h \
    ../mxsrclib/csvwork.h \
    ../mxsrclib/correct_alg.h \
    ../mxsrclib/arch/qt/ni488.h \
    ../mxsrclib/arch/qt/mxnetda.h \
    ../mxsrclib/arch/qt/mxifal.h \
    ../mxsrclib/arch/qt/irsdefsarch.h \
    ../mxsrclib/arch/qt/Decl-32.h \
    ../mxsrclib/arch/qt/counter.h \
    settings.h \
    ../mxsrclib/irsdcp.h \
    updatedialog.h

FORMS    += mainwindow.ui \
    updatedialog.ui
