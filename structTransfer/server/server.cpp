#include "server.h"
#include "message.h"
#include "qdatetime.h"

#include <QDebug>
#include <QTcpSocket>
#include <QThread>
server::server(QObject *parent) : QTcpServer(parent),msg(nullptr){

}

server::~server(){
    _server->close();

}

void server::startServer(){

    if(this->listen(QHostAddress::Any,8080))
    {
        qDebug() << "Sunucu Başaltıldı...";
    }
    else{
        qDebug() << "Sunucu Başlatılamadı...";
    }


    connect(this, &QTcpServer::newConnection, this, &server::onNewConnection);
    onReadyRead();


}



//socketi yi bağladık
void server::onNewConnection()
{
    QTcpSocket *socket = this->nextPendingConnection();
    if(!socket)
    {
        qDebug() << "Socket Oluşturulamadı...";
        return;
    }
    qDebug() << "Yeni istemci bağlandı! " << socket->peerAddress().toString() << ":" << socket->peerPort();
    connect(socket, &QTcpSocket::readyRead,this, &server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected,this, &server::deleteSocket);

}
void server::onReadyRead(){
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        QByteArray data = clientSocket->readAll();

        // QByteArray verisini deserialize etmek için doğru kullanım
    message::person_info person = message::deserializePerson(data);


        // Geçerli yılı al
        int currentYear = QDate::currentDate().year();

        // Doğum yılı ile yaş hesapla
        int age = currentYear - person.birthYear;
        qDebug() << "Alınan Doğum Yılı: " << person.birthYear;
        qDebug() << "Hesaplanan Yaş: " << age;

        // Yaşın geçerli olup olmadığını kontrol et
        if (age < 0) {
            qDebug() << "Hatalı yaş hesaplaması!";
        } else {
           qDebug()<< person.name << age << "yaşındadır.";
        }

        // Diğer işlemler...




    }
}
void server::deleteSocket(){

}
