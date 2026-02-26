#pragma once

#include <QString>

#include "StructUSE.h"

class SortFileByDate {
public:
    SortFileByDate();

    int sortResultData(QString& inPath, const OutputData& logs);
};
