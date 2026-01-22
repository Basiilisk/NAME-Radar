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
    bool strVal = setting.radioBtnLoad("STROYOVA_BTN");
    stroyova->setChecked(strVal);
    stroyovaChanged(strVal);

    rc = new QCheckBox("РС", this);
    bool rcVal = setting.radioBtnLoad("RC_BTN");
    rc->setChecked(rcVal);
    rcChanged(rcVal);

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
    stroyovaText->setPlaceholderText("Пошук по СТРОЙОВИМ наказам");
    rcText->setPlaceholderText("Пошук по РСним наказам");

    btn = new QPushButton("Шукати", this);

    mainL->addWidget(btn);

    mainL->addWidget(splitter);

    connect(btn, &QPushButton::clicked,
        this, &CentralWidget::onButtonClicked);

    connect(stroyova, &QCheckBox::toggled,
        this, &CentralWidget::stroyovaChanged);

    connect(rc, &QCheckBox::toggled,
        this, &CentralWidget::rcChanged);
}

void CentralWidget::onButtonClicked()
{
    SearchedName names;
    names.last = firstName->text();
    names.first = secondName->text();
    names.father = fatherName->text();

    if (stroyova->isChecked()) {
        QString path = setting.loadFolder("STROYOVA_PATH");
        QString outText1;
        stroyovaText->setText("Шукає");
        searchName.searchNameInFile(path, outText1, names, logData1);
        sortFile.sortResultData(outText1, logData1);

        stroyovaText->setText(outText1);
    }

    if (rc->isChecked()) {
        QString path = setting.loadFolder("PC_PATH");
        QString outText2;
        rcText->setText("Шукає");
        searchName.searchNameInFile(path, outText2, names, logData2);
        sortFile.sortResultData(outText2, logData2);

        rcText->setText(outText2);
    }
}

void CentralWidget::stroyovaChanged(bool state)
{
    if (state) {
        stroyovaText->show();
        setting.radioBtnSave("STROYOVA_BTN", true);
    } else {
        stroyovaText->hide();
        setting.radioBtnSave("STROYOVA_BTN", false);
    }
}

void CentralWidget::rcChanged(bool state)
{
    if (state) {
        rcText->show();
        setting.radioBtnSave("RC_BTN", true);
    } else {
        rcText->hide();
        setting.radioBtnSave("RC_BTN", false);
    }
}
