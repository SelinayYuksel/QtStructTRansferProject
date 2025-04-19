#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTcpServer>


class message : public QObject {
    Q_OBJECT

public:

    struct person_info {   // ✅ Kişisel bilgileri içeren struct
        char name[50];
        int birthYear;
        int age;
        uint32_t crc;  // ✅ Veri doğrulaması için CRC ekledik
    };

    explicit message(QTcpSocket *socket, QObject *parent = nullptr);

    uint32_t calculateCRC(const uint8_t* data, size_t length);
    QByteArray serializePerson(const person_info& person);
    static person_info deserializePerson(const QByteArray& byteArray);
    void inputHandler();

private:
    QTcpSocket *socket;
    QTcpServer *server;
};

#endif // MESSAGE_H
