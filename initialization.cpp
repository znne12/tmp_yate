#include "initialization.h"
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

Initialization::Initialization()
{
    mainSplitter = new QSplitter;
    workspaceSplitter = new WorkspaceSplitter(Qt::Vertical);
    mdiArea = new QMdiArea();
    mainMenuBar = new MenuBar();

}

MainWindow* Initialization::createMainWindow()
{
    // Загружаем настройки из файла,
    // в зависимости от настроек создаем MainWindow
    window = new MainWindow();
    fileManager = new FileManager(window);
    fileManager->setMenuBar(mainMenuBar);

    settings = new SettingsLoader("settings.json");

    createMenu();
    createMdiArea();

    appearance = new Appearance(window);
    appearance->setWindow(window);
    appearance->setSettings(settings);
    appearance->setMdiArea(mdiArea);

    QString jsonFileName = "yate-settings.json";
    try {settings = SettingsLoader::getInstance(jsonFileName);}
    catch (std::invalid_argument &openFileErr)
    {
        qDebug() << "Ошибка открытия файла: " << QString(openFileErr.what());
    }
    catch (QJsonParseError &jsonError)
    {
        qDebug() << "Ошибка чтения файла " + jsonFileName + ": " << jsonError.errorString();
    }
    if (settings != nullptr)
    {
        QString title = settings->getValue("caption").toString();
        window->setWindowTitle(title);

        QString fontFamily = settings->getValue("font-family").toString();
        appearance->setFontFamily(fontFamily);

        QString theme = settings->getValue("theme").toString();
        appearance->setStyleName(theme);

        QString syntaxFontFamily = settings->getValue("syntax-font-family").toString();
        appearance->setSyntaxFontFamily(syntaxFontFamily);

        QString syntaxTheme = settings->getValue("syntax-theme").toString();
        appearance->setSyntaxTheme(syntaxTheme);


    }
    mainMenuBar->setAppearance(appearance);
    window->setAppearance(appearance);

    return window;
}

QSplitter *Initialization::getMainSplitter() const
{
    return mainSplitter;
}

WorkspaceSplitter *Initialization::getWorkspaceSplitter() const
{
    return workspaceSplitter;
}

Appearance *Initialization::getAppearance() const
{
    return appearance;
}

void Initialization::createMdiArea()
{
    // mdiArea

    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setViewMode(QMdiArea::ViewMode::TabbedView);
    mdiArea->setTabsClosable(true);
    mdiArea->setTabsMovable(true);

    mainSplitter->addWidget(fileManager);
    workspaceSplitter->addWidget(mdiArea);
    mainSplitter->addWidget(workspaceSplitter);

    window->setCentralWidget(mainSplitter);
    window->setMainSplitter(mainSplitter);
    window->setMdiArea(mdiArea);
    window->setWorkspaceSplitter(workspaceSplitter);
    window->setMenuBar(mainMenuBar);

}

void Initialization::createMenu()
{

    //menuBar = new QMenuBar();
    mainMenuBar->setWorkspaceSplitter(workspaceSplitter);
    mainMenuBar->setMdiArea(mdiArea);



}
