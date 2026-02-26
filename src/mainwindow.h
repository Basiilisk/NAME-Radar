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

public slots:
    void setEnabledTabs(bool enable);

private:
    Ui::MainWindow* ui;

    CentralWidget* central = nullptr;
    ConvertorForm* convertor = nullptr;
    QTabWidget* mainTabs = nullptr;
};
