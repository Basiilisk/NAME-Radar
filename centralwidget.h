#pragma once

#include "jsonsettings.h"

#include <QCheckBox>
#include <QSplitter>
#include <QTextEdit>
#include <QWidget>

class QPushButton;
class QLineEdit;

class CentralWidget : public QWidget {
    Q_OBJECT

public:
    explicit CentralWidget(QWidget* parent = nullptr);

private slots:
    void onButtonClicked();
    void stroyovaChanged(Qt::CheckState state);
    void rcChanged(Qt::CheckState state);

private:
    QLineEdit* firstName;
    QLineEdit* secondName;
    QLineEdit* fatherName;

    QCheckBox* stroyova;
    QCheckBox* rc;

    QTextEdit* stroyovaText;
    QTextEdit* rcText;

    QSplitter* splitter;
    QPushButton* btn;
    JSONSettings setting;
};
