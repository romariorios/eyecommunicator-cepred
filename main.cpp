#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Centro de Prevenção e Reabilitação da Pessoa com Deficiência");
    QCoreApplication::setOrganizationDomain("www.saude.ba.gov.br/cepred/");
    QCoreApplication::setApplicationName("CEPRED eye communicator");

    MainWindow w;
    w.show();

    return a.exec();
}
