#pragma once

#include <QString>
#include <QStringList>

class FileScanner {
public:
    // Повертає список абсолютних шляхів до всіх .doc та .docx
    static QStringList getWordDocuments(const QString& rootDir);
};
