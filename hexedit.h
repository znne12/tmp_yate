#ifndef HEXEDIT_H
#define HEXEDIT_H
#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QApplication>
#include "buffer.h"
#include "Style/appearance.h"
class Appearance;


class HexEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    HexEdit(Appearance *appearance = nullptr, Buffer *buffer = nullptr);
    ~HexEdit();
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    Buffer *buffer() const;
    void setFilePath(QString filePath);
    void openFile(QString pathFileName);

protected:
    void resizeEvent(QResizeEvent *event) override;
  //  void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
 //   void mousePressEvent(QMouseEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

    void seekAndConvert(qint64 fromGlobalPos, qint64 size);
    void replaceTextBlock(QString newTextBlock, qint64 loadedFromPos, qint64 sizeLoaded);
    void scrollContent(int verticalSBvalue, bool cursorMoved = false);
    void syncScrollBars(int verticalSBvalue);
    void syncCursor();

    void showErrorMsg(const QString &err);
    void loadFile();

private:
    Buffer *m_buffer;
    Appearance *m_appearance;
    QWidget *lineNumberAreaHex;
    qint32 cursorAtBlockNum;
    QFile file;
    qint32 sizeOfFragment;          // размер выводимого фрагмента файла
    qint32 globalLinesCount;        // общее количество строк в выводимом файле
    qint64 currentLine;
    qint64 firstLineNumberLoaded;
    qint64 lastLineNumberLoaded;
    bool dontScroll;

    //QSharedPointer<SeekAndConvert>  getNewTextBlock;

    QScrollBar *newVertScrollBar;
    qint32 rowsOnScreen();



signals:
    void deleteBuffer(Buffer*);

};



class LineNumberAreaHex : public QWidget
{
public:
    LineNumberAreaHex(HexEdit *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    HexEdit *codeEditor;
};
#endif // HEXEDIT_H
