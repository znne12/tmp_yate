#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QWidget>
#include <QTreeView>
#include <QHeaderView>
#include <QDesktopWidget>

#include <QDebug>

#include "MenuBar/menubar.h"
#include "textedit.h"
#include "filemodel.h"

class FileManager : public QTreeView
{
    Q_OBJECT
public:
    explicit FileManager(QWidget *parent = nullptr);
    void setMenuBar(MenuBar *MenuBar);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    FileModel *fileSystemModel;
    MenuBar *m_MenuBar;
    QPoint m_DragPos;

private slots:
    void slotCreateRecord();
    void slotOpenInExplorer();
    void slotOpenInHexEdit();
    void slotRenameRecord();
    void slotDialogAceptDelete();
    void slotRemove();
};

#endif // FILEMANAGER_H
