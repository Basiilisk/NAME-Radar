#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "centralwidget.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    central = new CentralWidget(this);
    setCentralWidget(central);

    setWindowTitle("NAME RADAR");
    resize(500, 400);
}

MainWindow::~MainWindow()
{
    delete ui;
}
