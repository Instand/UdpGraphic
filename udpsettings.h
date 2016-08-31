#ifndef UDPSETTINGS_H
#define UDPSETTINGS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class UdpSettings : public QWidget {
    Q_OBJECT
public:
    explicit UdpSettings(QWidget *parent = 0);
    ~UdpSettings();

private:
    struct Data;
    Data* data;

    //gui
    QLineEdit* ipEdit;
    QLineEdit* portEdit;

public:

    //interface
    uint port() const;
    QString ip() const;

private slots:
    void ipTextEdit(const QString& str);
    void portTextEdit(const QString& str);
};

#endif // UDPSETTINGS_H
