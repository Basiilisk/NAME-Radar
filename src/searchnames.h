#pragma once

#include <QRegularExpression>
#include <QSqlDatabase>
#include <QString>

#include "StructUSE.h"

class SearchNames {
public:
    SearchNames();

    void searchNameInDatabase(const QString& dbPath,
        const QString& convertDBName,
        QString& outText,
        const SearchedName& names,
        OutputData& logs);

private:
    QVector<MatchBeforeAndName> findBeforeLastFirst(const QString& text,
        const SearchedName& names);
};
