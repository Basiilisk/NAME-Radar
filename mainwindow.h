#pragma once

#include "centralwidget.h"
#include "convertorform.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void createMenus();

private:
    Ui::MainWindow* ui;

    CentralWidget* central;
    ConvertorForm* convertor;
};
