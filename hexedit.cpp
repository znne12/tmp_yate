#include "hexedit.h"
#include <QDebug>


HexEdit::HexEdit(Appearance *appearance, Buffer *buffer) : m_buffer(buffer), m_appearance(appearance)
{
    lineNumberAreaHex = new LineNumberAreaHex(this);
    sizeOfFragment = 200;  // количество строк загружаемого фрагмента
    firstLineNumberLoaded = 0;
    lastLineNumberLoaded = 0;
    currentLine = 1;
   // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // range default скроллбара - фрагмент
    newVertScrollBar = new QScrollBar(this); // range этого нового скроллбара - весь файл
    newVertScrollBar->setEnabled(false);
    connect(this, &HexEdit::blockCountChanged, this, &HexEdit::updateLineNumberAreaWidth);
    connect(this, &HexEdit::updateRequest, this, &HexEdit::updateLineNumberArea);
    connect(this, &HexEdit::cursorPositionChanged, this, &HexEdit::highlightCurrentLine);

/*    Подключение сигналов со скролбаров */
    connect(newVertScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollContent(int)));
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &HexEdit::syncScrollBars);

    connect(this, &HexEdit::cursorPositionChanged, this, &HexEdit::syncCursor);
    setFont(QFont("Courier", 10));
    setOverwriteMode(true);
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setAttribute(Qt::WA_DeleteOnClose);

   // connect(m_buffer, &Buffer::createFile, this, &HexEdit::newFile);
    connect(m_buffer, &Buffer::openFile, this, &HexEdit::loadFile);
    //connect(m_buffer, SIGNAL(saveFile(QString)), this, SLOT(saveFile(const QString &)));
    connect(m_buffer, &Buffer::errorWithFile, this, &HexEdit::showErrorMsg);
    setReadOnly(true);
    if (!m_buffer->pathFile().isEmpty()) { m_buffer->setPathFile(m_buffer->pathFile()); }

}

HexEdit::~HexEdit()
{
    emit deleteBuffer(m_buffer);
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

void HexEdit::seekAndConvert(qint64 fromGlobalPos, qint64 size)
{
    QByteArray dataIn = m_buffer->getTextFragment(fromGlobalPos, size);
    qDebug() << "size:" << dataIn.size();
    QString result;
    for (int i = 0, k = 0; i < dataIn.size(); i += 16, ++k)
    {
        QString hexStr;
        QString ascStr;
        for (int j=0; j<16; j++)
        {
            if ((i + j) < dataIn.size())
            {
                hexStr.append(" ").append(dataIn.mid(i + j, 1).toHex());
                char ch = dataIn[i + j];
                if ((ch < 0x20) || (ch > 0x7e)){ ch = '.'; }
                ascStr.append(QChar(ch));
            }
        }
        result.append(QString("%1").arg(hexStr)+  "       " + QString("%1").arg(ascStr) + "\n");
    }
    result.chop(2);
    replaceTextBlock(result, fromGlobalPos, size);
}

void HexEdit::replaceTextBlock(const QString &newTextBlock, qint64 loadedFromPos, qint64 sizeLoaded)
{
    firstLineNumberLoaded = loadedFromPos;
    lastLineNumberLoaded = loadedFromPos + sizeLoaded;
    setPlainText(newTextBlock);
}

void HexEdit::scrollContent(int verticalSBvalue, bool cursorMoved)
{
    QString tmp;
   // qDebug() << "verticalScrollBar:" + tmp.setNum(verticalSBvalue);

    if(!dontScroll){
        if((verticalSBvalue > firstLineNumberLoaded) && (verticalSBvalue <= lastLineNumberLoaded - rowsOnScreen()) )
        {
            verticalScrollBar()->setValue(verticalSBvalue-firstLineNumberLoaded);
        } else {
            seekAndConvert(verticalSBvalue, sizeOfFragment);
        }
    }
//    if(cursorMoved)
//       {
//            const QTextBlock &block = document()->findBlockByNumber(verticalScrollBar()->value());
//             QTextCursor cursor(block);
//             cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 0);
//             setTextCursor(cursor);
//             centerCursor();
//             setFocus();
//       }

}

void HexEdit::syncScrollBars(int verticalSBvalue)
{
    newVertScrollBar->blockSignals(true);
    newVertScrollBar->setValue(firstLineNumberLoaded+verticalSBvalue);
    QString tmp;
   // qDebug() << "newVertScrollbarr:" + tmp.setNum(firstLineNumberLoaded + verticalSBvalue);
   // qDebug() << "VertSbValue:" + tmp.setNum(verticalSBvalue);

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
    newVertScrollBar->setGeometry(QRect(cr.width()-50, cr.top(), 20, cr.height()));
}

void HexEdit::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    qDebug() << "newvertSB:" << newVertScrollBar->value();
    if (!numPixels.isNull()) {
       // newVertScrollBar->setValue(newVertScrollBar->value() + numPixels);
    } else if (!numDegrees.isNull()) {
        if (numDegrees.y() < 0) { scrollContent(newVertScrollBar->value() + 1);  qDebug() << numDegrees;}
        else {
            newVertScrollBar->setValue(newVertScrollBar->value() - 1);
            scrollContent(newVertScrollBar->value());  qDebug() << numDegrees;
        }
       // QApplication::sendEvent (this,  &QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier)   );
        // QPoint numSteps = numDegrees / 15;
        //verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps.y());
    }

    event->accept();
}

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
            newPos=newVertScrollBar->value()+rowsOnScreen();
            newVertScrollBar->setValue(newPos);
            scrollContent(newPos,true);
            break;
        case Qt::Key_Up :
            if (newVertScrollBar->value() > 0 && verticalScrollBar()->value() == 0){
                //                newVertScrollBar->setValue(newVertScrollBar->value() - 1);
                //                scrollContent(newVertScrollBar->value());
                if (newVertScrollBar->value() < sizeOfFragment) {
                    //seekAndConvert(0, sizeOfFragment);
                    verticalScrollBar()->setValue(newVertScrollBar->value());
                }
                else {
                    seekAndConvert( newVertScrollBar->value() - sizeOfFragment + rowsOnScreen() - 2, sizeOfFragment);
                    verticalScrollBar()->setValue(newVertScrollBar->value());
                }
            }
            QPlainTextEdit::keyPressEvent(event);
            break;
        case Qt::Key_Down :

            if (verticalScrollBar()->value() > sizeOfFragment - rowsOnScreen() - 1){
                seekAndConvert(newVertScrollBar->value() + 1, sizeOfFragment);
            } else {
                QPlainTextEdit::keyPressEvent(event);
            }
            break;
        default:
            //QPlainTextEdit::keyPressEvent(event);
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
        QColor lineColor;
        if(m_appearance->getStyleName() == "Light style"){
            lineColor = QColor(Qt::yellow).lighter(180);
        }else if(m_appearance->getStyleName() == "Dark style"){
            lineColor = QColor(Qt::gray).lighter(80);
        }

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
            QString number = QString("%4").toUpper().arg(QString::number(blockNumber, 10), 4, QLatin1Char('0')) + "0h:";
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
