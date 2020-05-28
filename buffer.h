#ifndef BUFFER_H
#define BUFFER_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QMessageBox>

class Buffer : public QObject
{
    Q_OBJECT

public:
    Buffer();
    Buffer(const QString &fileName);
    ~Buffer();

    QString pathFile() const;
    void setPathFile(const QString &pathFile);
    QString fileName() const;
    bool hasPath() const;
    void save(const QString &textToSave);
    bool save(const QString &pathFile, const QString &textToSave);
    bool loadFile(const QString &fileName);
    QByteArray getTextFragment(QFile &fl, qint64 fromGlobalPos, qint64 size);
    QString getTextAll();
    quint64 getFileSize();
    QFile &getFile();

private:
    QString m_fileName;
    QString m_pathFile;
    bool m_hasPath;
    QFile file;

signals:
    void createFile();
    void openFile();
    void errorWithFile(QString);
};

#endif // BUFFER_H
