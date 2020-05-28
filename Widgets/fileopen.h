#ifndef FILEOPEN_H
#define FILEOPEN_H

#include <QObject>
#include <QMdiArea>
#include <QVector>

#include "Widgets/imageviewer.h"
#include "textedit.h"
#include "buffer.h"
#include "hexedit.h"
class TextEdit;

#include "Style/appearance.h"
class Appearance;

class FileOpen : public QObject
{
    Q_OBJECT
public:
    explicit FileOpen(QObject *parent = nullptr);

    void setFilePath(const QString &filePath, bool openInHex);

    void setMdiArea(QMdiArea *value);

    void setAppearance(Appearance *value);

    QVector<Buffer *> getContainerWithBuffers() const;

private:
    QMdiArea *mdiArea = nullptr;
    Appearance *appearance = nullptr;

    QVector<QString> imageFormats;

    QVector<Buffer*> bufferPtrs;

private slots:
    void deleteBuffer(Buffer*);

};

#endif // FILEOPEN_H
