#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class DatabaseManager {

public:
    enum FileStatus {
        Unchanged,
        New,
        Modified
    };

    explicit DatabaseManager(const QString& convertDBName);
    ~DatabaseManager();

    bool initDatabase(const QString& dbFilePath);
    FileStatus getFileStatus(const QString& relativePath, const QString& compositeHash);
    bool upsertDocument(const QString& relativePath, const QString& compositeHash, const QString& textContent);
    QStringList getDeletedFiles(const QStringList& currentRelativePathsOnDisk);
    bool removeDocument(const QString& relativePath);

private:
    QSqlDatabase db;
};
