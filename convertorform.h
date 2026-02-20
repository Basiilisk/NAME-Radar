#pragma once

#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include "Convertor/convertormanager.h"

class ConvertorForm : public QWidget {
    Q_OBJECT

public:
    ConvertorForm(QWidget* parent = nullptr);

private:
    QPushButton* convert = nullptr;
    QPushButton* cancel = nullptr;
    QPushButton* deleteAll = nullptr;

    QTextEdit* logText = nullptr;
    std::unique_ptr<ConvertorManager> converManager;
};
