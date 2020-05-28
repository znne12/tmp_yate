#include "textedit.h"

TextEdit::TextEdit(Appearance *appearance, Buffer *buffer) : m_buffer(buffer), m_appearance(appearance)
{

    //**************** lineNumberArea *******************************
    VisibleLineNumber = false;
    lineNumberArea = new LineNumberArea(this);
    connect(this, &TextEdit::blockCountChanged, this, &TextEdit::updateLineNumberAreaWidth);
    connect(this, &TextEdit::updateRequest, this, &TextEdit::updateLineNumberArea);
    connect(this, &TextEdit::cursorPositionChanged, this, &TextEdit::highlightCurrentLine);

    updateLineNumberAreaWidth();
    highlightCurrentLine();
   //****************************************************************
    setAttribute(Qt::WA_DeleteOnClose);

    connect(m_buffer, &Buffer::createFile, this, &TextEdit::newFile);
    connect(m_buffer, &Buffer::openFile, this, &TextEdit::loadFile);
    connect(m_buffer, &Buffer::errorWithFile, this, &TextEdit::showErrorMsg);

//    setFont(SettingsLoader::getInstance()->getValue("application", "font-family"));
    if (m_buffer->pathFile().isEmpty()) { m_buffer->setPathFile(""); }
    if (!m_buffer->pathFile().isEmpty()) { m_buffer->setPathFile(m_buffer->pathFile()); }
}

TextEdit::~TextEdit()
{
    deleteBuffer(m_buffer);
    // TODO пережелать создание нового файла через fileopen
    //      сделать вектор указателей на buffers  в fileopen
    //      связать сигнал-слотами деструктор виджета и удаление buffera из вектора и вообще
    //  добавить в findMDIchild Hexeditor в менюбаре
}

Buffer *TextEdit::buffer() const
{
    return m_buffer;
}

void TextEdit::setBuffer(Buffer *buffer)
{
    m_buffer = buffer;

}

Highlighter * TextEdit::highlighter() const
{
    return m_highlighter;
}

void TextEdit::newFile()
{
    setWindowTitle(m_buffer->fileName() + "[*]");
    connect(document(), &QTextDocument::contentsChanged,
            this, &TextEdit::documentWasModified);
    m_highlighter = new Highlighter(document(), m_appearance);
}

bool TextEdit::saveFile()
{
    if (m_buffer->hasPath())
    {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        m_buffer->save(toPlainText());
        QGuiApplication::restoreOverrideCursor();
        document()->setModified(false);
        //m_highlighter->setSuffix(QFileInfo(m_buffer->fileName()).suffix());
        setWindowModified(false);
        setWindowTitle(m_buffer->fileName() + "[*]");
        return true;
    }
    else
    {
        return saveAsFile();
    }
}

void TextEdit::loadFile()
{
    m_highlighter = new Highlighter(document(), m_appearance);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(m_buffer->getTextAll());
    QGuiApplication::restoreOverrideCursor();
    document()->setModified(false);
    m_highlighter->setSuffix(QFileInfo(m_buffer->fileName()).suffix());
    setWindowModified(false);
    setWindowTitle(m_buffer->fileName() + "[*]");
    connect(document(), &QTextDocument::contentsChanged,
            this, &TextEdit::documentWasModified);
}

void TextEdit::documentWasModified()
{
    setWindowModified(document()->isModified());
}

void TextEdit::showErrorMsg(const QString &err)
{
    QMessageBox::warning(this, tr("MDI"), err);
}

bool TextEdit::maybeSave()
{
    if (!document()->isModified()){ return true; }
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("MDI"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(m_buffer->fileName()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return saveFile();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool TextEdit::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), m_buffer->pathFile(),
                                                        "Text files (*.txt);;All files (*)");
    if (fileName.isEmpty()){ return false; }
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    m_buffer->save(fileName, toPlainText());
    QGuiApplication::restoreOverrideCursor();
    document()->setModified(false);
    m_highlighter->setSuffix(QFileInfo(m_buffer->fileName()).suffix());
    setWindowModified(false);
    setWindowTitle(m_buffer->fileName() + "[*]");
    return true;
}

void TextEdit::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}


//*******************************************************************
//**************** line Number Area code***********************************
int TextEdit::lineNumberAreaWidth()
{
    int lastLineNumber = blockCount();
    int numDigitsInLastLine = QString::number(lastLineNumber).length();
    int maxWidthOfAnyDigit = fontMetrics().QFontMetrics::horizontalAdvance(QLatin1Char('9'));
   if(VisibleLineNumber)
    {

        return numDigitsInLastLine * maxWidthOfAnyDigit + lineNumberAreaPadding;
    }
   else
    {
       return 0;
    }
}

void TextEdit::updateLineNumberAreaWidth()
{
  auto val = VisibleLineNumber ? ( lineNumberAreaWidth() + lineNumberAreaPadding ): lineNumberAreaWidth();
  setViewportMargins( val, 0, 0, 0 );
// setViewportMargins(lineNumberAreaWidth() + lineNumberAreaPadding , 0, 0, 0);
}

void TextEdit::updateLineNumberArea(const QRect &rect, int numPixelsScrolledVertically)
{
    if (numPixelsScrolledVertically)
        lineNumberArea->scroll(0, numPixelsScrolledVertically);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void TextEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void TextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
       // TODO добавить стиль "текущая строка"
        QColor lineColor =  QColor(0, 0, 255, 127).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void TextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    // TODO добавить стиль "нумерация строк"
    painter.fillRect(event->rect(),Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qvariant_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qvariant_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qvariant_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
