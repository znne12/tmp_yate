#include "hexedit.h"
#include <QDebug>


HexEdit::HexEdit(Appearance *appearance, Buffer *buffer) :  m_buffer(buffer), m_appearance(appearance)
{
    lineNumberAreaHex = new LineNumberAreaHex(this);
    sizeOfFragment = 200;  // количество строк загружаемого фрагмента
    firstLineNumberLoaded = 0;
    lastLineNumberLoaded = 0;
    currentLine = 1;
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    newVertScrollBar = new QScrollBar(this);
    newVertScrollBar->setEnabled(false);
       connect(this, &HexEdit::blockCountChanged, this, &HexEdit::updateLineNumberAreaWidth);
    connect(this, &HexEdit::updateRequest, this, &HexEdit::updateLineNumberArea);
    connect(this, &HexEdit::cursorPositionChanged, this, &HexEdit::highlightCurrentLine);

/*    Подключение сигналов со скролбаров */
    connect(newVertScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollContent(int)));
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &HexEdit::syncScrollBars);

    connect(this, &HexEdit::cursorPositionChanged, this, &HexEdit::syncCursor);
    connect(m_buffer, &Buffer::openFile, this, &HexEdit::loadFile);
       //connect(m_buffer, SIGNAL(saveFile(QString)), this, SLOT(saveFile(const QString &)));
       connect(m_buffer, &Buffer::errorWithFile, this, &HexEdit::showErrorMsg);
    setFont(QFont("Courier", 10));
    setOverwriteMode(true);
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    openFile("d:/GeekBrains/blackjack.o");
   // setReadOnly(true);
    if (!m_buffer->pathFile().isEmpty()) { m_buffer->setPathFile(m_buffer->pathFile()); }
}

HexEdit::~HexEdit()
{

}

int HexEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 50 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

Buffer *HexEdit::buffer() const
{
    return m_buffer;
}

void HexEdit::setFilePath(QString filePath)
{
    if (filePath != "") {
        openFile(filePath);
    }
}

void HexEdit::openFile(QString pathFileName)
{

    file.setFileName(pathFileName);
    if(file.open(QFile::ReadOnly | QFile::ExistingOnly)){
        globalLinesCount = file.size() / 16;
        qDebug() << "File read - ok";
        qDebug() << "Size of file: " << file.size();
        qDebug() << "Lines in file: " << globalLinesCount;
        seekAndConvert(0, sizeOfFragment);
        newVertScrollBar->setRange(0, globalLinesCount);
        newVertScrollBar->setEnabled(true);
        currentLine = 1;

    }


}

void HexEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
void HexEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberAreaHex->scroll(0, dy);
    else
        lineNumberAreaHex->update(0, rect.y(), lineNumberAreaHex->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void HexEdit::seekAndConvert(qint64 fromGlobalPos, qint64 sizeToRead)
{
   // m_buffer->getFile().seek(fromGlobalPos * 16);
   // QByteArray dataIn = m_buffer->getFile().read(sizeToRead * 16);
    //file.seek(fromGlobalPos * 16);
  // QByteArray dataIn = file.read(sizeToRead * 16);
  //  qDebug() << dataIn;
   // qDebug() << dataIn.size();

    QByteArray dataIn =  m_buffer->getTextFragment(file, fromGlobalPos, sizeToRead);
   QString out;
    for (int i = 0,k = 0; i < dataIn.size(); i += 16, ++k)
    {

        QString hexStr;
        QString ascStr;
        for (int j=0; j<16; j++)
        {
            if ((i + j) < dataIn.size())
            {
                hexStr.append(" ").append(dataIn.mid(i + j, 1).toHex());
                char ch = dataIn[i + j];
                if ((ch < 0x20) || (ch > 0x7e))
                    ch = '.';
                ascStr.append(QChar(ch));
            }
        }
        out.append(QString("%1").arg(hexStr)+  "       " + QString("%1").arg(ascStr) + "\n");

    }


    replaceTextBlock(out, fromGlobalPos, (dataIn.size() % 16 == 0 ? dataIn.size() / 16 : dataIn.size() / 16 + 1));
}

void HexEdit::replaceTextBlock(QString newTextBlock, qint64 loadedFromPos, qint64 sizeLoaded)
{
    firstLineNumberLoaded = loadedFromPos;
    lastLineNumberLoaded = loadedFromPos + sizeLoaded;
    setPlainText(newTextBlock);
    dontScroll=true;
    //verticalScrollBar()->setValue(loadedFromPos-firstLineNumberLoaded);
    verticalScrollBar()->setValue(0);
    dontScroll=false;

}

void HexEdit::scrollContent(int verticalSBvalue, bool cursorMoved)
{
//    if(verticalSBvalue % 16 != 0){
//        verticalSBvalue -= verticalSBvalue % 16;
//    }
    QString tmp;
    qDebug() << "verticalScrollBar:" + tmp.setNum(verticalSBvalue);

    if(!dontScroll){
        if((verticalSBvalue >= firstLineNumberLoaded) && (verticalSBvalue <= lastLineNumberLoaded) )
        {
            verticalScrollBar()->setValue(verticalSBvalue-firstLineNumberLoaded);
        } else {
            seekAndConvert(verticalSBvalue, sizeOfFragment);
        }
    }
    if(cursorMoved)
       {
            const QTextBlock &block = document()->findBlockByNumber(verticalScrollBar()->value());
             QTextCursor cursor(block);
             cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 0);
             setTextCursor(cursor);
             centerCursor();
             setFocus();
       }

}

void HexEdit::syncScrollBars(int verticalSBvalue)
{
    newVertScrollBar->blockSignals(true);
    newVertScrollBar->setValue(firstLineNumberLoaded+verticalSBvalue);
    QString tmp;
    qDebug() << "newVertScrollbarr:" + tmp.setNum(firstLineNumberLoaded + verticalSBvalue);
    newVertScrollBar->blockSignals(false);
}

void HexEdit::syncCursor()
{
    currentLine = textCursor().blockNumber() + firstLineNumberLoaded + 1;
}

void HexEdit::showErrorMsg(const QString &err)
{
    QMessageBox::warning(this, tr("MDI"), err);
}

void HexEdit::loadFile()
{
    globalLinesCount = (m_buffer->getFileSize() % 16 == 0) ? m_buffer->getFileSize() / 16 : m_buffer->getFileSize() / 16 + 1;
     qDebug() << "File read - ok";
     qDebug() << "Size of file: " << m_buffer->getFileSize() ;
     qDebug() << "Lines in file: " << globalLinesCount;
     seekAndConvert(0, sizeOfFragment);
     newVertScrollBar->setRange(0, globalLinesCount);
     newVertScrollBar->setEnabled(true);
     currentLine = 1;
     document()->setModified(false);
     setWindowModified(false);
     setWindowTitle(m_buffer->fileName() + "[*]");
     //connect(document(), &QTextDocument::contentsChanged,
     //        this, &HexEdit::documentWasModified);
}

qint32 HexEdit::rowsOnScreen()
{
    int result=0;
    QTextBlock block = firstVisibleBlock();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && block.isVisible() && top <= rect().bottom())
    {

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        result++;
    }
    return result;
}

void HexEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberAreaHex->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    newVertScrollBar->setGeometry(QRect(cr.width()-40, cr.top(), 20, cr.height()));
}

//void HexEdit::wheelEvent(QWheelEvent *event)
//{
//    if (event->orientation() == Qt::Vertical)
//    {
//         QApplication::sendEvent ( verticalScrollBar(),  event );
//    }
//}

void HexEdit::keyPressEvent(QKeyEvent *event)
{
        int newPos;
        QString tmp;
        switch(event->key())
        {
        case Qt::Key_PageUp :
        case QKeySequence::MoveToPreviousPage:
            newPos=qMax(0,newVertScrollBar->value()-rowsOnScreen());
            newVertScrollBar->setValue(newPos);
            scrollContent(newPos,true);
            break;
        case Qt::Key_PageDown :
        case QKeySequence::MoveToNextPage:
         //   currentLine += textCursor().blockNumber()
            newPos=currentLine+rowsOnScreen();
            newVertScrollBar->setValue(newPos);
            scrollContent(newPos,true);
            break;
        case Qt::Key_Up :
          //  currentLine--;

            qDebug() << tmp.setNum(currentLine);
            if((firstLineNumberLoaded != 0)&&(currentLine <= firstLineNumberLoaded + rowsOnScreen())){
                QTextCursor cursor(this->textCursor());
                const QTextBlock &block = firstVisibleBlock();
                seekAndConvert(currentLine - sizeOfFragment + rowsOnScreen(), sizeOfFragment);
                for (int i = 0; i < sizeOfFragment; ++i) cursor.movePosition(QTextCursor::Down);
                setTextCursor(cursor);
                for (int i = 0; i < rowsOnScreen() + 3; ++i) cursor.movePosition(QTextCursor::Up);
                setTextCursor(cursor);
                break;
            }
            QPlainTextEdit::keyPressEvent(event);
            break;
        case Qt::Key_Down :
            //currentLine++;

            qDebug() << tmp.setNum(currentLine);
            if(currentLine >= lastLineNumberLoaded - rowsOnScreen()){
                QTextCursor cursor(this->textCursor());
                const QTextBlock &block = firstVisibleBlock();
                qint32 cursorPos = cursor.position() - block.position();
                seekAndConvert(currentLine - rowsOnScreen() +1, sizeOfFragment);
             //   verticalScrollBar()->setValue(rowsOnScreen());
                cursor.setPosition(cursorPos);
                setTextCursor(cursor);
                break;
            }

        default:
            QPlainTextEdit::keyPressEvent(event);
            break;
        }
        event->accept();
}

//void HexEdit::mousePressEvent(QMouseEvent *e)
//{
//    currentLine = textCursor().blockNumber() + firstLineNumberLoaded;
//    QPlainTextEdit::mousePressEvent(e);
//}

void HexEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(180);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


void HexEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberAreaHex);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber() + firstLineNumberLoaded;
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString("%4").toUpper().arg(QString::number(blockNumber, 16), 4, QLatin1Char('0')) + "0h:";
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberAreaHex->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
