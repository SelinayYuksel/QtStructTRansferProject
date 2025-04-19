
/* main.cpp */
#include <QApplication>
#include "client.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    client *socket = new client();
    socket->socketStart();
    return a.exec();
}
