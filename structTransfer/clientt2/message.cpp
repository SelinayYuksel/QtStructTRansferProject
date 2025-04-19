#include "message.h"
#include <QDialog>
#include <QLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QMessageBox>
#include<QDate>
// ✅ message::person_info zaten message.h içinde tanımlı, burada tekrar tanımlama!

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

// ✅ Struct'ı serileştirme
QByteArray message::serializePerson(const person_info& person) {
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.writeRawData(reinterpret_cast<const char*>(&person), sizeof(person_info));
    return byteArray;
}

// ✅ Struct'ı deserialize etme
message::person_info message::deserializePerson(const QByteArray& byteArray) {
    person_info person;
    QDataStream stream(byteArray);
    stream.readRawData(reinterpret_cast<char*>(&person), sizeof(person_info));
    return person;
}

// ✅ Kullanıcı girişlerini alıp struct oluşturma ve gönderme
void message::inputHandler() {
    QDialog dialog;
    dialog.setWindowTitle("Kişisel Bilgiler");
    QFormLayout *layout = new QFormLayout(&dialog);

    QMap<QString, QLineEdit*> fields;
    QStringList fieldNames = {"Name", "Birth Year"};

    for (const QString &field : fieldNames) {
        QLineEdit *lineEdit = new QLineEdit();
        layout->addRow(field, lineEdit);
        fields[field] = lineEdit;
        if (field == "Name") {
            lineEdit->setStyleSheet("font-size: 32px; color: #4f6f00; background-color: #f8f4ff  ;");
        } else if (field == "Birth Year") {
            lineEdit->setStyleSheet("font-size: 32px; color: #4f6f00; background-color: #f8f4ff  ;");
        }
    }

    QPushButton *submitButton = new QPushButton("Gönder");
    QPushButton *cancelButton = new QPushButton("İptal");
    layout->addRow(submitButton, cancelButton);

    QObject::connect(submitButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        person_info person = {};
        strncpy(person.name, fields["Name"]->text().toUtf8().constData(), sizeof(person.name) - 1);

        // Yaşı al
        QString birthYearText = fields["Birth Year"]->text().trimmed();  // Boşlukları temizle
        bool ok;
        int birthYear = birthYearText.toInt(&ok);  // Dönüştürme işlemi

        // Yaş bilgisi geçerli değilse uyarı göster
        if (!ok || birthYear <= 0) {  // Yaş negatif ya da sıfır olamaz
            QMessageBox::warning(&dialog, "Hata", "Geçerli bir doğum yılı girin.");
            return; // Hatalı giriş yapılırsa işlemi durdur
        }

        person.birth_year = birthYear;

        // Geçerli yılı al
        int currentYear = QDate::currentDate().year(); // Geçerli yıl
        person.age = currentYear - person.birth_year; // Yaş hesaplanıyor
        if (person.age < 0) {
            QMessageBox::warning(&dialog, "Hata", "Doğum yılı geçerli değil.");
            return;
        }


        // CRC hesapla
        person.crc = calculateCRC(reinterpret_cast<uint8_t*>(&person), sizeof(person_info) - sizeof(uint32_t));

        // Struct'ı ByteArray'e çevir
        QByteArray serializedData = serializePerson(person);

        // Socket üzerinden gönder
        if (socket && socket->isOpen()) {
            socket->write(serializedData);
            socket->flush();
            qDebug() << "Kişisel bilgiler başarıyla gönderildi!";
        } else {
            qDebug() << "Bağlantı hatası! Sunucuya bağlı değil.";
        }
    }
}
