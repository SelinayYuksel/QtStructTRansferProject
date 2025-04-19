#include <QCoreApplication>
#include "server.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    server _server;
    _server.startServer();
    return a.exec();
}
