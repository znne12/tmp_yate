#include "filemodel.h"

bool FileModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) // drives
        return true;
    return QDir(filePath(parent)).count() > 2;
}

FileModel::FileModel(QWidget *parent) : QFileSystemModel(parent)
{
    setRootPath("");
    setReadOnly(false);
}
