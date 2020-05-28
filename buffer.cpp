#include "buffer.h"
#include <QFileInfo>
#include <QDebug>

Buffer::Buffer()
{
    m_pathFile = "";
}

Buffer::Buffer(const QString &fileName) : m_pathFile(fileName)
{

}

Buffer::~Buffer()
{

}

QString Buffer::pathFile() const
{
    return m_pathFile;
}

void Buffer::setPathFile(const QString &pathFile)
{
    if (pathFile.isEmpty()){
        // создание нового файла
        m_fileName = QString("untitled");
        m_hasPath = false;
        emit createFile();
        return;
    }

    if (QFileInfo(pathFile).exists()){
        m_pathFile = pathFile;
        m_fileName = QFileInfo(pathFile).fileName();
        m_hasPath = true;
        loadFile(pathFile);
    }
}

QString Buffer::fileName() const
{
    return m_fileName;
}

bool Buffer::hasPath() const
{
    return m_hasPath;
}

void Buffer::save(const QString &textToSave)
{
    save(m_pathFile, textToSave);
}

bool Buffer::save(const QString &pathFile, const QString &textToSave)
{
    m_pathFile = pathFile;
    m_fileName = QFileInfo(pathFile).fileName();
    m_hasPath = true;
    if (!file.isWritable()) { file.close(); }
    file.setFileName(pathFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
       errorWithFile(tr("Cannot write file %1:\n%2.")
                     .arg(QDir::toNativeSeparators(m_fileName), file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << textToSave;
    file.close();
    return true;
}

QString Buffer::getTextAll()
{
    return file.readAll();
}

QByteArray Buffer::getTextFragment(QFile &fl, qint64 fromGlobalPos, qint64 size)
{
    fl.seek(fromGlobalPos * 16);

    return fl.read(size * 16);;
}

bool Buffer::loadFile(const QString &fileName)
{
    file.setFileName(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        errorWithFile(tr("Cannot read file %1:\n%2.")
                      .arg(fileName)
                      .arg(file.errorString()));

        return false;
    }
    emit openFile();
    return true;
}

quint64 Buffer::getFileSize()
{
    file.setFileName(m_pathFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        errorWithFile(tr("Cannot read file %1:\n%2.")
                      .arg(m_pathFile)
                      .arg(file.errorString()));

        return false;
    }
    return (file.isOpen()) ? file.size() : 0;
}

QFile &Buffer::getFile()
{
    return file;
}

