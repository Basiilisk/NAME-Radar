#include "jsonsettings.h"

#include <QFileDialog>

JSONSettings::JSONSettings(QObject* parent)
    : QObject(parent)
{
}

void JSONSettings::saveFolder(const QString& name, const QString& path)
{
    if (path.isEmpty())
        return;

    QJsonObject settings = load();
    QJsonObject savePath = settings.value("Path").toObject();
    savePath[name] = path;
    settings["Path"] = savePath;

    save(settings);
}

QString JSONSettings::loadFolder(const QString& name) const
{
    QFile file(settingsFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    QJsonObject path = root.value("Path").toObject();

    return path[name].toString();
}

void JSONSettings::radioBtnSave(const QString& name, bool state)
{
    QJsonObject root = load();
    QJsonObject buttState = root.value("ButtonState").toObject();
    buttState[name] = state;
    root["ButtonState"] = buttState;

    save(root);
}

bool JSONSettings::radioBtnLoad(const QString& name)
{
    QJsonObject root = load();
    QJsonObject buttState = root.value("ButtonState").toObject();

    return buttState[name].toBool(false);
}
