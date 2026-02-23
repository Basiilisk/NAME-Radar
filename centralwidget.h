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
class HighlightLineEdit;

class CentralWidget : public QWidget {
    Q_OBJECT

public:
    explicit CentralWidget(QWidget* parent = nullptr);

    void startHeavyWork(const QString& dbPath, SearchedName& names, const QString prossecName, QThread*& thread);
    void startLoading(QTextEdit* edit, QTimer*& timer);
    void stopLoading(QTimer*& timer);

private slots:
    void onButtonClicked();
    void stroyovaChanged(bool state);
    void rcChanged(bool state);
    void onHeavyWorkFinished(std::tuple<QString, OutputData, const QString> outData);

private:
    HighlightLineEdit* firstName;
    HighlightLineEdit* secondName;
    HighlightLineEdit* fatherName;

    QCheckBox* stroyova;
    QCheckBox* rc;

    QTextEdit* stroyovaText;
    QTextEdit* rcText;

    QSplitter* splitter;
    QPushButton* butt;

    JSONSettings setting;
    OutputData logData1;
    OutputData logData2;

    HeavyWorkThread* worker;

    QString outText1;
    QString outText2;

    bool op1 = false;
    bool op2 = false;

    bool radButtState1 = false;
    bool radButtState2 = false;

    QTimer* timerST = nullptr;
    QTimer* timerRS = nullptr;

    QThread* stroyovaThread = nullptr;
    QThread* rsThread = nullptr;
};
