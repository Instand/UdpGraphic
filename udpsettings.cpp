#include "udpsettings.h"
#include <QString>
#include <QLabel>
#include <QGroupBox>

//pimpl
struct UdpSettings::Data {
    uint port1 = 6000;
    QString ip1 = "192.168.0.93";
    uint port2 = 6000;
    QString ip2 = "192.168.0.94";
    uint port3 = 8080;
    QString ip3 = "192.168.0.90";
};


UdpSettings::UdpSettings(QWidget *parent):
    QWidget(parent),
    data(new Data)
{
    ip1Edit = new QLineEdit(this);
    port1Edit = new QLineEdit(this);
    ip2Edit = new QLineEdit(this);
    port2Edit = new QLineEdit(this);
    ip3Edit = new QLineEdit(this);
    port3Edit = new QLineEdit(this);

    QLabel* ipLabel = new QLabel("Ip №1: ", this);
    QLabel* portLabel = new QLabel("Port №1: ", this);

    //buddy setup
    ipLabel->setBuddy(ip1Edit);
    portLabel->setBuddy(port1Edit);

    QGroupBox* groupBox = new QGroupBox("Udp settings", this);

    QHBoxLayout* lay = new QHBoxLayout();

    lay->addWidget(ipLabel);
    lay->addWidget(ip1Edit);
    lay->addSpacing(10);
    lay->addWidget(portLabel);
    lay->addWidget(port1Edit);

    //group2
    QLabel* ip2Label = new QLabel("Ip №2: ", this);
    QLabel* port2Label = new QLabel("Port №2: ", this);
    ip2Label->setBuddy(ip2Edit);
    port2Label->setBuddy(port2Edit);

    QHBoxLayout* lay2 = new QHBoxLayout();
    lay2->addWidget(ip2Label);
    lay2->addWidget(ip2Edit);
    lay2->addSpacing(10);
    lay2->addWidget(port2Label);
    lay2->addWidget(port2Edit);

    //group3
    QLabel* ip3Label = new QLabel("Ip №3: ", this);
    QLabel* port3Label = new QLabel("Port №3: ", this);
    ip3Label->setBuddy(ip3Edit);
    port3Label->setBuddy(port3Edit);

    QHBoxLayout* lay3 = new QHBoxLayout();
    lay3->addWidget(ip3Label);
    lay3->addWidget(ip3Edit);
    lay3->addSpacing(10);
    lay3->addWidget(port3Label);
    lay3->addWidget(port3Edit);

    QVBoxLayout* vLay = new QVBoxLayout();
    vLay->addLayout(lay);
    vLay->addLayout(lay2);
    vLay->addLayout(lay3);

    groupBox->setLayout(vLay);

    QHBoxLayout* groupLay = new QHBoxLayout();
    groupLay->addWidget(groupBox);

    setLayout(groupLay);

    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    //set default values
    ip1Edit->setText(data->ip1);
    port1Edit->setText(QString::number(data->port1));
    ip2Edit->setText(data->ip2);
    port2Edit->setText(QString::number(data->port2));
    ip3Edit->setText(data->ip3);
    port3Edit->setText(QString::number(data->port3));

    QObject::connect(ip1Edit, SIGNAL(textChanged(QString)), SLOT(ip1TextEdit(QString)));
    QObject::connect(port1Edit, SIGNAL(textChanged(QString)), SLOT(port1TextEdit(QString)));
    QObject::connect(ip2Edit, SIGNAL(textChanged(QString)), SLOT(ip2TextEdit(QString)));
    QObject::connect(port2Edit, SIGNAL(textChanged(QString)), SLOT(port2TextEdit(QString)));
    QObject::connect(ip3Edit, SIGNAL(textChanged(QString)), SLOT(ip3TextEdit(QString)));
    QObject::connect(port3Edit, SIGNAL(textChanged(QString)), SLOT(port3TextEdit(QString)));
}

UdpSettings::~UdpSettings()
{
    delete data;
}

uint UdpSettings::port1() const
{
    return data->port1;
}

QString UdpSettings::ip1() const
{
    return data->ip1;
}

uint UdpSettings::port2() const
{
    return data->port2;
}

QString UdpSettings::ip2() const
{
    return data->ip2;
}

uint UdpSettings::port3() const
{
    return data->port3;
}

QString UdpSettings::ip3() const
{
    return data->ip3;
}

void UdpSettings::ip1TextEdit(const QString &str)
{
    data->ip1 = str;
}

void UdpSettings::port1TextEdit(const QString &str)
{
    data->port1 = str.toUInt();
}

void UdpSettings::ip2TextEdit(const QString &str)
{
    data->ip2 = str;
}

void UdpSettings::port2TextEdit(const QString &str)
{
    data->port2 = str.toUInt();
}

void UdpSettings::ip3TextEdit(const QString &str)
{
    data->ip3 = str;
}

void UdpSettings::port3TextEdit(const QString &str)
{
    data->port3 = str.toUInt();
}
