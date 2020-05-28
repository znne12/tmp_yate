QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 debug
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MenuBar/menubar.cpp \
    Style/appearance.cpp \
    Style/template_style.cpp \
    Widgets/console.cpp \
    Widgets/filemanager.cpp \
    Widgets/filemodel.cpp \
    Widgets/fileopen.cpp \
    Widgets/imageviewer.cpp \
    Widgets/widgetfoundation.cpp \
    Widgets/workspacesplitter.cpp \
    buffer.cpp \
    hexedit.cpp \
    highlighter.cpp \
    initialization.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsloader.cpp \
    textedit.cpp

HEADERS += \
    MenuBar/menubar.h \
    Style/appearance.h \
    Style/template_style.h \
    Widgets/console.h \
    Widgets/filemanager.h \
    Widgets/filemodel.h \
    Widgets/fileopen.h \
    Widgets/imageviewer.h \
    Widgets/widgetfoundation.h \
    Widgets/workspacesplitter.h \
    buffer.h \
    hexedit.h \
    highlighter.h \
    initialization.h \
    mainwindow.h \
    settingsloader.h \
    textedit.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Style/Resources.qrc \
    res.qrc

FORMS += \
    Style/appearance.ui
