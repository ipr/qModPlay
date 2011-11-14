#-------------------------------------------------
#
# Project created by QtCreator 2011-09-07T02:05:18
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = qModPlay
TEMPLATE = app

# decompression support
INCLUDEPATH += ../qXpkLib

SOURCES += main.cpp\
        MainWindow.cpp \
    AnsiFile.cpp \
    FileType.cpp \
    SymphoniePlayer.cpp \
    DigiBoosterPlayer.cpp \
    DigiBoosterProPlayer.cpp \
    TfmxPlayer.cpp \
    AhxPlayer.cpp \
    PlaybackHandler.cpp \
    MadTracker2Player.cpp \
    ProTrackerPlayer.cpp \
    OktalyzerPlayer.cpp \
    ScreamTrackerPlayer.cpp \
    ImpulseTrackerPlayer.cpp \
    FastTrackerPlayer.cpp \
    AudioSample.cpp \
    ArchiveHandler.cpp

HEADERS  += MainWindow.h \
    AnsiFile.h \
    FileType.h \
    ModPlayer.h \
    SymphoniePlayer.h \
    DigiBoosterPlayer.h \
    DigiBoosterProPlayer.h \
    TfmxPlayer.h \
    AhxPlayer.h \
    AudioSample.h \
    PlaybackHandler.h \
    MadTracker2Player.h \
    ProTrackerPlayer.h \
    OktalyzerPlayer.h \
    ScreamTrackerPlayer.h \
    ImpulseTrackerPlayer.h \
    DecodeCtx.h \
    FastTrackerPlayer.h \
    ArchiveHandler.h

FORMS    += MainWindow.ui

LIBS += -lqXpkLib

CONFIG (debug, debug|release) 
{
    LIBS += -L../qXpkLib-build-desktop/debug
} 
CONFIG (release, debug|release) 
{
    LIBS += -L../qXpkLib-build-desktop/release
}






