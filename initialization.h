#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <QObject>
#include "mainwindow.h"
#include "settingsloader.h"
#include "Style/template_style.h"
#include "Widgets/filemanager.h"
#include "Widgets/workspacesplitter.h"
#include "MenuBar/menubar.h"
#include "Style/appearance.h"
#include <QMdiArea>
#include <QSplitter>
#include <QMessageBox>

class Initialization : public QObject
{
    Q_OBJECT
public:
    Initialization();
    MainWindow *createMainWindow();

    QSplitter *getMainSplitter() const;

    WorkspaceSplitter *getWorkspaceSplitter() const;

    Appearance *getAppearance() const;

private:
    MainWindow* window;

    QSplitter *mainSplitter;
    WorkspaceSplitter *workspaceSplitter;
    MenuBar *mainMenuBar;

    QMdiArea *mdiArea;

    Appearance *appearance;
    SettingsLoader *settings = nullptr;
    FileManager *fileManager;

    void createMdiArea();
    void createMenu();

};

#endif // INITIALIZATION_H
