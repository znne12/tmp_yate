#include "workspacesplitter.h"
#include "widgetfoundation.h"

WorkspaceSplitter::WorkspaceSplitter(Qt::Orientation orientation)
{
    setOrientation(orientation);

    pWidgets = new QVector<QWidget*>();

}

void WorkspaceSplitter::addNewWidget(QWidget *widget)
{
    // Создаем обьект фундамент
    WidgetFoundation *wF = new WidgetFoundation();

    // Добавлем к нему: пользовательский виджет,
    // указатель на этот класс и указатель на
    // вектор pWidgets
    wF->setWidget(widget);
    wF->setWorkspaceSplitter(this);
    wF->setPWidgets(pWidgets);

    // Из-за внутренних проблем, пришлось получить только
    // указатель QWidget, а потом добавлять его в вектор.
    QWidget *w1 = wF->getWidget();
    pWidgets->push_back(w1);

    addWidget(w1);

}
