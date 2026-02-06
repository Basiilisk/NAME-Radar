#include "centralwidget.h"

#include <QLayout>
#include <QPushButton>
#include <QTimer>

#include "highlightlineedit.h"

void settingHighlightLineEdit(HighlightLineEdit* lineEdit)
{
    lineEdit->setMinimumHeight(30);
    auto font = lineEdit->font();
    font.setPointSize(12);
    lineEdit->setFont(font);
}

const QSize LineEditSize(309, 21);

static const QStringList LoadingFrames = {
    "Ш",
    "Шу",
    "Шук",
    "Шука",
    "Шукає",
    "Шукає.",
    "Шукає..",
    "Шукає..."
};

CentralWidget::CentralWidget(QWidget* parent)
    : QWidget(parent)
    , worker(nullptr)
{
    auto* mainL = new QVBoxLayout(this);
    auto* namesL = new QHBoxLayout();
    auto* buttL = new QHBoxLayout();

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    butt = new QPushButton("Шукати", this);
    butt->setMinimumWidth(260);

    auto font = butt->font();
    font.setPointSize(11);
    butt->setFont(font);

    mainL->addWidget(line);
    mainL->addLayout(namesL);

    splitter = new QSplitter(Qt::Horizontal);
    stroyovaText = new QTextEdit(this);
    rcText = new QTextEdit(this);

    firstName = new HighlightLineEdit(this);
    secondName = new HighlightLineEdit(this);
    fatherName = new HighlightLineEdit(this);

    settingHighlightLineEdit(firstName);
    settingHighlightLineEdit(secondName);
    settingHighlightLineEdit(fatherName);

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

void CentralWidget::startHeavyWork(QString& rootDir, SearchedName& names, const QString prossecName, QThread* thread)
{
    thread = new QThread(this);
    worker = new HeavyWorkThread(rootDir, names, prossecName);

    worker->moveToThread(thread);

    connect(thread, &QThread::started,
        worker, &HeavyWorkThread::process);

    connect(worker, &HeavyWorkThread::finished,
        this, &CentralWidget::onHeavyWorkFinished);

    connect(worker, &HeavyWorkThread::finished,
        thread, &QThread::quit);

    connect(thread, &QThread::finished,
        worker, &QObject::deleteLater);

    connect(thread, &QThread::finished,
        thread, &QObject::deleteLater);

    thread->start();
}

void CentralWidget::CentralWidget::startLoading(QTextEdit* edit, QTimer*& timer)
{
    if (LoadingFrames.isEmpty())
        return; // safety

    int step = 0;

    if (timer) {
        timer->stop();
        timer->deleteLater();
    }

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]() mutable {
        if (step < 0 || step >= LoadingFrames.size())
            step = 0; // HARD safety reset

        edit->setHtml(
            QString(
                "<div style='font-size:18pt; font-weight:700; text-align:center;'>%1</div>")
                .arg(LoadingFrames.at(step)));

        step = (step + 1) % LoadingFrames.size();
    });

    timer->start(200);
}

void CentralWidget::stopLoading(QTimer*& timer)
{
    if (!timer)
        return;

    timer->stop();
    timer->deleteLater();
    timer = nullptr;
}

void CentralWidget::onButtonClicked()
{
    SearchedName names;
    names.last = firstName->text();
    names.first = secondName->text();
    names.father = fatherName->text();

    if (stroyova->isChecked() || rc->isChecked())
        butt->setEnabled(false);

    if (stroyova->isChecked()) {
        QString path = setting.loadFolder("STROYOVA_PATH");

        stroyovaText->setReadOnly(true);
        startLoading(stroyovaText, timerST);
        startHeavyWork(path, names, "STROYOVA", stroyovaThread);

        radButtState1 = true;
    }

    if (rc->isChecked()) {
        QString path = setting.loadFolder("PC_PATH");

        rcText->setReadOnly(true);
        startLoading(rcText, timerPC);
        startHeavyWork(path, names, "PC", pcThread);

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
        stopLoading(timerST);
        stroyovaText->setText(get<0>(outData));
        stroyovaText->setReadOnly(false);
    }

    if (get<2>(outData) == "PC") {
        qDebug() << "Change ~~PC~~";
        op2 = true;
        stopLoading(timerPC);
        rcText->setText(get<0>(outData));
        rcText->setReadOnly(false);
    }

    if (((radButtState1 == op1) && (radButtState2 == op2))) {
        radButtState1 = false;
        radButtState2 = false;
        op1 = false;
        op2 = false;
        butt->setEnabled(true);
    }
}
