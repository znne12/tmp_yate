#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QColorDialog>
#include <QComboBox>


#include "mainwindow.h"
class MainWindow;

#include "settingsloader.h"
class SettingsLoader;

#include "template_style.h"
class TemplateStyle;

#include "MenuBar/menubar.h"
class MenuBar;

namespace Ui {
class Appearance;
}

/*
 * Это диалоговое окно отвечает за внешний вид приложения.
 *
 * Автор: Савелий Никулин
 * GitLab/GitHub: @SteveMoore18
 */

#define DEFAULT_STYLES 2

class Appearance : public QDialog
{
    Q_OBJECT

    typedef TemplateStyle::DefaultStyles DS;

private:
    const QString STYLES_PATH = "User Styles/";
    const int DEFAULT_STYLES_COUNT = 2;
    const QString DATA_FILE = "data.bin";
    //const int DEFAULT_STYLES = 2;

public:
    explicit Appearance(QWidget *parent = nullptr);
    ~Appearance();

    void setWindow(MainWindow *window);

    void setSettings(SettingsLoader *value);

    void setStyleName(const QString &value);

    void setMdiArea(QMdiArea *value);

    void setFontFamily(const QString &value);

    void setMenuBar(MenuBar *value);

    void setSyntaxTheme(const QString &value);

    void setSyntaxFontFamily(const QString &value);

private slots:
    void on_cbUIStyles_activated(const QString &arg1);

    void on_btnAddNewStyle_clicked();

    void on_btnRemoveStyle_clicked();

    void on_chDarkStyle_clicked(bool checked);

    void on_btnNewColor_clicked();

    void on_cbFontSize_activated(const QString &arg1);

    void on_fontComboBox_activated(const QString &arg1);

    void on_btnEditStyle_clicked();

    void on_btnRefresh_clicked();

    void on_btnImport_clicked();

    void on_cbSyntaxStyle_activated(const QString &arg1);

    void on_fontCbSyntaxStyle_activated(const QString &arg1);

    void on_cbFontSyntaxSize_activated(const QString &arg1);

private:
    Ui::Appearance *ui;

    struct StylesArray
    {

    public:
        QVector<QString> *styleNames;
        QVector<QColor> *mainColors;
        QVector<int> *darkStyleChecked;
        QVector<int> *fontSizes;
        QVector<int> *importedFiles;


        StylesArray()
        {
            styleNames = new QVector<QString>();
            mainColors = new QVector<QColor>();
            darkStyleChecked = new QVector<int>();
            fontSizes = new QVector<int>();
            importedFiles = new QVector<int>();

        }

        void addInfo(const QString &styleName,
                     const QColor &mainColor,
                     const int checkBoxChecked,
                     const int fontSize,
                     const int importedFile)
        {
            styleNames->push_back(styleName);
            mainColors->push_back(mainColor);
            darkStyleChecked->push_back(checkBoxChecked);
            fontSizes->push_back(fontSize);
            importedFiles->push_back(importedFile);
        }

        QString getName(int index)
        {
            return styleNames->at(index - DEFAULT_STYLES);
        }

        QColor getMainColor(int index)
        {
            return mainColors->at(index - DEFAULT_STYLES);
        }

        int getDarkStyleChecked(int index)
        {
            return darkStyleChecked->at(index - DEFAULT_STYLES);
        }

        int getFontSize(int index)
        {
            return fontSizes->at(index - DEFAULT_STYLES);
        }

        int getImportedFile(int index)
        {
            return importedFiles->at(index - DEFAULT_STYLES);
        }

        void setImportedFile(int index, int value)
        {
            importedFiles->replace(index, value);
        }

        void setDarkStyleChecked(int index, int value)
        {
            darkStyleChecked->replace(index - DEFAULT_STYLES, value);
        }

        void setMainColor(int index, const QColor &mainColor)
        {
            mainColors->replace(index - DEFAULT_STYLES, mainColor);
        }

        void setFontSize(int index, const int value)
        {
            fontSizes->replace(index - DEFAULT_STYLES, value);
        }

        void remove(int index)
        {
            styleNames->remove(index - DEFAULT_STYLES);
            mainColors->remove(index - DEFAULT_STYLES);
            darkStyleChecked->remove(index - DEFAULT_STYLES);
            fontSizes->remove(index - DEFAULT_STYLES);
            importedFiles->remove(index - DEFAULT_STYLES);
        }
        QVector<QString> *getStyleNames() const;
    };
    StylesArray stStylesArray;

    struct HighlighterStyle
    {
        QBrush collectQuotationColor;
        QBrush collectFunctionColor;
        QBrush collectLineCommentColor;
        QBrush collectBlockCommentColor;
        QBrush collectKeywordColor;
    };


    TemplateStyle *templateStyle;
    MainWindow *m_window;
    MenuBar *menuBar;
    SettingsLoader *settings;
    QMdiArea *mdiArea;
    QString fontFamily;

    QString strBtnNewColorStyle;

    QString styleName;
    QString cssStyle;
    int currentCbUIStyleIndex = 0;

    QString syntaxTheme;
    QString syntaxFontFamily;
    int syntaxFontSize = 0;

private:
    void enableCustomMode(const bool status);
    QString readCssFile(const QString &path);

    void loadData();


public:
    HighlighterStyle highlighterStyle;

    // QWidget interface
    QString getStyleName() const;

    QString getFontFamily() const;

    QString getSyntaxTheme() const;

    QString getSyntaxFontFamily() const;

    void saveData();

};

#endif // APPEARANCE_H
