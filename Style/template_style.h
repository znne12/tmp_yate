#ifndef LIGHTSTYLE_H
#define LIGHTSTYLE_H



#include <QFile>
#include <QColor>
#include <QVector>
#include <QDebug>

// %5 - main color

class TemplateStyle : public QObject
{
    Q_OBJECT
private:
    const int MAX_GRAY_COLOR = 170;
    const int MAX_GRAY_COLOR_FOR_SLIDER = 200;

public:
    explicit TemplateStyle(QObject *parent = nullptr);

    enum DefaultStyles {
        LIGHT_STYLE,
        DARK_STYLE,
    };

public:
    QString getStyleSheet(DefaultStyles style);
    QString getDefaultStyleSheet(DefaultStyles style);

    QString getStyle() const;

    void setFontSize(int fontSize);
    void setFontFamily(const QString &fontFamily);
    void setMainColor(const QColor &color);

    void setBackgroundSyntaxColor(const QColor &color);
    void setTextSyntaxColor(const QColor &color);
    void setFontFamilySyntax(const QString &fontFamily);
    void setFontSizeSyntax(const int fontSize);

private:

    struct structUIStyle
    {
        int tabHeight;
        QColor backgroundColor;
        QColor textColor;
        QColor tabBackgroundColor;
        QColor mainColor;
        QColor menuBackground;
        QColor menuBackgroundSelected;
        QString arrowDownImage;
        QString checkMarkImage;
        QString dotImage;
        int fontSize;
        QString fontFamily;

    };
    structUIStyle stStyle;

    struct structSyntaxStyle
    {
        QColor backgroundColor;
        QColor textColor;
        QString fontFamily;
        int fontSize;
    };
    structSyntaxStyle stSyntaxStyle;

    QString templateStyle;
    QString cssStyle;


    /*QVector<QColor> defaultLightStyle;
    QVector<QColor> defaultDarkStyle;

    QString fontFamilySyntax;
    int fontSizeSyntax;
    QColor backgroundColorTextEdit;
    QColor textColorSyntax;

    QString fontFamily;

    int tabHeight;
    int fontSize;
    QColor mainColor;

    QColor backgroundColor;
    QColor textColor;
    QColor tabBackground;

    QColor menuBackground;
    QColor menuBackgroundSelected;

    QString templateStyle;
    QString cssText = "";

    QString arrowDownImage;
    const QString arrowDownLight = ":/images/images/arrow-down.png";
    const QString arrowDownDark = ":/images/images/arrow-down-dark.png";

    QString checkMarkImage;
    const QString checkMarkLight = ":/images/images/check-mark.png";
    const QString checkMarkDark = ":/images/images/check-mark-dark.png";

    QString dotImage;
    const QString dotLight = ":/images/images/dot.png";
    const QString dotDark = ":/images/images/dot-dark.png";

    QColor borderSliderColor;
    QColor comboBoxSelectionTextColor;*/
};

#endif // LIGHTSTYLE_H
