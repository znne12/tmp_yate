#include "appearance.h"
#include "ui_appearance.h"


#include "textedit.h"

Appearance::Appearance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Appearance)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    QDir().mkdir(STYLES_PATH);

    m_window = Q_NULLPTR;
    settings = Q_NULLPTR;
    mdiArea = Q_NULLPTR;
    menuBar = Q_NULLPTR;

    highlighterStyle.collectQuotationColor = QBrush(QColor("#2ab519"));
    highlighterStyle.collectFunctionColor = QBrush(QColor("#248add"));
    highlighterStyle.collectLineCommentColor = QBrush(QColor("#4d4c4c"));
    highlighterStyle.collectBlockCommentColor = QBrush(QColor("#4d4c4c"));
    highlighterStyle.collectKeywordColor = QBrush(QColor("#ab47eb"));

    templateStyle = new TemplateStyle(this);

    strBtnNewColorStyle = "QPushButton {"
                          "  background-color: %1;"
                          "  border: 1px solid %1;"
                          "}"
                          ""
                          "QPushButton:!enabled{"
                          "  background-color: gray;"
                          "  border: 1px solid gray;"
                          "}";
    ui->btnNewColor->setStyleSheet(strBtnNewColorStyle.arg(QColor(0, 0, 0).name()));

    QString defaultFamily = QFont().defaultFamily();
    ui->fontComboBox->setFont(defaultFamily);
    ui->fontCbSyntaxStyle->setFont(defaultFamily);

    syntaxFontSize = ui->cbFontSyntaxSize->currentText().toInt();

    enableCustomMode(false);

    loadData();

}

Appearance::~Appearance()
{
    delete ui;
}

void Appearance::setWindow(MainWindow *window)
{
    m_window = window;
}

void Appearance::setSettings(SettingsLoader *value)
{
    settings = value;
}

void Appearance::setStyleName(const QString &value)
{
    styleName = value;
    ui->cbUIStyles->setCurrentText(value);

    currentCbUIStyleIndex = ui->cbUIStyles->currentIndex();

    if (styleName == "Light style")
    {
        cssStyle = templateStyle->getDefaultStyleSheet(DS::LIGHT_STYLE);
        enableCustomMode(false);
    }
    else if (styleName == "Dark style")
    {
        cssStyle = templateStyle->getDefaultStyleSheet(DS::DARK_STYLE);
        enableCustomMode(false);
    }
    else
    {
        enableCustomMode(true);

        ui->btnNewColor->setStyleSheet(strBtnNewColorStyle.arg(stStylesArray.getMainColor(currentCbUIStyleIndex).name()));


        templateStyle->setMainColor(stStylesArray.getMainColor(currentCbUIStyleIndex));
        templateStyle->setFontSize(stStylesArray.getFontSize(currentCbUIStyleIndex));



        if (stStylesArray.getDarkStyleChecked(currentCbUIStyleIndex) == true)
            cssStyle = templateStyle->getStyleSheet(DS::DARK_STYLE);
        else
            cssStyle = templateStyle->getStyleSheet(DS::LIGHT_STYLE);

    }

    m_window->setStyleSheet(cssStyle);

}

void Appearance::setMdiArea(QMdiArea *value)
{
    mdiArea = value;
}

void Appearance::setFontFamily(const QString &value)
{
    fontFamily = value;
    ui->fontComboBox->setCurrentFont(value);
}


void Appearance::on_cbUIStyles_activated(const QString &arg1)
{
    setStyleName(arg1);
}

void Appearance::on_btnAddNewStyle_clicked()
{
    QString styleName = ui->lineStyleName->text();
    if (styleName.isEmpty())
        return;

    QColor mainColor = QColor(66, 214, 3);
    int checkBoxStatus = ui->chDarkStyle->isChecked();
    int fontSize = ui->cbFontSize->currentText().toInt();

    stStylesArray.addInfo(styleName,
                          mainColor,
                          checkBoxStatus,
                          fontSize,
                          0);

    ui->cbUIStyles->addItem(styleName);
    ui->cbUIStyles->setCurrentText(styleName);
    ui->lineStyleName->clear();

    enableCustomMode(true);

    setStyleName(styleName);
}

void Appearance::on_btnRemoveStyle_clicked()
{
    stStylesArray.remove(currentCbUIStyleIndex);
    ui->cbUIStyles->removeItem(currentCbUIStyleIndex);

    styleName = ui->cbUIStyles->currentText();
    setStyleName(styleName);
}

void Appearance::enableCustomMode(const bool status)
{
    ui->groupBox->setEnabled(status);
    ui->btnEditStyle->setEnabled(status);
    ui->btnRefresh->setEnabled(status);
    ui->btnRemoveStyle->setEnabled(status);
}

QString Appearance::readCssFile(const QString &path)
{
    QString t = "";
    QFile file (path);
    if (file.open(QIODevice::ReadOnly))
        t = file.readAll();
    file.close();
    return t;
}

void Appearance::loadData()
{
    QFile binFile(STYLES_PATH + DATA_FILE);
    if (binFile.open(QIODevice::ReadOnly))
    {
        QDataStream data(&binFile);
        data >> *stStylesArray.styleNames >> *stStylesArray.mainColors >>
                *stStylesArray.darkStyleChecked >> *stStylesArray.fontSizes >>
                *stStylesArray.importedFiles >> syntaxFontSize;
    }
    binFile.close();

    for (int i = 0; i < stStylesArray.styleNames->size(); i++)
    {
        ui->cbUIStyles->addItem(stStylesArray.styleNames->at(i));
    }

    ui->cbFontSyntaxSize->setCurrentText(QString::number(syntaxFontSize));
}

void Appearance::saveData()
{
    QFile binFile(STYLES_PATH + DATA_FILE);
    if (binFile.open(QIODevice::WriteOnly))
    {
        QDataStream data(&binFile);
        data << *stStylesArray.styleNames << *stStylesArray.mainColors <<
                *stStylesArray.darkStyleChecked << *stStylesArray.fontSizes <<
                *stStylesArray.importedFiles << syntaxFontSize;
    }
    binFile.close();


    for (int i = 0; i < ui->cbUIStyles->count() - DEFAULT_STYLES; i++)
    {
        QFile cssFile(STYLES_PATH + stStylesArray.styleNames->at(i) + ".css");
        if (cssFile.open(QIODevice::WriteOnly))
        {

            templateStyle->setMainColor(stStylesArray.mainColors->at(i));
            templateStyle->setFontSize(stStylesArray.fontSizes->at(i));
            templateStyle->setFontFamily(ui->fontComboBox->currentText());
            templateStyle->setFontSizeSyntax(ui->cbFontSyntaxSize->currentText().toInt());
            templateStyle->setFontFamilySyntax(ui->fontCbSyntaxStyle->currentText());


            if (stStylesArray.darkStyleChecked->at(i) == true)
                cssStyle = templateStyle->getStyleSheet(DS::DARK_STYLE);
            else
                cssStyle = templateStyle->getStyleSheet(DS::LIGHT_STYLE);

            QTextStream arr(&cssFile);
            arr << cssStyle;

        }
        cssFile.close();
    }

}


void Appearance::on_chDarkStyle_clicked(bool checked)
{
    stStylesArray.setDarkStyleChecked(currentCbUIStyleIndex, checked);
    setStyleName(styleName);
}

void Appearance::on_btnNewColor_clicked()
{
    QColorDialog dlg;
    QColor tempColor = dlg.getColor();

    if (!tempColor.isValid())
        return;

    ui->btnNewColor->setStyleSheet(strBtnNewColorStyle.arg(tempColor.name()));
    stStylesArray.setMainColor(currentCbUIStyleIndex, tempColor);
    setStyleName(styleName);
}

void Appearance::on_cbFontSize_activated(const QString &arg1)
{
    int fontSize = arg1.toInt();
    stStylesArray.setFontSize(currentCbUIStyleIndex, fontSize);
    setStyleName(styleName);
}

void Appearance::on_fontComboBox_activated(const QString &arg1)
{
    templateStyle->setFontFamily(arg1);
    setStyleName(styleName);
}

void Appearance::on_btnEditStyle_clicked()
{
    QFile file(STYLES_PATH + styleName + ".css");
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream arr(&file);
        arr << cssStyle;
    }
    file.close();

    menuBar->openFileByPath(STYLES_PATH + styleName + ".css");

    enableCustomMode(false);
    ui->btnEditStyle->setEnabled(true);
    ui->btnRefresh->setEnabled(true);
    ui->btnRemoveStyle->setEnabled(true);
}

void Appearance::setMenuBar(MenuBar *value)
{
    menuBar = value;
}

void Appearance::on_btnRefresh_clicked()
{
    cssStyle = readCssFile(STYLES_PATH + styleName + ".css");
    m_window->setStyleSheet(cssStyle);
}

void Appearance::on_btnImport_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Open css file.", "", "CSS (*.css)");

    if (path.isEmpty())
        return;

    QFile file (path);
    if (file.open(QIODevice::ReadOnly))
        cssStyle = file.readAll();
    file.close();

    QString onlyFileName = QFileInfo(file).fileName();

    QFile::copy(path, STYLES_PATH + onlyFileName);

    onlyFileName.remove(onlyFileName.size() - 4);

    ui->cbUIStyles->addItem(onlyFileName);
    ui->cbUIStyles->setCurrentText(onlyFileName);
    stStylesArray.addInfo(onlyFileName, QColor(0, 0, 0), -1, -1, 1);

    m_window->setStyleSheet(cssStyle);
}

void Appearance::on_cbSyntaxStyle_activated(const QString &arg1)
{
    syntaxTheme = arg1;

    if (arg1 == "Light style")
    {
        templateStyle->setBackgroundSyntaxColor(QColor(Qt::white));
        templateStyle->setTextSyntaxColor(QColor(Qt::black));
        templateStyle->setFontFamilySyntax(ui->fontCbSyntaxStyle->currentText());
        templateStyle->setFontSizeSyntax(ui->cbFontSyntaxSize->currentText().toInt());   

        setStyleName(styleName);
    }
    else if (arg1 == "Dark style")
    {
        templateStyle->setBackgroundSyntaxColor(QColor("#1c1d1c"));
        templateStyle->setTextSyntaxColor(QColor(Qt::white));
        templateStyle->setFontFamilySyntax(ui->fontCbSyntaxStyle->currentText());
        templateStyle->setFontSizeSyntax(ui->cbFontSyntaxSize->currentText().toInt());

        setStyleName(styleName);
    }


}

void Appearance::on_fontCbSyntaxStyle_activated(const QString &arg1)
{
    syntaxFontFamily = arg1;
    templateStyle->setFontFamilySyntax(ui->fontCbSyntaxStyle->currentText());
    setStyleName(styleName);
}

void Appearance::on_cbFontSyntaxSize_activated(const QString &arg1)
{
    syntaxFontSize = arg1.toInt();
    templateStyle->setFontSizeSyntax(syntaxFontSize);
    setStyleName(styleName);
}

QString Appearance::getSyntaxFontFamily() const
{
    return syntaxFontFamily;
}

QString Appearance::getSyntaxTheme() const
{
    return syntaxTheme;
}

QString Appearance::getFontFamily() const
{
    return fontFamily;
}

QString Appearance::getStyleName() const
{
    return styleName;
}

void Appearance::setSyntaxFontFamily(const QString &value)
{
    syntaxFontFamily = value;
    ui->fontCbSyntaxStyle->setCurrentFont(QFont(value));
}

void Appearance::setSyntaxTheme(const QString &value)
{
    syntaxTheme = value;
    ui->cbSyntaxStyle->setCurrentText(syntaxTheme);

    on_cbSyntaxStyle_activated(syntaxTheme);
}
