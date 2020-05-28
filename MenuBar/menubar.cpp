#include "menubar.h"



MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent)
{
    fileOpen = new FileOpen(this);
    // fileMenu
    fileMenu = this->addMenu("File");

    newAct = new QAction("New");
    newAct->setShortcuts(QKeySequence::New);
    fileMenu->addAction(newAct);

    openAct = new QAction("Open");
    openAct->setShortcuts(QKeySequence::Open);
    fileMenu->addAction(openAct);

    saveAct = new QAction("Save");
    saveAct->setShortcuts(QKeySequence::Save);
    fileMenu->addAction(saveAct);

    saveAsAct = new QAction("Save As");
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    fileMenu->addAction(saveAsAct);

    printAct = new QAction("Print");
    printAct->setShortcuts(QKeySequence::Print);
    fileMenu->addAction(printAct);

    fileMenu->addSeparator();

    exitAct = new QAction("Exit");
    exitAct->setShortcuts(QKeySequence::Quit);
    fileMenu->addAction(exitAct);

    // editMenu
    editMenu = this->addMenu("Edit");

    cutAct = new QAction("Cut");
    cutAct->setShortcuts(QKeySequence::Cut);
    editMenu->addAction(cutAct);

    copyAct = new QAction("Copy");
    copyAct->setShortcuts(QKeySequence::Copy);
    editMenu->addAction(copyAct);

    pasteAct = new QAction("Paste");
    pasteAct->setShortcuts(QKeySequence::Paste);
    editMenu->addAction(pasteAct);

    // Instruments menu
    instruments = this->addMenu("Instruments");
    consoleAct = new QAction("Console");
    instruments->addAction(consoleAct);

    appearanceAct = new QAction("Appearance");
    instruments->addAction(appearanceAct);

    //numberAct = new QAction("Line index");
    //instruments->addAction(numberAct);

   buttonVisibleLine = new QAction("Line index",this);
   buttonVisibleLine->setCheckable(true);
   //buttonVisibleLine->setChecked(true);
   instruments->addAction(buttonVisibleLine);


    // language menu
    languageMenu = this->addMenu("Language");

    setCppLanguage = new QAction("C++");
    languageMenu->addAction(setCppLanguage);



    // helpMenu
    helpMenu = this->addMenu("Help");

    aboutAct = new QAction("About");
    helpMenu->addAction(aboutAct);



    // connections
    connect(newAct, &QAction::triggered, this, &MenuBar::newFile);
    connect(openAct, &QAction::triggered, this, &MenuBar::openFile);
    connect(saveAct, &QAction::triggered, this, &MenuBar::saveFile);
    connect(saveAsAct, &QAction::triggered, this, &MenuBar::saveAsFile);
    connect(exitAct, &QAction::triggered, qApp, &QApplication::closeAllWindows);
    connect(cutAct, &QAction::triggered, this, &MenuBar::cut);
    connect(copyAct, &QAction::triggered, this, &MenuBar::copy);
    connect(pasteAct, &QAction::triggered, this, &MenuBar::paste);
    connect(printAct, &QAction::triggered, this, &MenuBar::print);
    connect(aboutAct, &QAction::triggered, this, &MenuBar::about);
    connect(consoleAct, &QAction::triggered, this, &MenuBar::addConsole);
    connect(buttonVisibleLine, &QAction::triggered, this, &MenuBar::switchNumberLine);
}

void MenuBar::saveFile()
{
    if (auto tedit = activeBuffer()) {
        if (tedit->buffer()->hasPath()) {
            tedit->saveFile();
        } else {
            saveAsFile();
        }
    }
}

void MenuBar::saveAsFile()
{
    if (auto tedit = activeBuffer())
        tedit->saveAsFile();
}

void MenuBar::print()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()){
        QPrinter printer;
        QPrintDialog printDialog(&printer, this);
        printDialog.setWindowTitle("Print");
        if (printDialog.exec() != QDialog::Accepted) return;
        QString className = activeSubWindow->widget()->metaObject()->className();
        if( className == "TextEdit")
        {
            auto edit = qobject_cast<TextEdit*>(activeSubWindow->widget());
            edit->print(&printer);
        } else if (className == "HexEdit"){
            auto edit = qobject_cast<HexEdit*>(activeSubWindow->widget());
            edit->print(&printer);
        }
    }
}

void MenuBar::newFile()
{
    fileOpen->setFilePath("", false);
}

void MenuBar::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    openFileByPath(fileName);
}

void MenuBar::cut()
{
    if (activeBuffer()) activeBuffer()->cut();
}

void MenuBar::copy()
{
    if (activeBuffer()) activeBuffer()->copy();
}

void MenuBar::paste()
{
    if (activeBuffer()) activeBuffer()->paste();
}

void MenuBar::about()
{
    QMessageBox::about(this, tr("About YATE"),
                       tr("The <b>YATE</b> is a multifunctional and convenient text editor."));
}

void MenuBar::setLanguage()
{
    QString language = qobject_cast<QAction *>(sender())->text();
    QString suffix;
    if (language == "C++")
        suffix = "cpp";
    if (activeBuffer()) activeBuffer()->highlighter()->setSuffix(suffix);
}

void MenuBar::addConsole()
{
    workspaceSplitter->addNewWidget(new Console());
}

void MenuBar::switchNumberLine()
{
    //if(!buttonVisibleLine) {return;}
    if (activeBuffer()){

        if(activeBuffer()->getVisibleLine())
        {
            activeBuffer()->VisibleLine(0);
            buttonVisibleLine->setChecked(false);
        }
        else
        {
            activeBuffer()->VisibleLine(1);
            buttonVisibleLine->setChecked(true);
        }
        // qDebug()<< appearance->mySharedValue();
        //    if(buttonVisibleLine->isChecked())
        //    {
        //        activeBuffer()->VisibleLine(0);
        //        //SettingsLoader::setMySharedValue(0);
        //    }
        //    else
        //    {
        //        activeBuffer()->VisibleLine(1);
        //        //SettingsLoader::setMySharedValue(1);
        //    }
    }
    else
    {
        buttonVisibleLine->setChecked(false);
    }

}


TextEdit *MenuBar::activeBuffer() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<TextEdit*>(activeSubWindow->widget());
    return nullptr;
}

void MenuBar::setAppearance(Appearance *value)
{
    appearance = value;
    appearance->setMenuBar(this);

    connect(appearanceAct, &QAction::triggered, appearance, &Appearance::show);
}


void MenuBar::openFileByPath(QString filePath, bool openInHex)
{
    fileOpen->setMdiArea(mdiArea);
    fileOpen->setAppearance(appearance);

    if (filePath.isEmpty()) return;

    if (QMdiSubWindow *existing = findMdiChild(filePath))
    {
        mdiArea->setActiveSubWindow(existing);
        return;
    }
    fileOpen->setFilePath(filePath, openInHex);

}


void MenuBar::setMdiArea(QMdiArea *value)
{
    mdiArea = value;
}

void MenuBar::setWorkspaceSplitter(WorkspaceSplitter *value)
{
    workspaceSplitter = value;
}

QMdiSubWindow *MenuBar::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();


    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        QString className = window->widget()->metaObject()->className();

        if (className == "TextEdit")
        {
            TextEdit *textEdit = qobject_cast<TextEdit*>(window->widget());
            if (textEdit->buffer()->pathFile() == canonicalFilePath)
                return window;
        }
        else if (className == "ImageViewer")
        {
            ImageViewer *imgViewer = qobject_cast<ImageViewer*>(window->widget());
            if (imgViewer->getFilePath() == canonicalFilePath)
                return window;
        }
        else if (className == "HexEdit")
        {
            HexEdit *hexEdit = qobject_cast<HexEdit*>(window->widget());
            if (hexEdit->buffer()->pathFile() == canonicalFilePath)
                return window;
        }

    }
    return nullptr;
}
