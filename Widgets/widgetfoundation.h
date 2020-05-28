#ifndef WIDGETFOUNDATION_H
#define WIDGETFOUNDATION_H

#include <QWidget>
#include <QSplitter>
#include <QVector>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

#include "Widgets/workspacesplitter.h"

/*
 * Этот класс что-то типо обертки для пользовательского виджета.
 */

class WidgetFoundation : public QWidget
{
    Q_OBJECT

private:
    // Указатель на вектор из класса WorkspaceSplitter
    QVector<QWidget*> *pWidgets;

public:
    WidgetFoundation(QWidget *parent = nullptr);

    // Сюда присваиваем пользовательский виджет
    void setWidget(QWidget *widget);

    // Сюда присваиваем указатель на вектор из класса WorkspaceSplitter
    void setPWidgets(QVector<QWidget *> *value);

    // А здесь сам указатель на класс WorkspaceSplitter
    void setWorkspaceSplitter(WorkspaceSplitter *value);

    QWidget *getWidget();

private:
    QPushButton *btnClose;
    QVBoxLayout *mainLayout;
    WorkspaceSplitter *workspaceSplitter;

private slots:
    void on_btnClose_clicked();
};

#endif // WIDGETFOUNDATION_H
