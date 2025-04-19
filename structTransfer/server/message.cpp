#include "message.h"
#include <QDataStream>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <cstring>

message::message(QTcpSocket *socket, QObject *parent) : QObject(parent), socket(socket) {}

// ✅ CRC hesaplama fonksiyonu
uint32_t message::calculateCRC(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc ^ 0xFFFFFFFF;
}

// ✅ Kişisel bilgileri bayt dizisine çevirme (Serileştirme)
QByteArray message::serializePerson(const person_info& person) {
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.writeRawData(reinterpret_cast<const char*>(&person), sizeof(person_info));
    return byteArray;
}

// ✅ Bayt dizisini `person_info` struct'ına çevirme (Deserialize)
message::person_info message::deserializePerson(const QByteArray& byteArray) {
    person_info person;
    QDataStream stream(byteArray);
    stream.readRawData(reinterpret_cast<char*>(&person), sizeof(person_info));
    return person;
}


// ✅ Kullanıcıdan bilgileri alıp sunucuya gönderen fonksiyon
void message::inputHandler() {
    QDialog dialog;
    dialog.setWindowTitle("Kişisel Bilgiler");

    QFormLayout *layout = new QFormLayout(&dialog);
    QMap<QString, QLineEdit*> fields;
    QStringList fieldNames = {"Ad", "Doğum Yılı"};

    for (const QString &field : fieldNames) {
        QLineEdit *lineEdit = new QLineEdit();
        layout->addRow(field, lineEdit);
        fields[field] = lineEdit;
    }

    QPushButton *submitButton = new QPushButton("Gönder");
    QPushButton *cancelButton = new QPushButton("İptal");
    layout->addRow(submitButton, cancelButton);

    QObject::connect(submitButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        person_info person = {};
        strncpy(person.name, fields["Ad"]->text().toUtf8().constData(), sizeof(person.name) - 1);
        person.birthYear = fields["Doğum Yılı"]->text().toInt();


        person.crc = calculateCRC(reinterpret_cast<uint8_t*>(&person), sizeof(person_info) - sizeof(uint32_t));
        QByteArray serializedData = serializePerson(person);

        if (socket && socket->isOpen()) {
            socket->write(serializedData);
            socket->flush();
            qDebug() << "Kişisel bilgiler başarıyla gönderildi!";
        } else {
            qDebug() << "Bağlantı hatası! Sunucuya bağlı değil.";
        }
    }
}
