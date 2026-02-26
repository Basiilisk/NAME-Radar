#pragma once

#include <QString>

class LibreOfficeConverter {
public:
    static QString extractText(const QString& inputFilePath, QString& errorMessage);
};
