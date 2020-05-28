#include "imageviewer.h"

#include <QDebug>
#include <QWheelEvent>

ImageViewer::ImageViewer(QWidget *parent) : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);

    scene = new QGraphicsScene(this);
    item = new QGraphicsPixmapItem();
    item->setTransformationMode(Qt::SmoothTransformation);

    scene->addItem(item);

    setScene(scene);
}

void ImageViewer::setImagePath(const QString &path)
{
    this->filePath = path;

    QImage img(path);
    item->setPixmap(QPixmap::fromImage(img));
}

QString ImageViewer::getFilePath() const
{
    return filePath;
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{

    if (event->delta() < 0)
    {
        scale(scaleFactor, scaleFactor);
    }
    else
    {
        scale(scaleFactor / 1.4, scaleFactor / 1.4);
    }

}

void ImageViewer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left)
      rotate(90);
    else if(event->key() == Qt::Key_Right)
      rotate(-90);
}


