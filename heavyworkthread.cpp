#include "heavyworkthread.h"

HeavyWorkThread::HeavyWorkThread(const QString rootDir, const SearchedName& names, const QString processName, QObject* parent)
    : rootDir(rootDir)
    , names(names)
    , processName(processName)
    , QObject(parent)
{
}

void HeavyWorkThread::heavyWork()
{
    qDebug() << "rootDir" << rootDir;

    searchName.searchNameInFile(rootDir, outText, names, logs);
    sortFile.sortResultData(outText, logs);
}

void HeavyWorkThread::process()
{
    heavyWork();

    emit finished({ outText, logs, processName });
}
