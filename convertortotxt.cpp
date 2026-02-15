#include "convertortotxt.h"

ConvertorToTxt::ConvertorToTxt() { }

int ConvertorToTxt::convertWithLibreOffice(const QString& inputFilePath, const QString& outputTxtPath, QString& errorMessage)
{
    // Шукаємо soffice.exe поруч із нашою програмою (в папці build/.../Release або Debug)
    // QString appDir = QCoreApplication::applicationDirPath();
    const QString sofficePath = "../../LOP/libreOffice/App/libreoffice/program/soffice.exe";

    if (!QFile::exists(sofficePath)) {
        errorMessage = "Error: Не знайдено LibreOffice за шляхом:\n" + sofficePath;
        return -1;
    }

    if (!QFile::exists(inputFilePath)) {
        errorMessage = "Error: Вхідний файл не знайдено:\n" + inputFilePath;
        return -2;
    }

    // LibreOffice приймає ПАПКУ для збереження, тому витягуємо її з outputTxtPath
    QFileInfo outputInfo(outputTxtPath);
    QString outDir = outputInfo.absolutePath();

    // Формуємо аргументи для прихованого режиму
    QStringList args;
    args << "--headless"
         << "--invisible"
         << "--nologo"
         << "--nodefault"
         << "--convert-to" << "txt:Text"
         << "--outdir" << QDir::toNativeSeparators(outDir)
         << QDir::toNativeSeparators(inputFilePath);

    // Запускаємо процес
    QProcess process;
    process.start(sofficePath, args);

    if (!process.waitForStarted()) {
        errorMessage = "Error: Не вдалося запустити soffice.exe";
        return -2;
    }

    // Даємо йому максимум 30 секунд на файл
    if (!process.waitForFinished(30000)) {
        process.kill();
        errorMessage = "Error: Тайм-аут конвертації (завис).";
        return -3;
    }

    // LibreOffice створює .txt з таким самим іменем, як оригінал.
    // Переконаємося, що він називається саме так, як ми передали в outputTxtPath
    QFileInfo inputInfo(inputFilePath);
    QString generatedTxtPath = outDir + "/" + inputInfo.completeBaseName() + ".txt";

    if (QDir::cleanPath(generatedTxtPath) != QDir::cleanPath(outputTxtPath)) {
        if (QFile::exists(outputTxtPath))
            QFile::remove(outputTxtPath);
        QFile::rename(generatedTxtPath, outputTxtPath);
    }

    return 0; // Конвертація успішна
}

void ConvertorToTxt::processDirectory(const QString& sourceRoot, const QString& destRoot)
{
    // Timer start
    qDebug() << "\t______ToTxtConvertor______";
    const QDateTime tsStart = QDateTime::currentDateTime();
    qDebug() << "Start converting: " << tsStart.toString("yyyy-MM-dd hh:mm:ss");

    QDir sourceDir(sourceRoot);
    QDir destDir(destRoot);

    if (!sourceDir.exists()) {
        qDebug() << "Error: Вхідної папки не існує!";
        return;
    }

    if (!destDir.exists()) {
        destDir.mkpath(".");
    }

    // ==========================================================
    // ЕТАП 1: Збір файлів та підрахунок
    // ==========================================================
    qDebug() << "Сканую папку... Зачекайте.";

    QStringList filters;
    filters << "*.doc" << "*.docx";
    QDirIterator it(sourceRoot, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    // Створюємо список, куди запишемо всі знайдені файли
    QStringList filesToProcess;
    while (it.hasNext()) {
        filesToProcess.append(it.next());
    }

    int totalFiles = filesToProcess.size();

    if (totalFiles == 0) {
        qDebug() << "У вказаній папці не знайдено жодного .doc або .docx файлу.";
        return;
    }

    qDebug() << "Знайдено файлів для обробки:" << totalFiles;
    qDebug() << "Починаю конвертацію...\n";

    // ==========================================================
    // ЕТАП 2: Конвертація з виведенням прогресу
    // ==========================================================
    int successCount = 0;

    for (int i = 0; i < totalFiles; ++i) {
        QString inputFilePath = filesToProcess.at(i);

        // 1. Отримуємо відносний шлях
        QString relativePath = sourceDir.relativeFilePath(inputFilePath);

        // 2. Міняємо розширення на .txt
        QFileInfo relInfo(relativePath);
        QString relativePathTxt;
        if (relInfo.path() == ".") {
            relativePathTxt = relInfo.completeBaseName() + ".txt";
        } else {
            relativePathTxt = relInfo.path() + "/" + relInfo.completeBaseName() + ".txt";
        }

        // 3. Формуємо абсолютний шлях для збереження
        QString outputFilePath = destDir.absoluteFilePath(relativePathTxt);

        // 4. Створюємо підпапки
        QFileInfo outInfo(outputFilePath);
        QDir().mkpath(outInfo.absolutePath());

        // --- ОБРАХУНОК ВІДСОТКІВ ---
        // (i + 1) бо індекси починаються з 0
        int currentFileNum = i + 1;
        int percentage = (currentFileNum * 100) / totalFiles;

        // Виводимо красивий лог (noquote() прибирає зайві лапки з консолі)
        qDebug().noquote() << QString("[%1/%2] |%3%| Обробка: %4")
                                  .arg(currentFileNum)
                                  .arg(totalFiles)
                                  .arg(percentage)
                                  .arg(relativePath);

        // 5. Конвертуємо
        QString errorMsg;
        int err = convertWithLibreOffice(inputFilePath, outputFilePath, errorMsg);

        if (err == 0) {
            successCount++;

        // critical error
        } else if (err == -1) {
            qDebug().noquote() << "Error:" << errorMsg;
            return;
        } else {
            qDebug().noquote() << "Error:" << errorMsg;
        }
    }

    // Timer
    const QDateTime tsEnd = QDateTime::currentDateTime();

    qint64 timeMSecRunProg = tsStart.msecsTo(tsEnd);
    quint64 secRun = timeMSecRunProg / 1000;
    quint64 mSecRun = timeMSecRunProg % 1000;
    QString showSec = QString("%1.%2 sec")
                          .arg(secRun)
                          .arg(mSecRun, 2, 10, QChar('0'));

    // ==========================================================
    // ФІНАЛЬНИЙ ЗВІТ
    // ==========================================================
    qDebug() << "\n=== ВСІ ФАЙЛИ ОБРОБЛЕНО! ===";
    qDebug() << "Всього було:" << totalFiles << " за " << showSec << " сек";
    qDebug() << "Успішно конвертовано:" << successCount;
    if (totalFiles - successCount > 0) {
        qDebug() << "Файлів з помилками:" << (totalFiles - successCount);
    }
}
