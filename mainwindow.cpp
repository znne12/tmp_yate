#include "mainwindow.h"
#include "textedit.h"
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMdiArea>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QStringList>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QString jsonFileName = "settings.json";

    try { 
        settings = SettingsLoader::getInstance(jsonFileName); 
    }
    catch (std::invalid_argument &openFileErr)
    {
        qDebug() << "Ошибка открытия файла: " << QString(openFileErr.what());
    }
    catch (QJsonParseError &jsonError)
    {
        qDebug() << "Ошибка чтения файла " + jsonFileName + ": " << jsonError.errorString();
    }


// Вариант для уникальных имён ключей и массивов
//    window()->setGeometry(settings->getValue("position-x").toInt(),
//                          settings->getValue("position-y").toInt(),
//                          settings->getValue("width").toInt(),
//                          settings->getValue("height").toInt());


//    window()->setGeometry(settings->getValue({"main-window", "array1[0]"}).toInt(),
//                          settings->getValue({"main-window", "array1[1]"}).toInt(),
//                          settings->getValue({"main-window", "array1[2]"}).toInt(),
//                          settings->getValue({"main-window", "array1[3]"}).toInt());

    if (settings != nullptr)
        window()->setGeometry(settings->getValue({"main-window", "position-x"}).toInt(),
                              settings->getValue({"main-window", "position-y"}).toInt(),
                              settings->getValue({"main-window", "width"}).toInt(),
                              settings->getValue({"main-window", "height"}).toInt());

    mainSplitter = nullptr;
    mdiArea = nullptr;
    workspaceSplitter = nullptr;

}

MainWindow::~MainWindow()
{
}



void MainWindow::closeEvent(QCloseEvent *event){
    if (settings != nullptr)
    {
        settings->setValue({"main-window", "position-x"}, window()->geometry().x());
        settings->setValue({"main-window", "position-y"}, window()->geometry().y());
        settings->setValue({"main-window", "width"}, window()->geometry().width());
        settings->setValue({"main-window", "height"}, window()->geometry().height());

        settings->setValue("theme", appearance->getStyleName());
        settings->setValue("syntax-theme", appearance->getSyntaxTheme());
        settings->setValue("font-family", appearance->getFontFamily());
        settings->setValue("syntax-font-family", appearance->getSyntaxFontFamily());
        appearance->saveData();
        
        try {
            settings->saveJsonToFile();
        }
        catch (std::runtime_error saveFileErr)
        {
            qDebug() << "Ошибка сохранения файла: " << QString(saveFileErr.what());
        }

        delete settings;
    }

    mdiArea->closeAllSubWindows();
    if ((mdiArea->subWindowList()).isEmpty())
        event->accept();
    else
        event->ignore();
}

void MainWindow::setAppearance(Appearance *value)
{
    appearance = value;
}

SettingsLoader *MainWindow::getSettings() const
{
    return settings;
}

void MainWindow::setMdiArea(QMdiArea *value)
{
    mdiArea = value;
}

void MainWindow::setWorkspaceSplitter(WorkspaceSplitter *workspaceSplitter)
{
    this->workspaceSplitter = workspaceSplitter;
}

void MainWindow::setMainSplitter(QSplitter *value)
{
    mainSplitter = value;
}


