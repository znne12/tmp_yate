#include "fileopen.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>


FileOpen::FileOpen(QObject *parent) : QObject(parent)
{
    imageFormats = {"png", "jpg", "bmp", "tiff", "jpeg"};
}

void FileOpen::setFilePath(const QString &filePath, bool openInHex)
{
    QString fileFormat = QFileInfo(filePath).suffix();

    if(openInHex){
        Buffer *buffer = new Buffer(filePath);
        HexEdit *hexEdit = new HexEdit(appearance, buffer);
        bufferPtrs.push_front(buffer);
        mdiArea->addSubWindow(hexEdit);
        connect(hexEdit, &HexEdit::deleteBuffer, this, &FileOpen::deleteBuffer);
        hexEdit->show();
        return;
    }
    // if the file is an image open image file
    if (imageFormats.lastIndexOf(fileFormat) != -1)
    {
        ImageViewer *image = new ImageViewer;
        image->setImagePath(filePath);
        mdiArea->addSubWindow(image);
        image->setWindowTitle(QFileInfo(filePath).fileName());
        image->show();

    }
    // other files are opened by a regular text editor
    else
    {
        Buffer *buffer = new Buffer(filePath);
        if (buffer->getFileSize() > 100000000) {
            qDebug() << "Файл должен быть меньше 100Мб";
            delete buffer;
            return;
        }
        TextEdit *txEdit = new TextEdit(appearance, buffer);
        bufferPtrs.push_front(buffer);
        mdiArea->addSubWindow(txEdit);
        connect(txEdit, &TextEdit::deleteBuffer, this, &FileOpen::deleteBuffer);

        txEdit->show();
    }
}

void FileOpen::setMdiArea(QMdiArea *value)
{
    mdiArea = value;
}

void FileOpen::setAppearance(Appearance *value)
{
    appearance = value;
}

QVector<Buffer *> FileOpen::getContainerWithBuffers() const
{
    return bufferPtrs;
}

void FileOpen::deleteBuffer(Buffer *buffer)
{
   qint16 pos =  bufferPtrs.indexOf(buffer);
   if (pos != -1) {
       delete buffer;
       bufferPtrs.remove(pos);
       return;
   }
   delete(buffer);
}
