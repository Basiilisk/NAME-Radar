#pragma once

#include "jsonsettings.h"

#include <QCheckBox>
#include <QProgressBar>
#include <QSplitter>
#include <QTextEdit>
#include <QWidget>

#include "StructUSE.h"
#include "heavyworkthread.h"

class QPushButton;
class QLineEdit;

class CentralWidget : public QWidget {
    Q_OBJECT

public:
    explicit CentralWidget(QWidget* parent = nullptr);

    void startHeavyWork(QString& rootDir, SearchedName& names, const QString prossecName);

private slots:
    void onButtonClicked();
    void stroyovaChanged(bool state);
    void rcChanged(bool state);
    void onHeavyWorkFinished(std::tuple<QString, OutputData, const QString> outData);

private:
    QLineEdit* firstName;
    QLineEdit* secondName;
    QLineEdit* fatherName;

    QCheckBox* stroyova;
    QCheckBox* rc;

    QTextEdit* stroyovaText;
    QTextEdit* rcText;

    QSplitter* splitter;
    QPushButton* butt;

    JSONSettings setting;
    OutputData logData1;
    OutputData logData2;

    QThread* workerThread;
    HeavyWorkThread* worker;
    QProgressBar* progressBar;

    QString outText1;
    QString outText2;

    bool op1 = false;
    bool op2 = false;

    bool radButtState1 = false;
    bool radButtState2 = false;
};
