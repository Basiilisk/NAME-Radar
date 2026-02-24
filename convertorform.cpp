#include "convertorform.h"

#include "jsonsettings.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QStyle>
#include <QVBoxLayout>

#include "StructUSE.h"

const QString colorPath = "#53d2ff";
const QString colorNew = "#448D76";
const QString colorMod = "#FB8604";
const QString colorDel = "#EC3642";

void setStyleLabel(QLabel* label, const QString& color)
{
    label->setMinimumHeight(40);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        QString(
            "QLabel {"
            "    color: %1;" /* Колір тексту (наприклад, світло-блакитний) */
            "    background-color: #2b2b2b;" /* Колір фону */
            "    font-size: 14px;" /* Розмір шрифту */
            "    font-weight: bold;" /* Жирний текст (можна normal) */
            "    font-family: Arial;" /* Тип шрифту */
            "    padding: 6px 12px;" /* Внутрішні відступи (верх/низ 5px, ліво/право 10px) */
            "    border: 1px solid #555555;" /* Рамка */
            "    border-radius: 4px;" /* Заокруглення кутів рамки */
            "}")
            .arg(color));
}

ConvertorForm::ConvertorForm(QWidget* parent)
    : QWidget(parent)
    , converManager(std::make_unique<ConvertorManager>())
{
    auto* mainL = new QVBoxLayout(this);
    auto* logL = new QHBoxLayout();
    auto* rightL = new QVBoxLayout();
    auto* statL = new QVBoxLayout();
    auto* pathL = new QVBoxLayout();

    auto* buttL = new QHBoxLayout();

    // QFrame* line = new QFrame(this);
    // line->setFrameShape(QFrame::HLine);
    // line->setFrameShadow(QFrame::Sunken);

    // mainL->addWidget(line);
    mainL->addLayout(logL);
    mainL->addLayout(buttL);

    logText = new QTextEdit(this);
    logText->setPlaceholderText("Інфа по конвертації");
    // Додай цей код у конструктор CentralWidget після створення stroyovaText та rcText

    QString glassStyle = R"(
        QTextEdit {
            /* 150 - це рівень прозорості. Змінюй його від 0 до 255, щоб знайти ідеал */
            background-color: rgba(0, 0, 0, 20);
            color: #ffffff;
            border: 1px solid #444444;
            border-radius: 6px;
            padding: 5px;
        }
        QTextEdit:focus {
            border: 1px solid #555555; /* Трохи світліша рамка при кліку */
        }
    )";

    logText->setStyleSheet(glassStyle);

    logL->addWidget(logText);

    QLabel* newF = new QLabel(this);
    newF->setText("Нові файли:\t239/239");
    setStyleLabel(newF, colorNew);

    QLabel* modF = new QLabel(this);
    modF->setText("Змінені файли:\t0/239");
    setStyleLabel(modF, colorMod);

    QLabel* delF = new QLabel(this);
    delF->setText("Видалені файли:\t23/239");
    setStyleLabel(delF, colorDel);

    logL->addLayout(rightL);
    // rightL->addSpacing(150);
    rightL->addLayout(pathL, 4);
    // rightL->addSpacing(150);
    rightL->addStretch(3);
    rightL->addLayout(statL, 3);
    // rightL->addSpacing(150);

    JSONSettings setting;

    QLineEdit* stroyovaPath = new QLineEdit(this);
    stroyovaPath->setPlaceholderText("Оберіть шлях до папки з наказами по стройовій");
    stroyovaPath->setMinimumHeight(30);

    // ОДРАЗУ ЗАВАНТАЖУЄМО ЗБЕРЕЖЕНИЙ ШЛЯХ (якщо він є)
    stroyovaPath->setText(setting.loadFolder("STROYOVA_PATH"));

    stroyovaPath->setStyleSheet(
        QString(
            "QLineEdit {"
            "    color: %1;"
            "    background-color: #2b2b2b;"
            "    font-size: 12px;"
            "    font-weight: bold;"
            "    font-family: Arial;"
            "    padding: 6px 12px;"
            "    border: 1px solid #555555;"
            "    border-radius: 4px;"
            "}")
            .arg(colorPath));

    // 1. Створюємо дію (Action) з іконкою
    QIcon sysFolderIcon = style()->standardIcon(QStyle::SP_DirIcon);
    QAction* browseActionStroyova = new QAction(sysFolderIcon, "Огляд", this);

    // 2. Додаємо цю дію всередину QLineEdit у праву частину
    stroyovaPath->addAction(browseActionStroyova, QLineEdit::TrailingPosition);

    // 3. Підключаємо натискання на іконку (QFileDialog сам гарантує, що папка існує)
    connect(browseActionStroyova, &QAction::triggered, this, [this, stroyovaPath]() {
        QString dir = QFileDialog::getExistingDirectory(
            this,
            "Оберіть папку",
            stroyovaPath->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!dir.isEmpty()) {
            stroyovaPath->setText(dir);
            JSONSettings settingSaver;
            settingSaver.saveFolder("STROYOVA_PATH", dir);
        }
    });

    // 4. ПЕРЕВІРКА ПРИ РУЧНОМУ ВВЕДЕННІ (Ctrl+V або друк тексту)
    // Сигнал editingFinished спрацьовує, коли користувач натискає Enter або прибирає фокус з поля
    connect(stroyovaPath, &QLineEdit::editingFinished, this, [this, stroyovaPath]() {
        QString inputPath = stroyovaPath->text().trimmed(); // trimmed() прибирає випадкові пробіли по краях

        JSONSettings settingSaver;

        // Дозволяємо користувачу повністю стерти шлях, якщо він хоче його очистити
        if (inputPath.isEmpty()) {
            settingSaver.saveFolder("STROYOVA_PATH", "");
            return;
        }

        // Перевіряємо, чи існує така папка фізично на диску
        QDir dir(inputPath);
        if (!dir.exists()) {
            QMessageBox::warning(this, "Помилка шляху", "Вказаної папки не існує!");

            // Відміняємо зміни: повертаємо в поле останній збережений ВАЛІДНИЙ шлях
            stroyovaPath->setText(settingSaver.loadFolder("STROYOVA_PATH"));
        } else {
            // Якщо все добре — зберігаємо новий шлях
            settingSaver.saveFolder("STROYOVA_PATH", inputPath);
        }
    });

    // ПРОСУНУТА ФІШКА (За бажанням):
    // Якщо користувач не натисне кнопку, а просто вставить шлях текстом (Ctrl+V) і натисне Enter:
    connect(stroyovaPath, &QLineEdit::editingFinished, this, [stroyovaPath]() {
        JSONSettings settingSaver;
        settingSaver.saveFolder("STROYOVA_PATH", stroyovaPath->text());
    });

    QLineEdit* rsPath = new QLineEdit(this);
    rsPath->setPlaceholderText("Оберіть шлях до папки з РСними наказами");
    rsPath->setMinimumHeight(30);

    // ОДРАЗУ ЗАВАНТАЖУЄМО ЗБЕРЕЖЕНИЙ ШЛЯХ (якщо він є)
    rsPath->setText(setting.loadFolder("RS_PATH"));

    rsPath->setStyleSheet(
        QString(
            "QLineEdit {"
            "    color: %1;"
            "    background-color: #2b2b2b;"
            "    font-size: 12px;"
            "    font-weight: bold;"
            "    font-family: Arial;"
            "    padding: 6px 12px;"
            "    border: 1px solid #555555;"
            "    border-radius: 4px;"
            "}")
            .arg(colorPath));

    // 1. Створюємо дію (Action) з іконкою
    QAction* browseActionRS = new QAction(sysFolderIcon, "Огляд", this);

    // 2. Додаємо цю дію всередину QLineEdit у праву частину
    rsPath->addAction(browseActionRS, QLineEdit::TrailingPosition);

    // 3. Підключаємо натискання на іконку (QFileDialog сам гарантує, що папка існує)
    connect(browseActionRS, &QAction::triggered, this, [this, rsPath]() {
        QString dir = QFileDialog::getExistingDirectory(
            this,
            "Оберіть папку",
            rsPath->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!dir.isEmpty()) {
            rsPath->setText(dir);
            JSONSettings settingSaver;
            settingSaver.saveFolder("RS_PATH", dir);
        }
    });

    // 4. ПЕРЕВІРКА ПРИ РУЧНОМУ ВВЕДЕННІ (Ctrl+V або друк тексту)
    // Сигнал editingFinished спрацьовує, коли користувач натискає Enter або прибирає фокус з поля
    connect(rsPath, &QLineEdit::editingFinished, this, [this, rsPath]() {
        QString inputPath = rsPath->text().trimmed(); // trimmed() прибирає випадкові пробіли по краях

        JSONSettings settingSaver;

        // Дозволяємо користувачу повністю стерти шлях, якщо він хоче його очистити
        if (inputPath.isEmpty()) {
            settingSaver.saveFolder("RS_PATH", "");
            return;
        }

        // Перевіряємо, чи існує така папка фізично на диску
        QDir dir(inputPath);
        if (!dir.exists()) {
            QMessageBox::warning(this, "Помилка шляху", "Вказаної папки не існує!");

            // Відміняємо зміни: повертаємо в поле останній збережений ВАЛІДНИЙ шлях
            rsPath->setText(settingSaver.loadFolder("RS_PATH"));
        } else {
            // Якщо все добре — зберігаємо новий шлях
            settingSaver.saveFolder("RS_PATH", inputPath);
        }
    });

    // ПРОСУНУТА ФІШКА (За бажанням):
    // Якщо користувач не натисне кнопку, а просто вставить шлях текстом (Ctrl+V) і натисне Enter:
    connect(rsPath, &QLineEdit::editingFinished, this, [rsPath]() {
        JSONSettings settingSaver;
        settingSaver.saveFolder("RS_PATH", rsPath->text());
    });

    pathL->addWidget(stroyovaPath);
    pathL->addWidget(rsPath);

    statL->addStretch(7);
    statL->addWidget(newF, 1);
    statL->addWidget(modF, 1);
    statL->addWidget(delF, 1);

    convert = new QPushButton("Конвертувати", this);
    cancel = new QPushButton("Відмінити конвертування", this);
    deleteAllRS_BD = new QPushButton("Очистити БД РС наказами", this);
    deleteAllStroyouva_BD = new QPushButton("Очистити БД Стройовими наказами", this);

    // buttL->addSpacing(100);
    buttL->addWidget(convert);
    buttL->addWidget(cancel);
    // buttL->addSpacing(150);
    buttL->addWidget(deleteAllRS_BD);
    buttL->addWidget(deleteAllStroyouva_BD);
    // buttL->addSpacing(100);

    // Кнопка запуску конвертації
    connect(convert, &QPushButton::clicked, this, [&]() {
        JSONSettings setting;
        QString pathStroyova = setting.loadFolder("STROYOVA_PATH");
        QString pathRC = setting.loadFolder("RS_PATH");

        if (pathStroyova.isEmpty() && pathRC.isEmpty()) {
            QMessageBox::warning(this, "Помилка", "Спочатку вкажіть шляхи до папок!");
            return;
        }

        convert->setEnabled(false);

        // Конвертуємо Стройову (якщо шлях вказано)
        if (!pathStroyova.isEmpty()) {
            const QString dbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_STROYOVA.db";
            converManager->convertFiles(Names::Stroyova, pathStroyova, dbPath, logText);
        }

        // Конвертуємо РС (якщо шлях вказано)
        if (!pathRC.isEmpty()) {
            const QString dbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_RS.db";
            converManager->convertFiles(Names::Rs, pathRC, dbPath, logText);
        }

        convert->setEnabled(true);
        QMessageBox::information(this, "Готово", "Процес конвертації завершено!");
    });

    connect(cancel, &QPushButton::clicked, converManager.get(), &ConvertorManager::on_btnCancel_clicked);

    connect(deleteAllRS_BD, &QPushButton::clicked, this, [&]() {
        // Запитуємо підтвердження у користувача
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Підтвердження",
            "Ви впевнені, що хочете видалити\nвсі конвертовані дані РСних наказів?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString rsDbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_RS.db";

            bool rsDeleted = QFile::remove(rsDbPath);

            if (rsDeleted) {
                QMessageBox::information(this, "Готово", "Бази даних успішно очищені (видалені).");
            } else {

                QFile file(rsDbPath);
                // 3. If it failed, it is likely Read-Only. Let's force permissions.
                QFileDevice::Permissions currentPermissions = file.permissions();

                // Add Write permission for the current user
                if (file.setPermissions(currentPermissions | QFileDevice::WriteUser)) {

                    // 4. Try deleting it again now that we have permission
                    if (file.remove()) {
                        QMessageBox::information(this, "Готово", "нарешті видалено");
                    }

                    else {
                        QMessageBox::information(this, "Інформація", "Жопа");
                    }
                }
            }
        }
    });

    connect(deleteAllStroyouva_BD, &QPushButton::clicked, this, [&]() {
        // Запитуємо підтвердження у користувача
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Підтвердження",
            "Ви впевнені, що хочете видалити\nвсі конвертовані дані наказів по стройовій?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString stroyDbPath = QCoreApplication::applicationDirPath() + "/NameRadarDB_STROYOVA.db";

            bool stroyDeleted = QFile::remove(stroyDbPath);

            if (stroyDeleted) {
                QMessageBox::information(this, "Готово", "Бази даних успішно очищені (видалені).");
            } else {
                QFile file(stroyDbPath);
                // 3. If it failed, it is likely Read-Only. Let's force permissions.
                QFileDevice::Permissions currentPermissions = file.permissions();

                // Add Write permission for the current user
                if (file.setPermissions(currentPermissions | QFileDevice::WriteUser)) {

                    // 4. Try deleting it again now that we have permission
                    if (file.remove()) {
                        QMessageBox::information(this, "Готово", "нарешті видалено");
                    }

                    else {
                        QMessageBox::information(this, "Інформація", "Жопа");
                    }
                }
            }
        }
    });
}
