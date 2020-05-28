#include "filemanager.h"
FileManager::FileManager(QWidget *parent) : QTreeView(parent)
{
    fileSystemModel = new FileModel(this);
    setModel(fileSystemModel);
    for(int i = 1; i < header()->count(); ++i)
        header()->hideSection(i);
    QModelIndex index = fileSystemModel->index(QDir::currentPath());
    expand(index);
    scrollTo(index);
    resizeColumnToContents(0);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDefaultDropAction(Qt::MoveAction);
}

void FileManager::setMenuBar(MenuBar *MenuBar)
{
    m_MenuBar = MenuBar;
}
void FileManager::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QModelIndex item = indexAt(event->pos());
        selectionModel()->clearSelection();
        selectionModel()->setCurrentIndex(item, QItemSelectionModel::Select);
        if(item.isValid())
        {
            QMenu *menu = new QMenu(this);
            QAction *createFolder = new QAction("Создать папку", this);
            QAction *openInExplorer = new QAction("Открыть в проводнике", this);
            QAction *openInHexEdit = new QAction("Открыть в HEX редакторе", this);
            QAction *renameItem = new QAction("Переименовать", this);
            QAction *deleteItem = new QAction("Удалить", this);
            connect(createFolder, &QAction::triggered, this, &FileManager::slotCreateRecord);
            connect(openInExplorer, &QAction::triggered, this, &FileManager::slotOpenInExplorer);
            connect(openInHexEdit, &QAction::triggered, this, &FileManager::slotOpenInHexEdit);
            connect(renameItem, &QAction::triggered, this, &FileManager::slotRenameRecord);
            connect(deleteItem, &QAction::triggered, this, &FileManager::slotDialogAceptDelete);
            menu->addAction(createFolder);
            menu->addAction(openInExplorer);
            menu->addAction(openInHexEdit);
            menu->addAction(renameItem);
            menu->addAction(deleteItem);
            menu->popup(viewport()->mapToGlobal(event->pos()));
        }
    }
    if(event->button() == Qt::LeftButton)
    {
        QTreeView::mouseReleaseEvent(event);
    }

}

void FileManager::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QModelIndex index = indexAt(event->pos());
        if (fileSystemModel->fileInfo(index).isFile())
        {
            QString pathToFile = fileSystemModel->fileInfo(index).absoluteFilePath();
            QString fileName = fileSystemModel->fileName(index);
            setColumnWidth(1, 100);
            m_MenuBar->openFileByPath(pathToFile);
        }
        else
        {
            if (isExpanded(index)) setExpanded(index, false);
            else setExpanded(index, true);
        }
    }
}

void FileManager::slotCreateRecord()
{
    QModelIndex itemIndex = selectionModel()->currentIndex();
    int i = 1;
    QString path = "";
    if(fileSystemModel->fileInfo(itemIndex).isFile())
        path = fileSystemModel->fileInfo(itemIndex).absolutePath() + "/";
    else
        path = fileSystemModel->filePath(itemIndex) + "/";
    QString folder = "New folder";
    while(QDir(path+folder).exists())
    {
        folder = QString ("New folder %1").arg(i);
        ++i;
    }
    fileSystemModel->mkdir(fileSystemModel->index(path), path + folder);
    QModelIndex newItemIndex = fileSystemModel->index(path + folder);
    selectionModel()->clearSelection();
    selectionModel()->setCurrentIndex(newItemIndex, QItemSelectionModel::Select);
    edit(newItemIndex);
}

void FileManager::slotOpenInExplorer()
{
    QString path = fileSystemModel->filePath(selectionModel()->currentIndex());
#ifdef _WIN32    //Code for Windows
    QProcess::startDetached("explorer.exe", {"/select,", QDir::toNativeSeparators(path)});
#elif defined(__APPLE__)    //Code for Mac
    QProcess::execute("/usr/bin/osascript", {"-e", "tell application \"Finder\" to reveal POSIX file \"" + path + "\""});
    QProcess::execute("/usr/bin/osascript", {"-e", "tell application \"Finder\" to activate"});
#endif
}

void FileManager::slotOpenInHexEdit()
{
    if(fileSystemModel->fileInfo(selectionModel()->currentIndex()).isFile()){
        QString path = fileSystemModel->filePath(selectionModel()->currentIndex());
        setColumnWidth(1, 100);
        m_MenuBar->openFileByPath(path, true); //openInHex = true
    }
}
void FileManager::slotRenameRecord()
{
    QModelIndex cirItemIndex = selectionModel()->currentIndex();
    edit(cirItemIndex);
}
void FileManager::slotDialogAceptDelete()
{
    QPoint curPos = QCursor::pos();
    QWidget *wdj = new QWidget(nullptr, Qt::Popup);
    QHBoxLayout *lay = new QHBoxLayout;
    QPushButton *btn = new QPushButton("Удалить");
    connect(btn, &QPushButton::clicked, this, &FileManager::slotRemove);
    lay->addWidget(new QLabel("Вы действительно желаете выполнить удаление?"));
    lay->addWidget(btn);
    wdj->setLayout(lay);
    QRect rect;
    int screenWidth = QApplication::desktop()->width();
    if((curPos.x() + wdj->sizeHint().width()) > screenWidth)
        rect.setX(screenWidth - wdj->sizeHint().width());
    else if((curPos.x() - wdj->sizeHint().width() / 2) < 0)
        rect.setX(0);
    else
        rect.setX(curPos.x() - wdj->sizeHint().width() / 2);
    rect.setY(curPos.y() - wdj->sizeHint().height() / 2);
    rect.setWidth(wdj->sizeHint().width());
    rect.setHeight(wdj->sizeHint().height());
    wdj->setGeometry(rect);
    wdj->show();
}

void FileManager::slotRemove()
{
    QWidget *obj = qobject_cast<QWidget *>(sender()->parent());
    obj->close();
    QModelIndex itemIndex = selectionModel()->currentIndex();
    fileSystemModel->remove(itemIndex);
}
