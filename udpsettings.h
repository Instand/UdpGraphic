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

    //options 1
    QLineEdit* ip1Edit;
    QLineEdit* port1Edit;

    //options 2
    QLineEdit* ip2Edit;
    QLineEdit* port2Edit;

    //options 3
    QLineEdit* ip3Edit;
    QLineEdit* port3Edit;

public:

    //interface
    uint port1() const;
    QString ip1() const;
    uint port2() const;
    QString ip2() const;
    uint port3() const;
    QString ip3() const;

private slots:
    void ip1TextEdit(const QString& str);
    void port1TextEdit(const QString& str);
    void ip2TextEdit(const QString& str);
    void port2TextEdit(const QString& str);
    void ip3TextEdit(const QString& str);
    void port3TextEdit(const QString& str);
};

#endif // UDPSETTINGS_H
