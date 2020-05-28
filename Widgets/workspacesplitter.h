#ifndef WORKSPACESPLITTER_H
#define WORKSPACESPLITTER_H


#include <QWidget>
#include <QSplitter>
#include <QVector>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>




class WorkspaceSplitter : public QSplitter
{
    Q_OBJECT

private:
    // Здесь хранятся указатели на WidgetFoundation
    QVector<QWidget*> *pWidgets;

public:
    explicit WorkspaceSplitter(Qt::Orientation orientation);

    void addNewWidget(QWidget *widget);


};


#endif // WORKSPACESPLITTER_H
