#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <cstring>
#include <cstdint>
#include <cstring>
class message : public QObject {
    Q_OBJECT

public:
    explicit message(QTcpSocket *socket, QObject *parent = nullptr);
    void inputHandler();



    // ✅ Kişisel bilgi yapısını (struct) burada tanımlıyoruz
    struct person_info {
        char name[50];      // Kullanıcı adı
        int birth_year;     // Doğum yılı
        bool ok;
        int age;            // Yaş
        uint32_t crc;       // CRC kontrolü

        person_info() {
            memset(name, 0, sizeof(name));
            birth_year = 0;
            age = 0;
            crc = 0;
        }
    };

    // ✅ Struct'ı serialize ve deserialize eden fonksiyonlar
    QByteArray serializePerson(const person_info& person);
    person_info deserializePerson(const QByteArray& byteArray);

    // ✅ CRC hesaplama fonksiyonu
    uint32_t calculateCRC(const uint8_t* data, size_t length);

    // ✅ Kullanıcı girişleriyle struct oluşturup sunucuya gönderen fonksiyon

private:
    QTcpSocket *socket;
};

#endif // MESSAGE_H
