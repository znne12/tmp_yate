#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMdiSubWindow>
#include <QSplitter>
#include <QMap>

#include "Widgets/workspacesplitter.h"
#include "Widgets/console.h"
#include "settingsloader.h"
#include "textedit.h"
#include "MenuBar/menubar.h"

#include "Style/appearance.h"
class Appearance;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;


    void setMainSplitter(QSplitter *value);

    void setMdiArea(QMdiArea *value);

    void setWorkspaceSplitter(WorkspaceSplitter *workspaceSplitter);


    SettingsLoader *getSettings() const;

    void setAppearance(Appearance *value);

protected:
    void closeEvent(QCloseEvent *event) override;


private:

    SettingsLoader *settings = nullptr;
    QSplitter *mainSplitter;
    QMdiArea *mdiArea;
    Appearance *appearance;
    WorkspaceSplitter *workspaceSplitter;


public slots:

};
#endif // MAINWINDOW_H
