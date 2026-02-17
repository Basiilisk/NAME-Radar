#include "mainwindow.h"

#include <QApplication>

#include "Convertor/convertormanager.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    a.setWindowIcon(QIcon(":/icons/NameRadar.ico"));

    //w.show();

    const QString inputPath = R"(C:/Users/svyat/Desktop/НАКАЗИ/TEST_DATA/raw_STROYOVA)";
    const QString dataBasePath = R"(../../Convertor/DadaBase/NameRadarDB.db)";

    ConvertorManager convert;
    qDebug() << "\t__________START__________\n";
    convert.convertFiles(inputPath, dataBasePath);

    qDebug() << "\n\n\t__________FINISH__________";

    return a.exec();
}
