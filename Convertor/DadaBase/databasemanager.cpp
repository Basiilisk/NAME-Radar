#include "databasemanager.h"

#include <QDebug>
#include <QFile>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

bool DatabaseManager::isDatabaseValid(const QString& dbPath)
{
    QFile file(dbPath);
    // Якщо файлу немає або він порожній (0 байт) - повертаємо false
    if (!file.exists() || file.size() == 0) {
        return false;
    }
    return true;
}

bool DatabaseManager::deleteDatabase(const QString& dbPath)
{
    if (QFile::exists(dbPath)) {
        if (QFile::remove(dbPath)) {
            qDebug() << "Базу даних успішно видалено:" << dbPath;
            return true;
        } else {
            qWarning() << "Не вдалося видалити БД (можливо, зайнята іншим процесом):" << dbPath;
            return false;
        }
    }
    return true; // Якщо файлу і так не було - все ок
}

DatabaseManager::DatabaseManager(const QString& convertDBName)
{
    // 1. Перевіряємо, чи підключення з таким іменем вже існує в пам'яті Qt
    if (QSqlDatabase::contains(convertDBName)) {
        db = QSqlDatabase::database(convertDBName);
    }

    else {
        db = QSqlDatabase::addDatabase("QSQLITE", convertDBName);
        qDebug() << "2convertDBName: " << convertDBName;
    }
}

DatabaseManager::~DatabaseManager()
{
    // Зберігаємо ім'я підключення перед тим, як його закрити
    QString connectionName = db.connectionName();

    if (db.isOpen()) {
        db.close();
    }

    // 2. Правильне видалення підключення з реєстру Qt,
    // щоб не було помилки "duplicate connection" при наступному запуску
    db = QSqlDatabase(); // Обнуляємо саму змінну, щоб вона відпустила базу
    QSqlDatabase::removeDatabase(connectionName); // Видаляємо ім'я
}

bool DatabaseManager::initDatabase(const QString& dbFilePath)
{
    qDebug() << "2dbFilePath: " << dbFilePath;

    db.setDatabaseName(dbFilePath);
    if (!db.open()) {
        qDebug() << "Критична помилка: Не вдалося відкрити базу даних:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db); // ТУТ ВСЕ ПРАВИЛЬНО (передали db)
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
    QSqlQuery query(db); // <--- ВАЖЛИВО: ПЕРЕДАЄМО db!
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
    QSqlQuery query(db); // <--- ВАЖЛИВО: ПЕРЕДАЄМО db!
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

    // <--- ВАЖЛИВО: ПЕРЕДАЄМО db ДРУГИМ ПАРАМЕТРОМ!
    QSqlQuery query("SELECT relative_path FROM documents", db);

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
    QSqlQuery query(db); // <--- ВАЖЛИВО: ПЕРЕДАЄМО db!
    query.prepare("DELETE FROM documents WHERE relative_path = :path");
    query.bindValue(":path", relativePath);
    return query.exec();
}
