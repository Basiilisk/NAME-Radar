#include "convertortotxt.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    a.setWindowIcon(QIcon(":/icons/NameRadar.ico"));

    //w.show();

    QString inputPath = R"(C:\Users\svyat\Desktop\НАКАЗИ\TEST_DATA\raw_STROYOVA)";
    QString outputPath = R"(C:\Users\svyat\Desktop\НАКАЗИ\TEST_DATA\converted)";


    ConvertorToTxt convert;
    qDebug() << "\t__________START__________\n";
    convert.processDirectory(inputPath, outputPath);

    qDebug() << "\n\n\t__________FINISH__________";


    return a.exec();
}
