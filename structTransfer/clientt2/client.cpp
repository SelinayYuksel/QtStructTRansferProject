/* client.cpp */
#include "client.h"
#include <QDebug>

client::client(QObject *parent) : QObject(parent), socket(nullptr), msg(nullptr) {}

void client::socketStart() {
    socket = new QTcpSocket(this);
    msg = new message(socket, this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket->connectToHost("localhost", 8080);
    if (!socket->waitForConnected(1000)) {
        qDebug() << "Error..." << socket->errorString();
    }
}

void client::connected() {
    qDebug() << "Bağlantı başarılı...";
    msg->inputHandler();
}

void client::disconnected() {
    if (socket) {
        socket->close();
        socket->deleteLater();
        qDebug() << "Bağlantı kapandı..";
    }
}

client::~client() {
    if (msg) {
        msg->deleteLater();
        msg = nullptr;
    }
}

void client::readyRead() {
    QByteArray response = socket->readAll();
    qDebug() << "Sunucudan Gelen Yanıt: " << response;
}
