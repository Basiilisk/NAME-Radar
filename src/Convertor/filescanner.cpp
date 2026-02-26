#include "filescanner.h"

#include <QDir>
#include <QDirIterator>

QStringList FileScanner::getWordDocuments(const QString& rootDir)
{
    QStringList foundFiles;
    QDir dir(rootDir);

    if (!dir.exists()) {
        return foundFiles;
    }

    QStringList filters;
    filters << "*.doc" << "*.docx";

    QDirIterator it(rootDir, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        foundFiles.append(it.next());
    }

    return foundFiles;
}
