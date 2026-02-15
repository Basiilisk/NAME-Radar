#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>

class ConvertorToTxt {
public:
    ConvertorToTxt();

    int convertWithLibreOffice(const QString& inputFilePath, const QString& outputTxtPath, QString& errorMessage);
    void processDirectory(const QString& sourceRoot, const QString& destRoot);
};
