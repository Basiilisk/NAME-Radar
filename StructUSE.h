#pragma once

#include <QString>

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
