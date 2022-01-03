QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    albumview.cpp \
    artistview.cpp \
    createplaylistdialogue.cpp \
    home.cpp \
    main.cpp \
    mainwindow.cpp \
    playlist.cpp \
    playlistsview.cpp \
    search.cpp \
    loing.cpp \
    registerwindow.cpp \
    qkeccakhash/qkeccakhash.cpp \
    qkeccakhash/keccakimpl.cpp

HEADERS += \
    albumview.h \
    artistview.h \
    createplaylistdialogue.h \
    home.h \
    mainwindow.h \
    playlist.h \
    playlistsview.h \
    search.h \
    loing.h \
    registerwindow.h \
    qkeccakhash/qkeccakhash.h

FORMS += \
    albumview.ui \
    artistview.ui \
    createplaylistdialogue.ui \
    home.ui \
    mainwindow.ui \
    playlist.ui \
    playlistsview.ui \
    search.ui \
    loing.ui \
    registerwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
