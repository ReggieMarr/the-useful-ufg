//this is where i should store my global variabels
//aka mySQL setup and or kepserverEX

//#include "nnconfigwindow.h"
#include "startupwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName( "EDC Electronic Design Chemnitz GmbH" );
    QApplication::setOrganizationDomain( "ed-chemnitz.de" );
    QApplication::setApplicationName( "QModBus" );


    //NNConfigWindow w;
    StartupWindow w;
    w.show();
    return a.exec();
}
