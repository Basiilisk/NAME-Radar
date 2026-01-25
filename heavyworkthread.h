#pragma once

#include <QObject>
#include <QThread>

#include "searchnames.h"
#include "sortfilebydate.h"

class HeavyWorkThread : public QObject {

    Q_OBJECT
public:
    HeavyWorkThread(const QString rootDir, const SearchedName& names, const QString processName, QObject* parent = nullptr);

    void heavyWork();

public slots:
    void process();

signals:
    void finished(std::tuple<QString, OutputData, const QString> outData);

private:
    SearchNames searchName;
    SortFileByDate sortFile;

    const QString processName;
    const SearchedName names;
    const QString rootDir;
    QString outText;
    OutputData logs;
};
