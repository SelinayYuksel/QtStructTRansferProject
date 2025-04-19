/* client.h */
#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
#include <QTcpSocket>
#include <QObject>

class client : public QObject {
    Q_OBJECT
public:
    explicit client(QObject *parent = nullptr);
    ~client();
    void socketStart();
    void send_btn_clicked();
    QTcpSocket *getSocket() { return socket; }

public slots:
    void connected();
    void disconnected();
    void readyRead();

private:
    QTcpSocket *socket;
    message *msg;
};

#endif // CLIENT_H
