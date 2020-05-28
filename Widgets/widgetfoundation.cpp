#include "widgetfoundation.h"



WidgetFoundation::WidgetFoundation(QWidget *parent) : QWidget(parent)
{
    pWidgets = nullptr;
    workspaceSplitter = nullptr;

    // Создаем кнопку, которая будет закрывать выбранный QSplitter
    btnClose = new QPushButton("X");
    btnClose->setFixedSize(20, 20);
    connect(btnClose, &QPushButton::clicked, this, &WidgetFoundation::on_btnClose_clicked);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(btnClose, 0, Qt::AlignTop | Qt::AlignRight);

    setLayout(mainLayout);
}

void WidgetFoundation::setWidget(QWidget *widget)
{
    // Пользовательский виджет будет расширятся в зависимости от размеров QSplitter
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(widget);
}

void WidgetFoundation::on_btnClose_clicked()
{
    // Берем индекс из вектора, в котором один из элементов хранит
    // указатель на этот класс, и просто берем индекс этого элемента.
    int index = pWidgets->indexOf(this);

    // Удаляем из QSplitter соответствующий виджет
    pWidgets->remove(index);
    delete workspaceSplitter->widget(index + 1);

}

void WidgetFoundation::setPWidgets(QVector<QWidget *> *value)
{
    pWidgets = value;
}

void WidgetFoundation::setWorkspaceSplitter(WorkspaceSplitter *value)
{
    workspaceSplitter = value;
}

QWidget *WidgetFoundation::getWidget()
{
    return this;
}
