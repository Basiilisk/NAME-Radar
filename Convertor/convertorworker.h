#pragma once

#include <QObject>
#include <QString>
#include <atomic> // Для безпечної зупинки з іншого потоку

class ConvertorWorker : public QObject {
    Q_OBJECT
public:
    explicit ConvertorWorker(const QString& convertDBName, const QString& sourceRoot, const QString& dbPath, QObject* parent = nullptr);

public slots:
    void process();
    void cancel(); // Слот для зупинки процесу

signals:
    void logMessage(const QString& message);
    void finished();

private:
    QString convertDBName;
    QString sourceRoot;
    QString dbPath;

    // std::atomic гарантує, що зміна цієї змінної з головного потоку
    // миттєво і безпечно прочитається у фоновому потоці
    std::atomic<bool> cancelRequested;

    void writeErrorToLog(const QString& fileName, const QString& errorMsg);
};
