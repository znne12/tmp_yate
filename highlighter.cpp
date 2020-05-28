#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent, const QString &suffix)
    : QSyntaxHighlighter(parent)
{
    setSuffix(suffix);
}

Highlighter::Highlighter(QTextDocument *parent, Appearance *appearance)
    : QSyntaxHighlighter(parent)
{
    this->appearance = appearance;
    setSuffix("");
}

QString Highlighter::getSuffix()
{
    return m_suffix;
}

void Highlighter::setSuffix(const QString &suffix)
{
    m_suffix = suffix;

    //using default settings location (compile this file as a resource, or make it read-only)
    defaultSettings = getHighlightningSettings(":/highlightning/" + m_suffix + ".json");

    // loading user settings in case of user has his own settins file (preferrably use this)
    QString userPath = getValueFromSettings(defaultSettings, "userSettings").toString();
    userSettings = getHighlightningSettings(userPath);

    collectHighlightingRules();
    rehighlight();
}

QJsonObject Highlighter::getHighlightningSettings(const QString &path)
{
    QFile file(path);
    QString set;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        set = file.readAll();
    }
    return QJsonDocument::fromJson(set.toUtf8()).object();;
}

QJsonValue Highlighter::getValue(QString section)
{
    QJsonValue userDefined = getValueFromSettings(userSettings, section);
    return (userDefined.isUndefined()) ? getValueFromSettings(defaultSettings, section) : userDefined;
}

QJsonValue Highlighter::getValueFromSettings(QJsonObject settings, QString section)
{
    // TODO:
    // Если переписать эту функцию так, чтобы она принимала множество параметров,
    // Здесь мы можем в цикле запрашивать JsonObject'ы любой глубины, пока не дойдём до последнего параметра,
    // в этом случае, предполагается, что последний параметр - это и есть название свойства, которое нужно достать
    return settings.value(section);
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    highlightSurroudingPairs(text);
}

void Highlighter::highlightSurroudingPairs(const QString &text)
{
    // TODO: create a common function based on the following code for surrunding pairs
    for (const SurroundingPair &pair : qAsConst(surroundingPairs)) {
        setCurrentBlockState(0);

        int startIndex = 0;
        if (previousBlockState() != 1)
           startIndex = text.indexOf(pair.left);

        while (startIndex >= 0) {
            QRegularExpressionMatch match = pair.right.match(text, startIndex);
            int endIndex = match.capturedStart();
            int commentLength = 0;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex
                        + match.capturedLength();
            }
            setFormat(startIndex, commentLength, pair.format);
            startIndex = text.indexOf(pair.left, startIndex + commentLength);
        }
    }
}

void Highlighter::setAppearance(Appearance *value)
{
    appearance = value;
}

void Highlighter::collectHighlightingRules()
{
    //    TODO: additional json file for font's foreground and weight
    collectQuotationPattern();
    collectFunctionPattern();
    collectLineCommentPattern();
    collectBlockCommentPattern();
    collectKeywordPattern();
}

void Highlighter::collectQuotationPattern()
{
    QString pattern("\".*\"");
    if (pattern.isEmpty())
        return;

    HighlightingRule rule;

    QTextCharFormat format;
    format.setForeground(appearance->highlighterStyle.collectQuotationColor);

    rule.pattern = QRegularExpression(pattern);
    rule.format = format;

    highlightingRules.append(rule);
}

void Highlighter::collectFunctionPattern()
{
    QString pattern("\\b[A-Za-z0-9_]+(?=\\()");
    if (pattern.isEmpty())
        return;

    HighlightingRule rule;

    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(appearance->highlighterStyle.collectFunctionColor);
    format.setFontWeight(QFont::Bold);

    rule.pattern = QRegularExpression(pattern);
    rule.format = format;

    highlightingRules.append(rule);
}

void Highlighter::collectLineCommentPattern()
{
    QJsonObject jo = getValue("comment").toObject();
    if (jo.isEmpty())
        return;

    QString pattern = jo.value("lineComment").toString();
    if (pattern.isEmpty())
        return;

    HighlightingRule rule;

    QTextCharFormat format;
    format.setForeground(appearance->highlighterStyle.collectLineCommentColor);

    rule.pattern = QRegularExpression(pattern + "[^\n]*");
    rule.format = format;

    highlightingRules.append(rule);
}

void Highlighter::collectBlockCommentPattern()
{
    QJsonObject jo = getValue("comment").toObject();
    if (jo.isEmpty())
        return;

    QJsonArray ja = jo.value("blockComment").toArray();
    if (ja.isEmpty())
        return;

    QString blockCommentStartPattern = ja.at(0).toString();
    QString blockCommentEndPattern = ja.at(1).toString();
    if(blockCommentStartPattern.isEmpty()
            || blockCommentEndPattern.isEmpty())
        return;

    SurroundingPair pair;

    QTextCharFormat format;
    format.setForeground(appearance->highlighterStyle.collectBlockCommentColor);

    pair.left = QRegularExpression(blockCommentStartPattern);
    pair.right = QRegularExpression(blockCommentEndPattern);
    pair.format = format;

    surroundingPairs.append(pair);
}

void Highlighter::collectKeywordPattern()
{
    QJsonArray ja = getValue("keywordPattern").toArray();
    if (ja.isEmpty())
        return;

    HighlightingRule rule;

    QString pattern;

    QTextCharFormat format;
    format.setForeground(appearance->highlighterStyle.collectKeywordColor);
    format.setFontWeight(QFont::Bold);

    QJsonArray::const_iterator it = ja.constBegin();
    while (it++ != ja.constEnd())
    {
        pattern = it->toString();
        if (!pattern.isEmpty())
        {
            rule.pattern = QRegularExpression(pattern);
            rule.format = format;

            highlightingRules.append(rule);
        }
    }
}
