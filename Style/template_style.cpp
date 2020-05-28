#include "template_style.h"

TemplateStyle::TemplateStyle(QObject *parent)
{


    stStyle.tabHeight = 40;
    stStyle.mainColor = QColor(66, 214, 3);
    stStyle.menuBackground = QColor(224, 224, 224);
    stStyle.menuBackgroundSelected = QColor(236, 236, 236);
    stStyle.arrowDownImage = ":/images/images/arrow-down.png";
    stStyle.checkMarkImage = ":/images/images/check-mark.png";
    stStyle.dotImage = ":/images/images/dot.png";
    stStyle.fontSize = 14;
    stStyle.fontFamily = "";

    stSyntaxStyle.backgroundColor = QColor(Qt::white);
    stSyntaxStyle.textColor = QColor(Qt::black);
    stSyntaxStyle.fontFamily = "";
    stSyntaxStyle.fontSize = 14;


    QFile file(":/styles/template_style.css");
    if (file.open(QIODevice::ReadOnly))
    {
        templateStyle = file.readAll();
    }
    file.close();
}

QString TemplateStyle::getStyleSheet(DefaultStyles style)
{


    switch (style)
    {
    case DefaultStyles::LIGHT_STYLE:
        stStyle.backgroundColor = QColor(Qt::white);
        stStyle.textColor = QColor(Qt::black);
        stStyle.tabBackgroundColor = QColor(192, 192, 192);
        break;
    case DefaultStyles::DARK_STYLE:
        stStyle.backgroundColor = QColor("#1c1d1c");
        stStyle.textColor = QColor(Qt::white);
        stStyle.tabBackgroundColor = QColor(9, 10, 9);
        break;
    }

    cssStyle = templateStyle
            .arg(stStyle.tabHeight)
            .arg(stStyle.backgroundColor.name())
            .arg(stStyle.textColor.name())
            .arg(stStyle.tabBackgroundColor.name())
            .arg(stStyle.mainColor.name())
            .arg(stStyle.menuBackground.name())
            .arg(stStyle.menuBackgroundSelected.name())
            .arg(stStyle.arrowDownImage)
            .arg(stStyle.checkMarkImage)
            .arg(stStyle.dotImage)
            .arg(stStyle.fontSize)
            .arg(stStyle.fontFamily)
            .arg(stSyntaxStyle.backgroundColor.name())
            .arg(stSyntaxStyle.textColor.name())
            .arg(stSyntaxStyle.fontFamily)
            .arg(stSyntaxStyle.fontSize);

    return cssStyle;

    /*
     * %1 - Tab height
     * %2 - ui background color
     * %3 - ui text color
     * %4 - unselected tab background color
     * %5 - Main color (default lightgreen)
     * %6 - Menu background color
     * %7 - Menu background selected
     * %8 - Arrow down for combobox
     * %9 - check mark for checkbox
     * %10 - dot image for radiobutton
     * %11 - ui font size
     * %12 - ui font family
     *
     */


}

QString TemplateStyle::getDefaultStyleSheet(TemplateStyle::DefaultStyles style)
{
    stStyle.mainColor = QColor(QColor(66, 214, 3));

    return getStyleSheet(style);

}

QString TemplateStyle::getStyle() const
{
    return cssStyle;
}

void TemplateStyle::setFontSize(int fontSize)
{
    stStyle.fontSize = fontSize;
}

void TemplateStyle::setFontFamily(const QString &fontFamily)
{
    stStyle.fontFamily = fontFamily;
}

void TemplateStyle::setMainColor(const QColor &color)
{
    stStyle.mainColor = color;
}

void TemplateStyle::setBackgroundSyntaxColor(const QColor &color)
{
    stSyntaxStyle.backgroundColor = color;
}

void TemplateStyle::setTextSyntaxColor(const QColor &color)
{
    stSyntaxStyle.textColor = color;
}

void TemplateStyle::setFontFamilySyntax(const QString &fontFamily)
{
    stSyntaxStyle.fontFamily = fontFamily;
}

void TemplateStyle::setFontSizeSyntax(const int fontSize)
{
    stSyntaxStyle.fontSize = fontSize;
}


