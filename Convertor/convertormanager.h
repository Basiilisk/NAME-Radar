#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

class ConvertorManager {
public:
    ConvertorManager();

    void convertFiles(const QString& sourceRoot, const QString& dbPath);

    void writeErrorToLog(const QString& fileName, const QString& errorMsg);

    // void on_btnConvert_clicked(); // Твоя кнопка запуску
    void on_btnCancel_clicked();  // Твоя кнопка відміни

private:
    bool cancelRequested = false;
};
