#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("NAME RADAR");
    resize(1100, 600);

    // 1. Створюємо головний контейнер вкладок
    mainTabs = new QTabWidget(this);
    // mainTabs->setDocumentMode(true);

    // mainTabs->setStyleSheet(
    //     /* 1. ПЛОЩИНА З КОНТЕНТОМ (PANE) */
    //     "QTabWidget::pane {"
    //     "    background: #2b2b2b;" /* Головний колір фону (світліший сірий) */
    //     "    border: 1px solid #555555;" /* Рамка навколо всього контенту */
    //     "    top: 0px;" /* Зсув вгору для перекриття вкладок */
    //     "    border-radius: 6px;" /* Легке заокруглення всієї панелі */
    //     "    border-top-left-radius: 0px;" /* Робимо гострий кут зліва, де починається перша вкладка */
    //     "}"

    //     /* 2. ВСІ ВКЛАДКИ (за замовчуванням - неактивні) */
    //     "QTabBar::tab {"
    //     "    background: #2b2b2b;" /* Темний колір (ніби вони на задньому плані) */
    //     "    color: #888888;" /* 2ьмяний текст */
    //     "    padding: 8px 30px;" /* Широкі та зручні вкладки */
    //     "    border: 1px solid #555555;" /* Загальна рамка */
    //     "    border-bottom: 1px solid #555555;" /* Замикає рамку контенту знизу */
    //     "    border-top-left-radius: 7px;" /* Заокруглення верхніх кутів */
    //     "    border-top-right-radius: 7px;"
    //     "    margin-left: 2px;" /* Відступ між вкладками */
    //     "}"

    //     /* 3. АКТИВНА ВКЛАДКА */
    //     "QTabBar::tab:selected {"
    //     "    background: #1e1e1e;" /* ФОН ТОЧНО ТАКИЙ ЖЕ ЯК У PANE (#2b2b2b) */
    //     "    color: #ffffff;" /* Яскравий білий текст */
    //     "    border-bottom: 1px solid #1e1e1e;" /* МАГІЯ: нижня рамка стирає кордон з pane */
    //     "}"

    //     /* 4. ЕФЕКТ НАВЕДЕННЯ НА НЕАКТИВНІ ВКЛАДКИ */
    //     "QTabBar::tab:hover:!selected {"
    //     "    background: #252525;" /* Трохи висвітлюємо при наведенні миші */
    //     "    color: #dddddd;"
    //     "}");
    // 1. Робимо фон самого вікна глибоким темним (щоб неактивні вкладки виділялися)
    this->setStyleSheet("QMainWindow { background-color: #121212; }");

    // 2. Застосовуємо стиль до вкладок
    mainTabs->setStyleSheet(QString(R"(
        /* 1. ПЛОЩИНА З КОНТЕНТОМ (PANE) */
        QTabWidget::pane {
            background-color: #1e1e1e;     /* Глибокий темний колір (повертає контраст) */
            border: 1px solid #333333;     /* Темніша і делікатніша рамка */
            border-radius: 6px;
            top: -1px;                     /* Зсув для безшовності */
        }

        /* 2. ЗСУВ ВКЛАДОК (ВІДСТУП ЗЛІВА) */
        QTabWidget::tab-bar {
            left: 25px;
        }

        /* 3. НЕАКТИВНІ ВКЛАДКИ */
        QTabBar::tab {
            background-color: transparent; /* Зливаються з фоном вікна (#121212) */
            color: #888888;
            border: 1px solid transparent;
            border-bottom: 1px solid #333333; /* Лінія під неактивними вкладками */
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            padding: 8px 25px;
            margin-right: 4px;
        }

        /* Ефект наведення миші на неактивні вкладки */
        QTabBar::tab:hover:!selected {
            background-color: #1a1a1a;     /* Ледь-ледь світлішають при наведенні */
            border-bottom: 1px solid #333333;
        }

        /* 4. АКТИВНА ВКЛАДКА (БЕЗШОВНА) */
        QTabBar::tab:selected {
            background-color: #1e1e1e;     /* ФОН ТОЧНО ЯК У ПАНЕЛІ (#1e1e1e) */
            color: #ffffff;                /* Білий текст */
            border: 1px solid #333333;
            border-bottom: 1px solid #1e1e1e; /* МАГІЯ: стирає лінію під собою */
        }
    )"));

    central = new CentralWidget(this);
    mainTabs->addTab(central, "Пошук");

    convertor = new ConvertorForm(this);
    mainTabs->addTab(convertor, "Конвертор");

    setCentralWidget(mainTabs);

    mainTabs->setCurrentIndex(1);

    connect(central, &CentralWidget::enabledTabs, this, &MainWindow::setEnabledTabs);
    connect(convertor, &ConvertorForm::enabledTabs, this, &MainWindow::setEnabledTabs);

    // mainTabs->setStyleSheet("background-color: rgba(0, 0, 0, 30);"); // Fully transparent white background
    // setStyleSheet("background-color: rgba(0, 0, 0, 30);"); // Fully transparent white background
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEnabledTabs(bool enable)
{
    for (int i = 0; i < mainTabs->count(); ++i) {
        if (i == mainTabs->currentIndex())
            continue;
        mainTabs->setTabEnabled(i, enable);
    }
}
