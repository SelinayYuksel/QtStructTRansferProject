#ifndef SERVER_H
#define SERVER_H


//#include "message.h"
#include <QTcpServer>
#include <QObject>
#include <QTcpSocket>
class message;  // İleri Bildirim (Forward Declaration)

class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    ~server();

    void startServer(); //start server

public slots:
    void onNewConnection();
    void onReadyRead();
    void deleteSocket();

signals:
    void finished();
private:
    QTcpServer *_server;
    message *msg;

};

#endif // SERVER_H


//serverı başlat
//kapat
//yanıt oku
//yazdır
