#include "jsonsettings.h"

#include <QFileDialog>

JSONSettings::JSONSettings(QObject* parent)
    : QObject(parent)
{
}

bool JSONSettings::chooseAndSaveFolder(const QString& name, QWidget* parent)
{
    QString folderPath = QFileDialog::getExistingDirectory(
        parent, name,
        loadFolder(name),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (folderPath.isEmpty())
        return false;

    QJsonObject settings = load();
    QJsonObject path = settings.value("Path").toObject();
    path[name] = folderPath;
    settings["Path"] = path;

    save(settings);

    return true;
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
