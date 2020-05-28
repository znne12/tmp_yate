#include "console.h"

Console::Console(QWidget *parent) : QPlainTextEdit(parent)
{

    m_process = new QProcess(this);
    clipboard = QApplication::clipboard();
    cursor = QTextCursor(this->textCursor());

    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(outConsole()));
    connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(outErrortoConsole()));

    OC = QSysInfo::productType();
    if (OC == "windows")
    {
        m_process->start("cmd /S");
        userName = "";
    }
    else
    {
        m_process->start("sh");
        userName = QSysInfo::machineHostName();

        insertPlainText(userName + "$");
    }

    this->setFont(QFont("Lucida Console"));
    this->setCursorWidth(CURSOR_WIDTH);

    QString style = "QPlainTextEdit {"
                    "   background-color: black;"
                    "   color: white;"
                    "   font-size: 16px;"
                    "   border: 1px solid black;"
                    "}";

    this->setStyleSheet(style);

}

Console::~Console()
{
    m_process->close();
}

void Console::on_keyEnter_clicked()
{
    insertPlainText("\n");

    QByteArray arr = userText.toUtf8() + '\n';
    m_process->write(arr);

    userText = "";
}

void Console::on_keyBackspace_clicked()
{
    if (!userText.isEmpty())
    {

        if (this->textCursor().selectedText().size() > 0)
        {
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            this->setTextCursor(cursor);
        }

        int pos = userText.size();
        userText = userText.remove(pos - 1, 1);

        cursor.deletePreviousChar();

    }
}

void Console::moveCursor(QTextCursor::MoveOperation operation)
{
    cursor.movePosition(operation, QTextCursor::MoveAnchor, 1);
    this->setTextCursor(cursor);
}

void Console::outConsole()
{

    QByteArray output = m_process->readAllStandardOutput();

    if(OC == "windows")
    {
        QTextCodec *codec = QTextCodec::codecForName("IBM 866");
        insertPlainText(codec->toUnicode(output));
    }
    else
    {
        insertPlainText(output);
        insertPlainText(userName + "$");
    }

    verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->maximum());


    userText = "";
}

void Console::outErrortoConsole()
{
    QByteArray output = m_process->readAllStandardError();

    if(OC == "windows")
    {
        QTextCodec *codec = QTextCodec::codecForName("IBM 866");
        appendHtml(QString("<font color='red'>%1</font>").arg(codec->toUnicode(output)));
    }
    else
    {
        insertPlainText(output);
        insertPlainText(userName + "$");
    }



    userText = "";


}

void Console::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
        on_keyEnter_clicked();
        break;
    case Qt::Key_Backspace:
        on_keyBackspace_clicked();
        break;
    case Qt::Key_Left:
        moveCursor(QTextCursor::Left);
        break;
    case Qt::Key_Right:
        moveCursor(QTextCursor::Right);
        break;
    case Qt::Key_Up:
        moveCursor(QTextCursor::Up);
        break;
    case Qt::Key_Down:
        moveCursor(QTextCursor::Down);
        break;
    default:

        if (event->matches(QKeySequence::Copy))
        {
            QString selectedText = this->textCursor().selectedText();
            clipboard->setText(selectedText);
        }
        else if (event->matches(QKeySequence::Paste))
        {
            insertPlainText(clipboard->text());
        }
        else
        {
            if (event->key() == Qt::Key_Control)
                return;

            if (!(this->textCursor().position() < this->toPlainText().size()))
            {
                userText += event->text();
                insertPlainText(event->text());
            }
        }

    }



}
