#pragma once

#include <QString>

#include "StructUSE.h"

class SortFileByDate {
public:
    SortFileByDate();

    int sortResultData(const QString& inPath, const QString& outPath, const OutputData& logs);
};
