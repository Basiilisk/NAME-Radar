#include "databasemanager.h"

#include <QDebug>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

DatabaseManager::DatabaseManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initDatabase(const QString& dbFilePath)
{
    db.setDatabaseName(dbFilePath);
    if (!db.open()) {
        qDebug() << "Критична помилка: Не вдалося відкрити базу даних:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS documents ("
                               "relative_path TEXT PRIMARY KEY, "
                               "hash TEXT, "
                               "content TEXT"
                               ")";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Помилка створення таблиці:" << query.lastError().text();
        return false;
    }
    return true;
}

DatabaseManager::FileStatus DatabaseManager::getFileStatus(const QString& relativePath, const QString& compositeHash)
{
    QSqlQuery query;
    query.prepare("SELECT hash FROM documents WHERE relative_path = :path");
    query.bindValue(":path", relativePath);

    if (query.exec() && query.next()) {
        QString savedHash = query.value(0).toString();
        if (savedHash == compositeHash) {
            return Unchanged;
        } else {
            return Modified;
        }
    }
    return New;
}

bool DatabaseManager::upsertDocument(const QString& relativePath, const QString& compositeHash, const QString& textContent)
{
    QSqlQuery query;
    query.prepare("REPLACE INTO documents (relative_path, hash, content) VALUES (:path, :hash, :content)");
    query.bindValue(":path", relativePath);
    query.bindValue(":hash", compositeHash);
    query.bindValue(":content", textContent);

    if (!query.exec()) {
        qDebug() << "Помилка запису в БД:" << query.lastError().text();
        return false;
    }
    return true;
}

QStringList DatabaseManager::getDeletedFiles(const QStringList& currentRelativePathsOnDisk)
{
    QStringList deletedFiles;
    QSet<QString> diskPaths(currentRelativePathsOnDisk.begin(), currentRelativePathsOnDisk.end());

    QSqlQuery query("SELECT relative_path FROM documents");
    while (query.next()) {
        QString pathInDb = query.value(0).toString();
        if (!diskPaths.contains(pathInDb)) {
            deletedFiles.append(pathInDb);
        }
    }
    return deletedFiles;
}

bool DatabaseManager::removeDocument(const QString& relativePath)
{
    QSqlQuery query;
    query.prepare("DELETE FROM documents WHERE relative_path = :path");
    query.bindValue(":path", relativePath);
    return query.exec();
}
