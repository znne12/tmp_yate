#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void setImagePath(const QString &path);

    QString getFilePath() const;

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    float scaleFactor = 1.15;

    QGraphicsScene *scene;
    QGraphicsPixmapItem *item;

    QString filePath;

};

#endif // VIEWER_H
