#include "libreofficeconverter.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryDir>
#include <QTextStream>

QString LibreOfficeConverter::extractText(const QString& inputFilePath, QString& errorMessage)
{
    const QString sofficePath = "../../LOP/libreOffice/App/libreoffice/program/soffice.exe";

    if (!QFile::exists(sofficePath)) {
        errorMessage = "Не знайдено рушій LibreOffice за шляхом: " + sofficePath;
        return "";
    }
    if (!QFile::exists(inputFilePath)) {
        errorMessage = "Вхідний файл фізично відсутній на диску.";
        return "";
    }

    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        errorMessage = "Системна помилка: не вдалося створити тимчасову папку ОС.";
        return "";
    }

    QStringList args;
    args << "--headless" << "--invisible" << "--nologo" << "--nodefault"
         << "--convert-to" << "txt:Text (encoded):UTF8"
         << "--outdir" << QDir::toNativeSeparators(tempDir.path())
         << QDir::toNativeSeparators(inputFilePath);

    QProcess process;
    process.start(sofficePath, args);

    if (!process.waitForStarted()) {
        errorMessage = "Не вдалося запустити процес. Системна помилка Qt: " + process.errorString();
        return "";
    }

    if (!process.waitForFinished(60000)) {
        process.kill();
        errorMessage = "Тайм-аут. LibreOffice завис (перевищено ліміт у 60 секунд).";
        return "";
    }

    if (process.exitStatus() == QProcess::CrashExit) {
        errorMessage = "Процес LibreOffice аварійно завершився (Crash).";
        return "";
    }

    int exitCode = process.exitCode();
    if (exitCode != 0) {
        QString processStdErr = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
        errorMessage = QString("LibreOffice повернув код помилки %1. Деталі: %2")
                           .arg(exitCode)
                           .arg(processStdErr.isEmpty() ? "Невідома внутрішня помилка" : processStdErr);
        return "";
    }

    QFileInfo inputInfo(inputFilePath);
    QString tempTxtPath = tempDir.path() + "/" + inputInfo.completeBaseName() + ".txt";

    if (!QFile::exists(tempTxtPath)) {
        errorMessage = "Процес завершився, але текстовий файл не був створений.";
        return "";
    }

    QFile txtFile(tempTxtPath);
    if (!txtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Текстовий файл створено, але немає прав на його зчитування.";
        return "";
    }

    QTextStream in(&txtFile);
    in.setEncoding(QStringConverter::Utf8);
    QString extractedText = in.readAll();
    txtFile.close();

    return extractedText;
}
