#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QDebug>
#include <QProcess>
#include <QTextCodec>
#include <QPalette>
#include <QScrollBar>
#include <QApplication>
#include <QClipboard>

class Console : public QPlainTextEdit
{
    Q_OBJECT

private:
    const int CURSOR_WIDTH = 7;

public:
    explicit Console(QWidget *parent = nullptr);
    ~Console() override;

        // Variables
private:
    QString userName = "";
    QString userText;

    QProcess *m_process;
    QClipboard *clipboard;
    QTextCursor cursor;

    QString OC;

        // Functions
private:
    void on_keyEnter_clicked();
    void on_keyBackspace_clicked();

    void moveCursor(QTextCursor::MoveOperation operation);


private slots:
    void outConsole();
    void outErrortoConsole();


    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // CONSOLE_H
