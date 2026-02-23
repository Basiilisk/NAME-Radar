#pragma once

#include <QString>

// Threads's and basedata's names
namespace Names {
    inline const QString Rs = QStringLiteral("RS");
    inline const QString Stroyova = QStringLiteral("STROYOVA");
}

struct OutputData {
    int allFiles = 0;
    int searchedNames = 0;
};

struct SearchedName {
    QString last; // last name as it appears in text
    QString first; // first name as it appears in text
    QString father; // first name as it appears in text
};

struct MatchBeforeAndName {
    QString before; // word before
    SearchedName names;
    bool found = false;
};
