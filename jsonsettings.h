#pragma once

#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>

static QString settingsFilePath()
{
    return QStandardPaths::writableLocation(
               QStandardPaths::AppConfigLocation)
        + "/Name_Radar_V2_settings.json";
}

static QJsonObject load()
{
    QFile file(settingsFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return {};

    return QJsonDocument::fromJson(file.readAll()).object();
}

static void save(const QJsonObject& obj)
{
    QFile file(settingsFilePath());
    QDir().mkpath(QFileInfo(file).path());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
        file.close();
    }
}
class JSONSettings : public QObject {
    Q_OBJECT

public:
    JSONSettings(QObject* parent = nullptr);

    // Новий метод, який просто приймає ключ та готовий шлях
    void saveFolder(const QString& name, const QString& path);

    QString loadFolder(const QString& name) const;

    void radioBtnSave(const QString& name, bool state);
    bool radioBtnLoad(const QString& name);
};
