#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QVector>

#include "Style/appearance.h"
class Appearance;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0, const QString &suffix = 0);
    Highlighter(QTextDocument *parent = 0, Appearance *appearance = nullptr);

    QString getSuffix();
    void setSuffix(const QString &);

    void setAppearance(Appearance *value);

protected:
    void highlightBlock(const QString &text) override;

private:
    QJsonObject getHighlightningSettings(const QString &path);
    QJsonValue  getValue(QString section);
    QJsonValue  getValueFromSettings(QJsonObject settings, QString section);

    void collectHighlightingRules();
    void collectQuotationPattern();
    void collectFunctionPattern();
    void collectLineCommentPattern();
    void collectBlockCommentPattern();
    void collectKeywordPattern();

    void highlightSurroudingPairs(const QString &text);

    QString m_suffix;

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    struct SurroundingPair
    {
        QRegularExpression left;
        QRegularExpression right;
        QTextCharFormat format;
    };
    QVector<SurroundingPair> surroundingPairs;

    QJsonObject defaultSettings;
    QJsonObject userSettings;

    Appearance *appearance;
};

#endif // HIGHLIGHTER_H
