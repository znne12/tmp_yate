#include "settingsloader.h"

SettingsLoader::SettingsLoader(const QString file_name) : fileName(file_name)
{
    QFileInfo fileInfo(file_name);
    QFile file(":/settings/" + fileInfo.fileName());
    if (file.open(QIODevice::ReadOnly))
    {
        file.close();
        getDefaultSettingsFileContents(); //если данный файл есть в ресурсах, то загрузить настройки по умолчанию из файла ресурсов
    }
    getSettingsFileContents();
}

SettingsLoader::~SettingsLoader()
{
}

SettingsLoader* SettingsLoader::getInstance(QString file_name)
{
    bool fileResExists = false, fileFSExists = false;   //флаг наличия файла в ресурсах

    if (pointerMap[file_name] != nullptr)       //если объект существует
        return pointerMap[file_name];

    QFileInfo fileInfo(file_name);      //проверка файла в ресурсах
    QFile fileRes(":/settings/" + fileInfo.fileName());
    if (fileRes.open(QIODevice::ReadOnly))
    {
        fileRes.close();
        fileResExists = true;
    }

    QFile fileFS(file_name);        //проверка файла в файловой системе
    if (fileFS.open(QIODevice::ReadOnly | QIODevice::Text)) //если файл открывается в файловой системе
    {
        QString set = fileFS.readAll();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(set.toUtf8(), &jsonError);
        fileFS.close();
        if (!jsonDoc.isNull())                           //нет ошибок чтения данных
            fileFSExists = true;
        else
        if (!fileResExists)
            throw jsonError; //если файла нет в ресурсах и ошибка чтения JSON в файловой системе, исключение по ошибке парсинга
    }
    else if (!fileResExists)
        throw std::invalid_argument(file_name.toUtf8()); //если файла нет в ресурсах и ошибка открытия файла в файловой системе, исключение по ошибке открытия файла
    
    if (fileFSExists || fileResExists)
        pointerMap[file_name] = new SettingsLoader(file_name); //если есть файл в ресурсах или корректно считывается файл из файловой системы
    return pointerMap[file_name];
}

void SettingsLoader::getDefaultSettingsFileContents()
{
    QFileInfo fileInfo(fileName);
    QFile file(":/settings/" + fileInfo.fileName());
    if (file.open(QIODevice::ReadOnly))
    {
        QString set = file.readAll();
        jsonObject = QJsonDocument::fromJson(set.toUtf8()).object();
        file.close();
    }
}

void SettingsLoader::getSettingsFileContents()
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString set = file.readAll();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(set.toUtf8(), &jsonError);
        if (!jsonDoc.isNull())                           //нет ошибок чтения данных
            jsonObject = QJsonDocument::fromJson(set.toUtf8()).object();
        file.close();
   }
}

QJsonValue SettingsLoader::getValue(QString property) const
{
    int index = -1; //индекс, на случай получения значения из массива.
                    //"-1" - получаемое значение не из массива
                    //"-2" - получение массива целиком
    int b = property.indexOf("[");
    int e = property.indexOf("]");
    if (e - b > 1)  //запрошен элемент массива
    {
        index = property.mid(b + 1, e - b - 1).toInt(); //чтение индекса между [ ]
        property.remove(b, e - b + 1);  //удаление из property [ ]
    }
    else if (e - b == 1)
    {
        index = -2;
        property.remove(b, e - b + 1);  //удаление из property []
    }
    QJsonValue value = getValueRecursion(jsonObject, property, index);
    return  value;
}

QJsonValue SettingsLoader::getValueRecursion(const QJsonObject jsonRecursion, const QString &property, const int &index) const
{
    QJsonValue value = QJsonValue::Null;
    QJsonObject::const_iterator it;

    for (it = jsonRecursion.begin(); it != jsonRecursion.end(); ++it) //перебор содержимого объекта json
    {
        if (it->isObject())
        {
            value = getValueRecursion(it->toObject(), property, index);  //рекурентный вызов и обработка в нём секции
            if (value != QJsonValue::Null) break;
        }
        else if (it->isArray())
        {
            if (it.key() == property)
            {
                if (index > -1)
                value = getArrayRecursion(it->toArray(), property, index, true);    //имя текущего массива совпадает со значением property, вызов функции обработки массива
                else if (index == -2) value = it.value();                           //получение всего массива
            }
            else                        value = getArrayRecursion(it->toArray(), property, index, false);
            if (value != QJsonValue::Null) break;   //если было считано или отсутствует значение, выход из цикла
        }
        else if (it.key() == property && index == -1)  //если встретился ключ с именени property и property - не имя массива
        {
            value = it.value();
            break;
        }
    }
    return value;
}

QJsonValue SettingsLoader::getArrayRecursion(const QJsonArray jsonArray, const QString &property, const int &index, bool propertyArray) const
{
    QJsonValue value = QJsonValue::Null;
    if (propertyArray && index > -1) //если у текущего массива имя == property и надо получить элемент массива
    {
        value = jsonArray[index];                    //возврат значения элемента массива или QJsonValue::Undefined в случае отсутствия элемента массива
        propertyArray = false;          //после считывания значения нужного элемента массива сброс флага равенства имени массива и property

    }
    else                //у текущего массива другое имя. Необходим поиск среди его элементов
    {
        for (int i = 0; i < jsonArray.size(); ++i)
        {
            if (jsonArray[i].isObject())    //если текущий элемент массива является объектом json
                value = getValueRecursion(jsonArray[i].toObject(), property, index);    //вызов функции обработки объекта
            if (value != QJsonValue::Null) break;   //если было считано или отсутствует значение, выход из цикла
        }
    }
    return value;
}

QJsonValue SettingsLoader::getValue(QStringList jsonNameList) const
{
    return getValueRecursion(jsonObject, jsonNameList);
}
QJsonValue SettingsLoader::getValueRecursion(const QJsonObject jsonRecursion, const QStringList jsonNameList) const
{
    QJsonValue value;
    QString jsonName;
    int index = -1;
    int b = jsonNameList[0].indexOf("[");
    int e = jsonNameList[0].indexOf("]");

    if (b == -1)    //объект, не массив
    {
        jsonName = jsonNameList[0];
    }
    else if (e - b > 1)  //запрошен элемент массива
    {
        index = jsonNameList[0].mid(b + 1, e - b - 1).toInt(); //чтение индекса между [ ]
        jsonName = jsonNameList[0].mid(0, b);
    }
    else if (e - b == 1)    //массив целиком
    {
        index = -2;
        jsonName = jsonNameList[0].mid(0, b);
    }

    if (jsonNameList.size() > 1)    //не достигнут низ иерархии
    {
        if (index == -1)            //текущий элемент списка имён - не элемент массива. Преобазование его в объект для дальнейшей обработки
        {
            QJsonObject jsonSubObject = jsonRecursion.value(jsonName).toObject();
            value = getValueRecursion(jsonSubObject, jsonNameList.mid(1, jsonNameList.size() - 1));
        }
        else if (index > -1)         //текущий элемент списка имён - элемент массива. Преобазование его элемента с индексом в объект для дальнейшей обработки
        {
            QJsonObject jsonSubObject = jsonRecursion.value(jsonName).toArray()[index].toObject();  //Получение значения объекта, преобразование его в массив, получение элемента массива и преобразование его в объект
            value = getValueRecursion(jsonSubObject, jsonNameList.mid(1, jsonNameList.size() - 1));
        }
        else if (index == -2) value = QJsonValue::Undefined; //запрошен массив без конкретного индекса. Поиск вниз по иерархии невозможен
    }
    else                            //низ иерархии
    {
        if (index > -1) value = jsonRecursion.value(jsonName).toArray()[index];

        else value = jsonRecursion.value(jsonName);
    }
    return value;
}

bool SettingsLoader::setValue(QString property, QJsonValue value)
{
    int index = -1; //индекс, на случай получения значения из массива.
                    //"-1" - получаемое значение не из массива
                    //"-2" - получение массива целиком
    int b = property.indexOf("[");
    int e = property.indexOf("]");
    if (e - b > 1)  //запрошен элемент массива
    {
        index = property.mid(b + 1, e - b - 1).toInt(); //чтение индекса между [ ]
        property.remove(b, e - b + 1);  //удаление из property [ ]
    }
    else if (e - b == 1)
    {
        index = -2;
        property.remove(b, e - b + 1);  //удаление из property []
    }
    return setValueRecursion(jsonObject, property, index, value);
}

bool SettingsLoader::setValueRecursion(QJsonObject &jsonRecursion, const QString &property, const int &index, const QJsonValue &value)
{
    bool valueChanged = false;  //флаг изменённого значения
    QJsonObject::iterator it;

    for (it = jsonRecursion.begin(); it != jsonRecursion.end(); ++it)
    {
        if (it->isObject()) //если встретился объект
        {
            QJsonObject jsonSection = it->toObject();   //получение секции и преобразовании её в объект
            valueChanged = setValueRecursion(jsonSection, property, index, value);  //рекурентный вызов и обработка в нём секции
            if (valueChanged)
            {
                *it = jsonSection;
                break;
            }
        }
        else if (it->isArray()) //если встретился массив
        {
            if (index > -1)
            {
                QJsonArray jsonArray = it->toArray();
                if (it.key() == property)   valueChanged = setArrayRecursion(jsonArray, property, index, value,  true); //если имя текущего массива совпадает со значением property
                else                        valueChanged = setArrayRecursion(jsonArray, property, index, value, false);
                if (valueChanged) break;
            }
            else if (index == -2 && it.key() == property)
            {
                it.value() = value;
                valueChanged = true;
            }
        }
        else if (it.key() == property && index < 0)  //если встретился ключ с именени property и property - не имя массива
        {
            it.value() = value;
            valueChanged = true;
            break;
        }
    }
    return valueChanged;
}

bool SettingsLoader::setArrayRecursion(QJsonArray &jsonArray, const QString &property, const int &index, const QJsonValue &value, bool propertyArray)
{
    bool valueChanged = false;
    if (propertyArray && index > -1) //если имя текущего массива совпадает с property
    {
        jsonArray[index] = value;   //запись нового значения
        valueChanged = true;        //флаг записи
        propertyArray = false;
    }
    else                //у текущего массива другое имя. Необходим поиск среди его элементов
    {
        for (int i = 0; i < jsonArray.size(); ++i)
        {
            if (jsonArray[i].isObject())
            {
                QJsonObject objectElement = jsonArray[i].toObject(); //получение объекта из элемента массива
                valueChanged = setValueRecursion(objectElement, property, index, value); //вызов основной функции рекурсии
                if (valueChanged)
                {
                    jsonArray[i] = objectElement;   //если в objectElement изменено значение, запись объекта в элемент массива
                    break;
                }
            }
        }
    }
    return valueChanged;
}

bool SettingsLoader::setValue(QStringList jsonNameList, QJsonValue value)
{
    return setValueRecursion(jsonObject, jsonNameList, value);
}

bool SettingsLoader::setValueRecursion(QJsonObject &jsonRecursion, const QStringList jsonNameList, const QJsonValue &value)
{
    QString jsonName;
    bool valueChanged = false;
    int index = -1;
    int b = jsonNameList[0].indexOf("[");
    int e = jsonNameList[0].indexOf("]");

    if (b == -1)    //объект, не массив
    {
        jsonName = jsonNameList[0];
    }
    else if (e - b > 1)  //запрошен элемент массива
    {
        index = jsonNameList[0].mid(b + 1, e - b - 1).toInt(); //чтение индекса между [ ]
        jsonName = jsonNameList[0].mid(0, b);
    }
    else if (e - b == 1)    //массив целиком
    {
        index = -2;
        jsonName = jsonNameList[0].mid(0, b);
    }

    if (jsonNameList.size() > 1)    //не достигнут низ иерархии
    {
        if (index == -1)            //текущий элемент списка имён - не элемент массива. Преобазование его в объект для дальнейшей обработки
        {
            QJsonObject jsonSubObject = jsonRecursion[jsonName].toObject();
            valueChanged = setValueRecursion(jsonSubObject, jsonNameList.mid(1, jsonNameList.size() - 1), value);
            if (valueChanged) jsonRecursion[jsonName] = jsonSubObject;
        }
        else if (index > -1)         //текущий элемент списка имён - элемент массива. Преобазование его элемента с индексом в объект для дальнейшей обработки
        {
            QJsonArray jsonArray = jsonRecursion.value(jsonName).toArray(); //Получение значения объекта, преобразование его в массив
            QJsonObject jsonSubObject = jsonArray[index].toObject();  //Получение элемента массива и преобразование его в объект
            valueChanged = setValueRecursion(jsonSubObject, jsonNameList.mid(1, jsonNameList.size() - 1), value);
            if (valueChanged)
            {
                jsonArray[index] = jsonSubObject;
                jsonRecursion[jsonName] = jsonArray;
            }
        }
    }
    else                            //низ иерархии
    {
        if (index > -1)
        {
            QJsonArray jsonArray = jsonRecursion.value(jsonName).toArray();
            jsonArray[index] = value;
            jsonRecursion[jsonName] = jsonArray;
            valueChanged = true;
        }
        else
        {
            jsonRecursion[jsonName] = value;
            valueChanged = true;
        }
    }
    return valueChanged;
}

void SettingsLoader::saveJsonToFile() const
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QJsonDocument set;
        set.setObject(jsonObject);
        file.write(set.toJson());
        file.close();
    }
    else
    {
        std::runtime_error saveFileError(fileName.toUtf8());
        throw saveFileError;    //исключение по ошибке открытия файла на запись
    }
}
