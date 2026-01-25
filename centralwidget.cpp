#include "centralwidget.h"

#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

CentralWidget::CentralWidget(QWidget* parent)
    : QWidget(parent)
    , workerThread(nullptr)
    , worker(nullptr)
{
    auto* mainL = new QVBoxLayout(this);
    auto* namesL = new QHBoxLayout();
    auto* buttL = new QHBoxLayout();

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    mainL->addWidget(line);
    mainL->addLayout(namesL);

    splitter = new QSplitter(Qt::Horizontal);
    stroyovaText = new QTextEdit(this);
    rcText = new QTextEdit(this);

    firstName = new QLineEdit(this);
    secondName = new QLineEdit(this);
    fatherName = new QLineEdit(this);

    progressBar = new QProgressBar();

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

    firstName->setText("Пур");
    butt = new QPushButton("Шукати", this);
    butt->setMinimumWidth(260);

    buttL->addStretch();
    buttL->addWidget(butt);
    buttL->addStretch();

    mainL->addLayout(buttL);
    mainL->addWidget(splitter);

    connect(butt, &QPushButton::clicked,
        this, &CentralWidget::onButtonClicked);

    connect(stroyova, &QCheckBox::toggled,
        this, &CentralWidget::stroyovaChanged);

    connect(rc, &QCheckBox::toggled,
        this, &CentralWidget::rcChanged);
}

void CentralWidget::startHeavyWork(QString& rootDir, SearchedName& names, const QString prossecName)
{
    workerThread = new QThread(this);
    worker = new HeavyWorkThread(rootDir, names, prossecName);

    worker->moveToThread(workerThread);

    // 3️   Connections
    connect(workerThread, &QThread::started,
        worker, &HeavyWorkThread::process);

    connect(worker, &HeavyWorkThread::finished,
        this, &CentralWidget::onHeavyWorkFinished);

    connect(worker, &HeavyWorkThread::finished,
        workerThread, &QThread::quit);

    connect(workerThread, &QThread::finished,
        worker, &QObject::deleteLater);

    connect(workerThread, &QThread::finished,
        workerThread, &QObject::deleteLater);

    // 4️⃣ Start
    workerThread->start();
}

void CentralWidget::onButtonClicked()
{
    SearchedName names;
    names.last = firstName->text();
    names.first = secondName->text();
    names.father = fatherName->text();

    progressBar->setRange(0, 0); // busy animation
    progressBar->setVisible(true);

    butt->setEnabled(false);

    if (stroyova->isChecked()) {
        QString path = setting.loadFolder("STROYOVA_PATH");
        QString outText1;
        QString outText2;

        stroyovaText->setText("Шукає");
        startHeavyWork(path, names, "STROYOVA");

        radButtState1 = true;
    }

    if (rc->isChecked()) {
        QString path = setting.loadFolder("PC_PATH");
        QString outText2;

        rcText->setText("Шукає");
        startHeavyWork(path, names, "PC");

        radButtState2 = true;
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

void CentralWidget::onHeavyWorkFinished(std::tuple<QString, OutputData, const QString> outData)
{
    std::tuple<QString, OutputData, const QString> data = outData;

    if (get<2>(outData) == "STROYOVA") {
        qDebug() << "Change ~~STROYOVA~~";
        op1 = true;
        stroyovaText->setText(get<0>(outData));
    }

    if (get<2>(outData) == "PC") {
        qDebug() << "Change ~~PC~~";
        op2 = true;
        rcText->setText(get<0>(outData));
    }

    if (((radButtState1 == op1) && (radButtState2 == op2))) {
        radButtState1 = false;
        radButtState2 = false;
        op1 = false;
        op2 = false;
        butt->setEnabled(true);
        progressBar->setVisible(false);
    }
}
