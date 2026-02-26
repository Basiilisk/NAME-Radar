#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTextEdit>

class ConvertorManager : public QObject {
    Q_OBJECT

public:
    ConvertorManager(QObject* obj = nullptr);

    void convertFiles(const QString& convertDBName, const QString& sourceRoot, const QString& dbPath, QTextEdit* textEditLog);

    void writeErrorToLog(const QString& fileName, const QString& errorMsg);

    // void on_btnConvert_clicked(); // Твоя кнопка запуску

public slots:
    void on_btnCancel_clicked(); // Твоя кнопка відміни

private:
    bool cancelRequested = false;
    // QTextEdit* textEditLog = nullptr;
};
