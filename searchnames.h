#pragma once

#include <QRegularExpression>
#include <QString>

#include "StructUSE.h"

class SearchNames {
public:
    SearchNames();

    void searchNameInFile(const QString& rootDir, QString& outText, const SearchedName& names, OutputData& logs);

private:
    MatchBeforeAndName findBeforeLastFirst(const QString& text,
        const SearchedName& names);
};
