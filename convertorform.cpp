#include "convertorform.h"

#include <QFrame>
#include <QVBoxLayout>

ConvertorForm::ConvertorForm(QWidget* parent)
    : QWidget(parent)
{
    auto* mainL = new QVBoxLayout(this);
    auto* namesL = new QHBoxLayout();
    auto* buttL = new QHBoxLayout();

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // QVBoxLayout* convLayout = new QVBoxLayout(converterTab);

    // // Створюємо елементи для вкладки конвертера
    // QLabel* labelTitle = new QLabel("<b>Налаштування шляхів та конвертація</b>", this);
    // QPushButton* btnPathStroyova = new QPushButton("Вказати папку: СТРОЙОВА", this);
    // QPushButton* btnPathRC = new QPushButton("Вказати папку: РС", this);

    // QFrame* line = new QFrame(this);
    // line->setFrameShape(QFrame::HLine);
    // line->setFrameShadow(QFrame::Sunken);

    // QPushButton* btnStartConvert = new QPushButton("Запустити конвертацію", this);
    // btnStartConvert->setMinimumHeight(40);

    // // Додаємо віджети на сторінку
    // convLayout->addWidget(labelTitle);
    // convLayout->addSpacing(10);
    // convLayout->addWidget(btnPathStroyova);
    // convLayout->addWidget(btnPathRC);
    // convLayout->addSpacing(20);
    // convLayout->addWidget(line);
    // convLayout->addSpacing(20);
    // convLayout->addWidget(btnStartConvert);
    // convLayout->addStretch(); // Виштовхує всі кнопки вгору

    // mainTabs->addTab(converterTab, "База даних (Конвертер)");

    // // Встановлюємо вкладки як головний елемент вікна
    // setCentralWidget(mainTabs);

    // // ==========================================
    // // ПІДКЛЮЧЕННЯ КНОПОК
    // // ==========================================

    // // Кнопки вибору шляхів (замість старого меню)
    // connect(btnPathStroyova, &QPushButton::clicked, this, [this]() {
    //     JSONSettings setting;
    //     if (setting.chooseAndSaveFolder("STROYOVA_PATH", this)) {
    //         QMessageBox::information(this, "Успіх", "Шлях для СТРОЙОВА успішно збережено!");
    //     }
    // });

    // connect(btnPathRC, &QPushButton::clicked, this, [this]() {
    //     JSONSettings setting;
    //     if (setting.chooseAndSaveFolder("PC_PATH", this)) {
    //         QMessageBox::information(this, "Успіх", "Шлях для РС успішно збережено!");
    //     }
    // });

    // // Кнопка запуску конвертації
    // connect(btnStartConvert, &QPushButton::clicked, this, [this, btnStartConvert]() {
    //     JSONSettings setting;
    //     QString pathStroyova = setting.loadFolder("STROYOVA_PATH");
    //     QString pathRC = setting.loadFolder("PC_PATH");

    //     if (pathStroyova.isEmpty() && pathRC.isEmpty()) {
    //         QMessageBox::warning(this, "Помилка", "Спочатку вкажіть шляхи до папок!");
    //         return;
    //     }

    //     btnStartConvert->setEnabled(false);
    //     btnStartConvert->setText("Конвертація виконується...");

    //     ConvertorManager convert;

    //     // Конвертуємо Стройову (якщо шлях вказано)
    //     if (!pathStroyova.isEmpty()) {
    //         QString dbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_STROYOVA.db";
    //         convert.convertFiles(pathStroyova, dbPath);
    //     }

    //     // Конвертуємо РС (якщо шлях вказано)
    //     if (!pathRC.isEmpty()) {
    //         QString dbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_PC.db";
    //         convert.convertFiles(pathRC, dbPath);
    //     }

    //     btnStartConvert->setEnabled(true);
    //     btnStartConvert->setText("Запустити конвертацію");
    //     QMessageBox::information(this, "Готово", "Процес конвертації завершено!");
    // });
}
