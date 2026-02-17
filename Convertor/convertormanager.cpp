#include "convertormanager.h"

#include "DadaBase/databasemanager.h"

#include "filescanner.h"
#include "hashutils.h"
#include "libreofficeconverter.h"

ConvertorManager::ConvertorManager() { }

void ConvertorManager::convertFiles(const QString& sourceRoot, const QString& dbPath)
{
    cancelRequested = false;

    DatabaseManager dbManager;
    if (!dbManager.initDatabase(dbPath)) {
        return;
    }

    qDebug() << "Сканую директорію на наявність документів...";
    QStringList currentFiles = FileScanner::getWordDocuments(sourceRoot);

    if (currentFiles.isEmpty()) {
        qDebug() << "Документів не знайдено.";
        return;
    }

    qDebug() << "Звіряю файли з базою даних...";

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

    if (convertCount > 0) {
        QDateTime startTime = QDateTime::currentDateTime();
        qDebug() << "\nПочаток обробки:" << startTime.toString("hh:mm:ss");
        qDebug() << "До конвертації заявлено файлів:" << convertCount;

        for (int i = 0; i < convertCount; ++i) {
            // Дозволяємо інтерфейсу оновлюватися та реєструвати натискання кнопок
            QCoreApplication::processEvents();

            // Перевіряємо, чи користувач натиснув "Відмінити"
            if (cancelRequested) {
                qDebug() << "\n[ПРОЦЕС ПЕРЕРВАНО КОРИСТУВАЧЕМ]";
                break;
            }

            QString inputPath = filesToConvert.at(i);
            QString relativePath = rootDirObj.relativeFilePath(inputPath);
            QString hash = hashesToUpdate.at(i);

            qDebug().noquote() << QString("[%1/%2] Обробка: %3").arg(i + 1).arg(convertCount).arg(relativePath);

            QString errorMsg;
            QString documentText = LibreOfficeConverter::extractText(inputPath, errorMsg);

            if (!documentText.isEmpty()) {
                // Запис відбувається одразу, що дозволяє безпечно переривати цикл
                if (dbManager.upsertDocument(relativePath, hash, documentText)) {
                    successCount++;
                } else {
                    QString dbError = "Помилка БД: Не вдалося зберегти текст у базу даних.";
                    qDebug().noquote() << "    " << dbError;

                    QFileInfo failedFileInfo(inputPath);
                    writeErrorToLog(failedFileInfo.fileName(), dbError);
                    errorCount++;
                }
            } else {
                QFileInfo failedFileInfo(inputPath);

                qDebug().noquote() << "\n  [ПОМИЛКА КОНВЕРТАЦІЇ]";
                qDebug().noquote() << "  Назва файлу: " << failedFileInfo.fileName();
                qDebug().noquote() << "  Причина:     " << errorMsg << "\n";

                // Запис у лог-файл
                writeErrorToLog(failedFileInfo.fileName(), errorMsg);

                errorCount++;
            }
        }

        QDateTime endTime = QDateTime::currentDateTime();
        qDebug() << "\nКінець обробки:" << endTime.toString("hh:mm:ss");

        qint64 elapsedSeconds = startTime.secsTo(endTime);
        qint64 minutes = elapsedSeconds / 60;
        qint64 seconds = elapsedSeconds % 60;

        qDebug().noquote() << QString("Загальний час виконання: %1 хв %2 сек").arg(minutes).arg(seconds);
    } else {
        qDebug() << "Всі файли актуальні. Конвертація не потрібна.";
    }

    // Очищення виконується незалежно від того, чи був процес перерваний
    QStringList deletedFiles = dbManager.getDeletedFiles(currentRelativePaths);
    for (const QString& delPath : deletedFiles) {
        qDebug().noquote() << "[ВИДАЛЕНО З ДИСКУ] Очищення запису в БД:" << delPath;
        dbManager.removeDocument(delPath);
    }

    qDebug() << "\n=== ПІДСУМКОВИЙ ЗВІТ ===";
    qDebug() << "Пропущено (вже в базі):   " << unchangedCount;
    qDebug() << "Успішно додано/оновлено:  " << successCount;
    qDebug() << "Помилок при обробці:      " << errorCount;
    qDebug() << "Видалено старих записів:  " << deletedFiles.size();
}

void ConvertorManager::writeErrorToLog(const QString& fileName, const QString& errorMsg)
{
    QString logPath = QCoreApplication::applicationDirPath() + "/log_file.txt";
    QFile logFile(logPath);

    // QIODevice::Append гарантує, що старі записи не будуть видалені
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

void ConvertorManager::on_btnCancel_clicked()
{
    cancelRequested = true;
}
