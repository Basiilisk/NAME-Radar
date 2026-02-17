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
    QTabWidget* mainTabs = new QTabWidget(this);
    mainTabs->setDocumentMode(true);

    mainTabs->setStyleSheet(
        /* 1. ПЛОЩИНА З КОНТЕНТОМ (PANE) */
        "QTabWidget::pane {"
        "    background: #2b2b2b;" /* Головний колір фону (світліший сірий) */
        "    border: 1px solid #555555;" /* Рамка навколо всього контенту */
        "    top: 0px;" /* Зсув вгору для перекриття вкладок */
        "    border-radius: 6px;" /* Легке заокруглення всієї панелі */
        "    border-top-left-radius: 0px;" /* Робимо гострий кут зліва, де починається перша вкладка */
        "}"

        /* 2. ВСІ ВКЛАДКИ (за замовчуванням - неактивні) */
        "QTabBar::tab {"
        "    background: #2b2b2b;" /* Темний колір (ніби вони на задньому плані) */
        "    color: #888888;" /* 2ьмяний текст */
        "    padding: 8px 30px;" /* Широкі та зручні вкладки */
        "    border: 1px solid #555555;" /* Загальна рамка */
        "    border-bottom: 1px solid #555555;" /* Замикає рамку контенту знизу */
        "    border-top-left-radius: 7px;" /* Заокруглення верхніх кутів */
        "    border-top-right-radius: 7px;"
        "    margin-left: 2px;" /* Відступ між вкладками */
        "}"

        /* 3. АКТИВНА ВКЛАДКА */
        "QTabBar::tab:selected {"
        "    background: #1e1e1e;" /* ФОН ТОЧНО ТАКИЙ ЖЕ ЯК У PANE (#2b2b2b) */
        "    color: #ffffff;" /* Яскравий білий текст */
        "    border-bottom: 1px solid #1e1e1e;" /* МАГІЯ: нижня рамка стирає кордон з pane */
        "}"

        /* 4. ЕФЕКТ НАВЕДЕННЯ НА НЕАКТИВНІ ВКЛАДКИ */
        "QTabBar::tab:hover:!selected {"
        "    background: #252525;" /* Трохи висвітлюємо при наведенні миші */
        "    color: #dddddd;"
        "}");


    central = new CentralWidget(this);
    mainTabs->addTab(central, "Пошук");

    convertor = new ConvertorForm(this);
    mainTabs->addTab(convertor, "Конвертор");

    setCentralWidget(mainTabs);
}

MainWindow::~MainWindow()
{
    delete ui;
}
