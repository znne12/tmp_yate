#ifndef MENUBAR_H
#define MENUBAR_H

#include <QObject>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QLabel>
#include <QPointer>

#include "Widgets/fileopen.h"
class FileOpen;

#include "textedit.h"
class TextEdit;

#include "hexedit.h"
class HexEdit;

#include "Widgets/imageviewer.h"
class ImageViewer;

#include "Widgets/workspacesplitter.h"
#include "Widgets/console.h"

#include "Style/appearance.h"
class Appearance;

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit MenuBar(QWidget *parent = nullptr);


    void setWorkspaceSplitter(WorkspaceSplitter *value);

    void setMdiArea(QMdiArea *value);

    void openFileByPath(QString filePath, bool openInHex = false);

    void setAppearance(Appearance *value);

private:
    // File
    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *exitAct;

    // Edit
    QMenu *editMenu;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *appearanceAct;

    // Instruments
    QMenu *instruments;
    QAction *consoleAct;
    QAction *numberAct;
    QPointer<QAction> buttonVisibleLine = nullptr;

    // Language
    QMenu *languageMenu;
    QAction * setCppLanguage;

    // Help
    QMenu *helpMenu;
    QAction *aboutAct;


    TextEdit *activeBuffer() const;

    // Рабочая область. Сюда можно добавлять свои виджеты
    WorkspaceSplitter *workspaceSplitter;

    QMdiArea *mdiArea;

    // Внешний вид приложения
    Appearance *appearance;


private:

    QMdiSubWindow *findMdiChild(const QString &fileName) const;
    FileOpen *fileOpen;

private slots:
    void saveFile();
    void saveAsFile();
    void print();
    void newFile();
    void openFile();
    void cut();
    void copy();
    void paste();
    void about();
    void setLanguage();
    void addConsole();
    void switchNumberLine();

};

#endif // MENUBAR_H
