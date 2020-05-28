#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QWidget>
#include <QFileSystemModel>

class FileModel : public QFileSystemModel
{
protected:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
public:
    explicit FileModel(QWidget *parent = nullptr);
};

#endif // FILEMODEL_H
