#pragma once

#include <QRegularExpression>
#include <QString>

#include "StructUSE.h"

class SearchNames {
public:
    SearchNames();

    void searchNameInFile(QString& rootDir, QString& outText, SearchedName& names, OutputData& logs);

private:
    MatchBeforeAndName findBeforeLastFirst(const QString& text,
        const SearchedName& names);
};
