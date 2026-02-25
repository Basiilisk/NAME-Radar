#include "centralwidget.h"

#include <QCoreApplication>
#include <QLayout>
#include <QPushButton>
#include <QTimer>

#include "highlightlineedit.h"

void settingHighlightLineEdit(HighlightLineEdit* lineEdit)
{
    lineEdit->setMinimumHeight(26);
    auto font = lineEdit->font();
    font.setPointSize(11);
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

    // QFrame* line = new QFrame(this);
    // line->setFrameShape(QFrame::HLine);
    // line->setFrameShadow(QFrame::Sunken);

    butt = new QPushButton("Шукати", this);
    butt->setMinimumWidth(260);

    auto font = butt->font();
    font.setPointSize(11);
    butt->setFont(font);

    // mainL->addWidget(line);
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

    stroyovaCB = new QCheckBox("СТРОЙОВА", this);
    bool strVal = setting.radioBtnLoad("STROYOVA_CENTR_BTN");
    stroyovaCB->setChecked(strVal);
    stroyovaChanged(strVal);

    rcCB = new QCheckBox("РС", this);
    bool rcVal = setting.radioBtnLoad("RC_CENTR_BTN");
    rcCB->setChecked(rcVal);
    rcChanged(rcVal);

    auto* stroyovaL = new QVBoxLayout();
    stroyovaL->addWidget(stroyovaText);

    auto* rcL = new QVBoxLayout();
    rcL->addWidget(rcText);

    QString glassStyle = R"(
        QTextEdit {
            /* Задаємо напівпрозорий колір самій рамці віджета */
            background-color: transparent; /* Зливаються з фоном вікна (#121212) */
            color: #ffffff;
            border: 1px solid #444444;
            border-radius: 6px;
            padding: 5px;
        }
        QTextEdit viewport {
            /* НАЙГОЛОВНІШИЙ РЯДОК: робимо внутрішню зону повністю прозорою */
            background-color: transparent; /* Зливаються з фоном вікна (#121212) */
        }
        QTextEdit:focus {
            border: 2px solid #555555;
        }
    )";

    stroyovaText->setStyleSheet(glassStyle);
    rcText->setStyleSheet(glassStyle);

    splitter->addWidget(stroyovaText);
    splitter->addWidget(rcText);

    namesL->addWidget(firstName);
    namesL->addWidget(secondName);
    namesL->addWidget(fatherName);
    namesL->addWidget(stroyovaCB);
    namesL->addWidget(rcCB);

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

    connect(butt, &QPushButton::clicked, this, &CentralWidget::onButtonClicked);

    connect(this, &CentralWidget::enabledTabs, butt, &QPushButton::setEnabled);

    connect(stroyovaCB, &QCheckBox::toggled, this, &CentralWidget::stroyovaChanged);

    connect(rcCB, &QCheckBox::toggled, this, &CentralWidget::rcChanged);
}

void CentralWidget::startHeavyWork(const QString& dbPath, SearchedName& names, const QString prossecName, QThread*& thread)
{
    // Захист від повторного запуску
    if (thread && thread->isRunning()) {
        return;
    }

    // Якщо потік закінчив роботу, але вказівник ще не очистився
    if (thread && !thread->isRunning()) {
        thread = nullptr;
    }

    thread = new QThread(this);

    auto* worker = new HeavyWorkThread(dbPath, names, prossecName);

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &HeavyWorkThread::process);
    connect(worker, &HeavyWorkThread::finished, this, &CentralWidget::onHeavyWorkFinished);
    connect(worker, &HeavyWorkThread::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    // МАГІЯ: Як тільки об'єкт потоку фізично видалиться з пам'яті,
    // ми безпечно обнуляємо вказівник (stroyovaThread або rsThread)
    QThread** threadPtr = &thread;
    connect(thread, &QObject::destroyed, this, [threadPtr]() {
        *threadPtr = nullptr;
    });

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

        edit->setText(LoadingFrames.at(step));
        QFont font = edit->font();
        font.setBold(true);
        font.setPointSize(18);
        edit->setFont(font);
        edit->setAlignment(Qt::AlignCenter);

        step = (step + 1) % LoadingFrames.size();
    });

    timer->start(100);
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
    names.last = firstName->text().trimmed();
    names.first = secondName->text().trimmed();
    names.father = fatherName->text().trimmed();

    if (stroyovaCB->isChecked() || rcCB->isChecked())
        enabledTabs(false);

    if (stroyovaCB->isChecked()) {
        // Шлях до БД Стройової біля програми
        QString dbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_STROYOVA.db";

        stroyovaText->setReadOnly(true);
        startLoading(stroyovaText, timerST);
        startHeavyWork(dbPath, names, Names::Stroyova, stroyovaThread);

        radButtState1 = true;
    }

    if (rcCB->isChecked()) {
        // Шлях до БД РС біля програми
        QString dbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_RS.db";

        rcText->setReadOnly(true);
        startLoading(rcText, timerRS);
        startHeavyWork(dbPath, names, Names::Rs, rsThread);

        radButtState2 = true;
    }
}

void CentralWidget::stroyovaChanged(bool state)
{
    if (state) {
        stroyovaText->show();
        setting.radioBtnSave("STROYOVA_CENTR_BTN", true);
    } else {
        stroyovaText->hide();
        stroyovaText->clear();
        setting.radioBtnSave("STROYOVA_CENTR_BTN", false);
    }
}

void CentralWidget::rcChanged(bool state)
{
    if (state) {
        rcText->show();
        setting.radioBtnSave("RC_CENTR_BTN", true);
    } else {
        rcText->hide();
        rcText->clear();
        setting.radioBtnSave("RC_CENTR_BTN", false);
    }
}

void CentralWidget::onHeavyWorkFinished(std::tuple<QString, OutputData, const QString> outData)
{
    auto showText = [&](QTextEdit* textEdit, bool& op) {
        op = true;

        QFont font = textEdit->font();
        font.setBold(false);
        font.setPointSize(9);
        textEdit->setFont(font);

        textEdit->setText(get<0>(outData));
        textEdit->setReadOnly(false);
    };

    std::tuple<QString, OutputData, const QString> data = outData;

    if (get<2>(outData) == Names::Stroyova) {
        qDebug() << "Change ~~STROYOVA~~";

        stopLoading(timerST);

        showText(stroyovaText, op1);
    }

    if (get<2>(outData) == Names::Rs) {
        qDebug() << "Change ~~RS~~";

        stopLoading(timerRS);

        showText(rcText, op2);
    }

    if (((radButtState1 == op1) && (radButtState2 == op2))) {
        radButtState1 = false;
        radButtState2 = false;
        op1 = false;
        op2 = false;
        enabledTabs(true);
    }
}
