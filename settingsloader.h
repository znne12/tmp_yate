/*
Класс предназначен для чтения/записи переменных в файлы формата JSON
Для работы с классом необходимо объявить указатель типа SettingsLoader и инициализировать его значением статической
функции SettingsLoader::getInstance(QString). В качестве аргумента передать имя существующего файла формата JSON.

В процессе инициализации происходит проверка и чтение данных из файла JSON в объект QJsonObject для работы с данными.
Если JSON-файл невозможно открыть на чтение, генерируется исключение std::invalid_argument со значением <имя json-файла>.
Если данные из файла невозможно прочитать, генерируется исключение QJsonParseError. В случае генерации любого из
исключений, инициализации объявленного объекта не происходит.
при наличии возможности чтения данных из файла, функция SettingsLoader::getInstance(QString) создаёт элемент по имени
файла в статическом контейнере QMap, присваивает ему значение указателя на созданный объект SettingsLoader и возвращает
указатель на этот объект.

Для получения значения ключа, элемента массива или всего массива используется перегруженная функция:
    QJsonValue getValue(QString property);                  вариант для уникального имени ключа или массива
    QJsonValue getValue(QStringList jsonNameList);          вариант для неуникальных имен ключей

В случае использования метода для уникального имени ключа, класс находит первый попавшийся ключ с запрашиваемым
именем и возвращает его значение. Поиск прекращается.

Примеры использования:
  Аргументами функции могут быть уникальные имена ключей, массивов и элементы массивов.
    getValue("position-x").toInt() - получение значения ключа "position-x"
    getValue("array1[]").toArray(); - получение массива с именем "array1"
    getValue("array1[2]").toInt(); - получение значения 2-го элемента массива "array1"

  Аргументами функции в виде элементов QStringList могут быть имена секций, имена массивов, элементы массивов, имена ключей.
    Необходимо перечислять все имена секций и массивов последовательно, согласно иерархии вложенности.
    Количество вложений секций не ограничено.
    getValue({"main-window", "position-x"}).toInt() - получение значения ключа, хранящегося в секции "main-window".
    getValue({"main-window", "array1[]"}).toArray(); - получение массива с именем "array1"
    getValue({"main-window", "array1[2]"}).toInt(); - получение значения 2-го элемента массива "array1"

Для записи значений, массивов и элементов массива используется перегруженная функция:
    (работа м методами сохранения аналогична методам получения значений)

    setValue("position-x", value);  - сохранение значения value в JSON объект c уникальным именем "position-x"
    setValue({"main-window", "position-x"}, value);  - сохранение значения value в JSON объект "position-x",
                                                       находящийся в секции "main-window"
    setValue({"main-window", "array1[2]"}, value);

Сохранение объекта JSON в файл, ассоциированный в QMap с указателем на данный объект выполняется вызовом метода
    saveJsonToFile(). Если возникает ошибка открытия на запись JSON-файла, функция генерирует исключение
    std::runtime_error со значением <имя json-файла>
*/

#ifndef SETTINGSLOADER_H
#define SETTINGSLOADER_H

#include <QJsonObject>
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMap>
#include <stdexcept>

class SettingsLoader
{
public:
    ~SettingsLoader();
    SettingsLoader(QString file_name);

    QJsonValue getValue(QString property) const;
    QJsonValue getValue(QStringList jsonNameList) const;

    bool setValue(QString property, QJsonValue value);
    bool setValue(QStringList jsonNameList, QJsonValue value);

    void saveJsonToFile() const;

    static SettingsLoader *getInstance(QString file_name);

private:
    SettingsLoader(const SettingsLoader&) = delete;
    SettingsLoader& operator=(const SettingsLoader&) = delete;

    void getDefaultSettingsFileContents();  //загрузка файла JSON из ресурсов
    void getSettingsFileContents();         //загрузка файла JSON из файловой системы

    QJsonValue getValueRecursion(const QJsonObject jsonRecursion, const QString &property, const int &index) const;
    QJsonValue getArrayRecursion(const QJsonArray jsonArray, const QString &property, const int &index, bool propertyArray) const;
    QJsonValue getValueRecursion(const QJsonObject jsonRecursion, const QStringList jsonNameList) const;

    bool setValueRecursion(QJsonObject &jsonRecursion, const QString &property, const int &index, const QJsonValue &value);
    bool setArrayRecursion(QJsonArray &jsonArray, const QString &property, const int &index, const QJsonValue &value, bool propertyArray);
    bool setValueRecursion(QJsonObject &jsonRecursion, const QStringList jsonNameList, const QJsonValue &value);

    QJsonObject jsonObject;     //объект JSON для работы с данными. Данные из файла загружаются в него
    const QString fileName;
    inline static QMap<QString, SettingsLoader*> pointerMap;    //Ассоциативный контейнер <имя файла>-<указатель на объект JSON> для всех загруженных файлов JSON
};

#endif // SETTINGSLOADER_H
