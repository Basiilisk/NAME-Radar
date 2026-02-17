#include "searchnames.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

SearchNames::SearchNames() { }

void SearchNames::searchNameInDatabase(const QString& dbPath, const QString& connectionName, QString& outText, const SearchedName& names, OutputData& logs)
{
    if (names.last.isEmpty() && names.first.isEmpty() && names.father.isEmpty())
        return;

    // Створюємо ізольоване підключення для конкретного потоку
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbPath);

        if (!db.open()) {
            qWarning() << "Помилка відкриття БД у потоці" << connectionName << ":" << db.lastError().text();
            return;
        }

        QSqlQuery query(db);
        if (!query.exec("SELECT relative_path, content FROM documents")) {
            qWarning() << "Помилка виконання запиту до БД:" << query.lastError().text();
            return;
        }

        while (query.next()) {
            // Запобігає зависанню GUI під час тривалого пошуку
            QCoreApplication::processEvents();

            QString path = query.value(0).toString();
            QString fullContent = query.value(1).toString();

            ++logs.allFiles;

            if (!names.last.isEmpty() && !fullContent.contains(names.last, Qt::CaseInsensitive)) {
                continue;
            }
            if (!names.first.isEmpty() && !fullContent.contains(names.first, Qt::CaseInsensitive)) {
                continue;
            }

            fullContent.replace('\r', ' ');

            auto matches = findBeforeLastFirst(fullContent, names);

            for (const auto& m : matches) {
                outText += QFileInfo(path).completeBaseName()
                    + ".doc : \t"
                    + m.names.last + " "
                    + m.names.first + " "
                    + m.names.father
                    + "\n";

                ++logs.searchedNames;
            }
        }
    } // db виходить із зони видимості та закривається
    QSqlDatabase::removeDatabase(connectionName);
}

QVector<MatchBeforeAndName> SearchNames::findBeforeLastFirst(const QString& text, const SearchedName& names)
{
    QVector<MatchBeforeAndName> results;

    const QString pattern = QString(R"((?:^|\p{Zs})([\p{L}’ʼ'-]*%1[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%2[\p{L}’ʼ'-]*)\p{Zs}+([\p{L}’ʼ'-]*%3[\p{L}’ʼ'-]*)(?=$|\P{L}))")
                                .arg(("(?<!\\p{L})" + QRegularExpression::escape(names.last) + "[\\p{L}’ʼ'-]*"),
                                    QRegularExpression::escape(names.first),
                                    QRegularExpression::escape(names.father));

    QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);

    auto it = re.globalMatch(text);

    while (it.hasNext()) {
        auto m = it.next();

        MatchBeforeAndName res;
        res.names.last = m.captured(1);
        res.names.first = m.captured(2);
        res.names.father = m.captured(3);

        res.found = true;
        results.push_back(res);
    }

    return results;
}
