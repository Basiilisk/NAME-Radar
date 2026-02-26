#include "heavyworkthread.h"

HeavyWorkThread::HeavyWorkThread(const QString dbPath, const SearchedName& names, const QString processName, QObject* parent)
    : dbPath(dbPath)
    , names(names)
    , processName(processName)
    , QObject(parent)
{
}

void HeavyWorkThread::heavyWork()
{
    // processName використовується як унікальне ім'я підключення SQLite (напр. "STROYOVA")
    searchName.searchNameInDatabase(dbPath, processName, outText, names, logs);
    sortFile.sortResultData(outText, logs);
}

void HeavyWorkThread::process()
{
    heavyWork();
    emit finished({ outText, logs, processName });
}
