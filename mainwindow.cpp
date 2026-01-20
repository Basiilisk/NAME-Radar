#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "centralwidget.h"
#include "jsonsettings.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    central = new CentralWidget(this);
    setCentralWidget(central);

    setWindowTitle("NAME RADAR");
    resize(500, 400);

    createMenus();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    JSONSettings setting;
    setting.saveSettings();

    QMainWindow::closeEvent(event);
}

void MainWindow::createMenus()
{
    QMenuBar* mb = menuBar();

    QMenu* fileMenu = mb->addMenu("Шляхи");

    fileMenu->addAction("Стройова", this, [this]() {
        JSONSettings setting;
        setting.chooseAndSaveFolder("STROYOVA_PATH");
    });

    fileMenu->addAction("РС", this, [this]() {
        JSONSettings setting;
        setting.chooseAndSaveFolder("PC_PATH");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
