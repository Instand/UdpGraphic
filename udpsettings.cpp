#include "udpsettings.h"
#include <QString>
#include <QLabel>
#include <QGroupBox>

//pimpl
struct UdpSettings::Data {
    uint port = 6000;
    QString ip = "192.168.0.90";
};


UdpSettings::UdpSettings(QWidget *parent):
    QWidget(parent),
    data(new Data)
{
    ipEdit = new QLineEdit(this);
    portEdit = new QLineEdit(this);

    QLabel* ipLabel = new QLabel("Ip: ", this);
    QLabel* portLabel = new QLabel("Port: ", this);

    //buddy setup
    ipLabel->setBuddy(ipEdit);
    portLabel->setBuddy(portEdit);

    QGroupBox* groupBox = new QGroupBox("Udp settings", this);

    QHBoxLayout* lay = new QHBoxLayout();

    lay->addWidget(ipLabel);
    lay->addWidget(ipEdit);
    lay->addSpacing(10);
    lay->addWidget(portLabel);
    lay->addWidget(portEdit);

    groupBox->setLayout(lay);

    QHBoxLayout* groupLay = new QHBoxLayout();
    groupLay->addWidget(groupBox);

    setLayout(groupLay);

    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    //set default values
    ipEdit->setText(data->ip);
    portEdit->setText(QString::number(data->port));

    QObject::connect(ipEdit, SIGNAL(textChanged(QString)), SLOT(ipTextEdit(QString)));
    QObject::connect(portEdit, SIGNAL(textChanged(QString)), SLOT(portTextEdit(QString)));
}

UdpSettings::~UdpSettings()
{
    delete data;
}

uint UdpSettings::port() const
{
    return data->port;
}

QString UdpSettings::ip() const
{
    return data->ip;
}

void UdpSettings::ipTextEdit(const QString &str)
{
    data->ip = str;
}

void UdpSettings::portTextEdit(const QString &str)
{
    data->port = str.toUInt();
}
