#pragma once

#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include "Convertor/convertorworker.h"

class ConvertorForm : public QWidget {
    Q_OBJECT

public:
    ConvertorForm(QWidget* parent = nullptr);

private:
    QPushButton* convert = nullptr;
    QPushButton* cancel = nullptr;
    QPushButton* deleteAllRS_BD = nullptr;
    QPushButton* deleteAllStroyouva_BD = nullptr;

    QTextEdit* logText = nullptr;

    ConvertorWorker* convertWorker = nullptr;

    // СТРУКТУРА ДЛЯ ЧЕРГИ
    struct ConversionTask {
        QString name;
        QString sourceRoot;
        QString dbPath;
    };
    QList<ConversionTask> taskQueue;

    // МЕТОД, ЯКИЙ БУДЕ БРАТИ ЗАВДАННЯ З ЧЕРГИ
    void convertTask();
};
