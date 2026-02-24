#include "convertorworker.h"

#include "DadaBase/databasemanager.h"
#include "filescanner.h"
#include "hashutils.h"
#include "libreofficeconverter.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>

ConvertorWorker::ConvertorWorker(const QString& convertDBName, const QString& sourceRoot, const QString& dbPath, QObject* parent)
    : QObject(parent)
    , convertDBName(convertDBName)
    , sourceRoot(sourceRoot)
    , dbPath(dbPath)
    , cancelRequested(false)
{
}

// Слот, який буде викликаний при натисканні "Відмінити"
void ConvertorWorker::cancel()
{
    cancelRequested = true;
}

void ConvertorWorker::process()
{
    DatabaseManager dbManager(convertDBName);

    if (!dbManager.initDatabase(dbPath)) {
        emit logMessage("<font color='red'>Помилка відкриття БД.</font>");
        emit finished();
        return;
    }

    QStringList currentFiles = FileScanner::getWordDocuments(sourceRoot);

    if (currentFiles.isEmpty()) {
        emit logMessage("Документів не знайдено.");
        emit finished();
        return;
    }

    QStringList filesToConvert;
    QStringList hashesToUpdate;
    QStringList currentRelativePaths;
    QDir rootDirObj(sourceRoot);

    int unchangedCount = 0;

    for (const QString& absolutePath : currentFiles) {
        QString relativePath = rootDirObj.relativeFilePath(absolutePath);
        currentRelativePaths.append(relativePath);

        QString hash = HashUtils::calculateCompositeHash(sourceRoot, absolutePath);
        if (hash.isEmpty())
            continue;

        DatabaseManager::FileStatus status = dbManager.getFileStatus(relativePath, hash);

        if (status == DatabaseManager::New || status == DatabaseManager::Modified) {
            filesToConvert.append(absolutePath);
            hashesToUpdate.append(hash);
        } else {
            unchangedCount++;
        }
    }

    int convertCount = filesToConvert.size();
    int successCount = 0;
    int errorCount = 0;
    QDateTime startTime;

    if (convertCount > 0) {
        startTime = QDateTime::currentDateTime();
        emit logMessage(QString("Початок обробки: %1").arg(startTime.toString("hh:mm:ss")));
        emit logMessage(QString("До конвертації заявлено файлів: %1").arg(convertCount));

        for (int i = 0; i < convertCount; ++i) {
            // ПЕРЕВІРКА НА ЗУПИНКУ ВІД КОРИСТУВАЧА
            if (cancelRequested) {
                emit logMessage("<font color='orange'><b>[ПРОЦЕС ПЕРЕРВАНО КОРИСТУВАЧЕМ]</b></font>");
                break;
            }

            QString inputPath = filesToConvert.at(i);
            QString relativePath = rootDirObj.relativeFilePath(inputPath);
            QString hash = hashesToUpdate.at(i);

            QString log1 = QString("[%1/%2] Обробка: %3").arg(i + 1).arg(convertCount).arg(relativePath);
            emit logMessage(log1);

            QString errorMsg;
            QString documentText = LibreOfficeConverter::extractText(inputPath, errorMsg);

            if (!documentText.isEmpty()) {
                if (dbManager.upsertDocument(relativePath, hash, documentText)) {
                    successCount++;
                } else {
                    QString dbError = "Помилка БД: Не вдалося зберегти текст у базу даних.";
                    QFileInfo failedFileInfo(inputPath);
                    writeErrorToLog(failedFileInfo.fileName(), dbError);
                    errorCount++;
                }
            } else {
                QFileInfo failedFileInfo(inputPath);
                writeErrorToLog(failedFileInfo.fileName(), errorMsg);
                errorCount++;
            }
        }

        QDateTime endTime = QDateTime::currentDateTime();
        emit logMessage(QString("Кінець обробки: %1").arg(endTime.toString("hh:mm:ss")));

        qint64 elapsedSeconds = startTime.secsTo(endTime);
        qint64 minutes = elapsedSeconds / 60;
        qint64 seconds = elapsedSeconds % 60;

        emit logMessage(QString("Загальний час виконання: %1 хв %2 сек").arg(minutes).arg(seconds));
    } else {
        emit logMessage("Всі файли актуальні. Конвертація не потрібна.");
    }

    // Очищення виконується незалежно від того, чи був процес перерваний
    QStringList deletedFiles = dbManager.getDeletedFiles(currentRelativePaths);
    for (const QString& delPath : deletedFiles) {
        emit logMessage("[ВИДАЛЕНО З ДИСКУ] Очищення запису в БД: " + delPath);
        dbManager.removeDocument(delPath);
    }

    // Підсумковий звіт
    emit logMessage("<br>=== ПІДСУМКОВИЙ ЗВІТ ===");
    emit logMessage(QString("Пропущено (вже в базі):  %1").arg(unchangedCount));
    emit logMessage(QString("Успішно додано/оновлено:   %1").arg(successCount));
    emit logMessage(QString("Помилок при обробці:       %1").arg(errorCount));
    emit logMessage(QString("Видалено старих записів:   %1<br>").arg(deletedFiles.size()));

    // Сигнал про повне завершення
    emit finished();
}

void ConvertorWorker::writeErrorToLog(const QString& fileName, const QString& errorMsg)
{
    QString logPath = QCoreApplication::applicationDirPath() + "/log_file.txt";
    QFile logFile(logPath);

    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out.setEncoding(QStringConverter::Utf8);
        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss_dd.MM.yyyy");
        out << "_____" << timestamp << "____\n";
        out << "error: cant convert file |" << fileName << "|\n";
        out << "details: " << errorMsg << "\n\n";
        logFile.close();
    }
}
