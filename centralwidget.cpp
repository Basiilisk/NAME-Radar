#include "centralwidget.h"

#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

CentralWidget::CentralWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* mainL = new QVBoxLayout(this);
    auto* namesL = new QHBoxLayout();

    mainL->addLayout(namesL);

    splitter = new QSplitter(Qt::Horizontal);
    stroyovaText = new QTextEdit(this);
    rcText = new QTextEdit(this);

    firstName = new QLineEdit(this);
    secondName = new QLineEdit(this);
    fatherName = new QLineEdit(this);

    stroyova = new QCheckBox("СТРОЙОВА", this);
    rc = new QCheckBox("РС", this);
    stroyova->setChecked(setting.radioBtnLoad("STROYOVA_BTN"));
    rc->setChecked(setting.radioBtnLoad("RC_BTN"));

    auto* stroyovaL = new QVBoxLayout();
    stroyovaL->addWidget(stroyovaText);

    auto* rcL = new QVBoxLayout();
    rcL->addWidget(rcText);

    splitter->addWidget(stroyovaText);
    splitter->addWidget(rcText);

    namesL->addWidget(firstName);
    namesL->addWidget(secondName);
    namesL->addWidget(fatherName);
    namesL->addWidget(stroyova);
    namesL->addWidget(rc);

    firstName->setPlaceholderText("Прізвище");
    secondName->setPlaceholderText("Ім'я");
    fatherName->setPlaceholderText("По батькові");
    stroyovaText->setPlaceholderText("Пошук по стройовим наказам");
    rcText->setPlaceholderText("Пошук по РСним наказам");

    btn = new QPushButton("Шукати", this);

    mainL->addWidget(btn);

    mainL->addWidget(splitter);

    connect(btn, &QPushButton::clicked,
        this, &CentralWidget::onButtonClicked);

    connect(stroyova, &QCheckBox::checkStateChanged,
        this, &CentralWidget::stroyovaChanged);

    connect(rc, &QCheckBox::checkStateChanged,
        this, &CentralWidget::rcChanged);
}

void CentralWidget::onButtonClicked()
{
    stroyovaText->setText("Push button");
}

void CentralWidget::stroyovaChanged(Qt::CheckState state)
{
    if (state) {
        stroyovaText->show();
        setting.radioBtnSave("STROYOVA_BTN", true);
    } else {
        stroyovaText->hide();
        setting.radioBtnSave("STROYOVA_BTN", false);
    }
}

void CentralWidget::rcChanged(Qt::CheckState state)
{
    if (state) {
        rcText->show();
        setting.radioBtnSave("RC_BTN", true);
    } else {
        rcText->hide();
        setting.radioBtnSave("RC_BTN", false);
    }
}
