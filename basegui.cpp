#include "basegui.h"
#include "ui_basegui.h"
#include <vector>
#include <QAtomicInteger>

//PIMPL
struct BaseGUI::Data {

    //socket
    QUdpSocket* socket;

    //memory independent receive flag
    QAtomicInteger<bool> isReceiving = false;

    //axis data
    float startX;
    float endX;
    float startY;
    float endY;
    float key = 8.0f;
    int graphCount;
    int approximateCount;
    int penWidth;

    //color vector
    std::vector<Qt::GlobalColor> colors;

    //graphs size vector
    std::vector<int> graphsSize;

    //size values
    uint widgetWidth = 1227;
    uint widgetHeight = 870;

    //show widget flag
    bool isIpPortShowing = false;
    bool isLogShowing = false;

    //names of graphs
    QStringList namesList;

    //udp ref
    QString ip;
    uint port;
};

BaseGUI::BaseGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaseGUI),
    udpSettings(new UdpSettings()),
    data(new Data)
{
    ui->setupUi(this);
    data->socket = new QUdpSocket(this);

    //fix size
    setFixedSize(data->widgetWidth, data->widgetHeight);

    //collect current ranges
    data->startX = ui->fromXEdit->text().toFloat();
    data->startY = ui->fromYEdit->text().toFloat();
    data->endX = ui->toXEdit->text().toFloat();
    data->endY = ui->toYEdit->text().toFloat();

    //get graph count
    data->graphCount = ui->graphCountEdit->text().toInt();

    //get approximate count
    data->approximateCount = ui->approximateEdit->text().toInt();

    //get pen width
    data->penWidth = ui->widthEdit->text().toInt();

    //get udp refs
    updateUdpData();

    //create base axis and labels
    createAxis();

    //setup base colors for graph count
    setupGraphColors();

    //create graphs
    createGraphs();

    //setup graph
    setGraphMoveble();

    //setup gui
    setPauseStatus();

    //create view modes
    createViews();

    //setup ip/port show button
    setupArrows();

    //setup log button
    setupLogArrows();

    //fill legend names
    setupNameList();

    //setup legend
    setupLegend();

    //connectings
    QObject::connect(ui->exitButton, &QPushButton::clicked, this, &BaseGUI::closeApplication);
    QObject::connect(data->socket, &QUdpSocket::readyRead, this, &BaseGUI::readyReadDataPendingDatagram);
    QObject::connect(ui->startButton, &QPushButton::clicked, this, &BaseGUI::resume);
    QObject::connect(ui->pauseButton, &QPushButton::clicked, this, &BaseGUI::pause);
    QObject::connect(ui->updateOptionsButton, &QPushButton::clicked, this, &BaseGUI::updateOptions);
    QObject::connect(ui->showButton, &QPushButton::clicked, this, &BaseGUI::showUdpSettings);
    QObject::connect(ui->logButton, &QPushButton::clicked, this, &BaseGUI::showLog);
    QObject::connect(ui->legendChekBox, SIGNAL(stateChanged(int)), SLOT(showLegend(int)));

    //axis connections
    QObject::connect(ui->painter->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->painter->xAxis2, SLOT(setRange(QCPRange)));
    QObject::connect(ui->painter->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->painter->yAxis2, SLOT(setRange(QCPRange)));
}

BaseGUI::~BaseGUI()
{
    delete data;
    delete udpSettings;
    delete ui;
}

void BaseGUI::processReceiveData(QByteArray &array, QHostAddress &address, quint16 port) noexcept
{
    //process data
    if (data->isReceiving) {

        //check sender
        if (address.toString() == data->ip && port == data->port) {

            //do something
            int index = 0, step = 4, timePos = 20, timeStep = 4;

            float time = array.mid(timePos, timeStep).toFloat();

            //graphs count cycle
            for (int i = 0; i < data->graphCount; ++i) {

                //draw on graphs
                addData(time, array.mid(index, step).toFloat(), i);

                //update index
                index += step;

            }
        }
    }
}

void BaseGUI::setupGraphColors()
{
    //base first color
    int color = Qt::red;

    //get graph count
    int graphCount = ui->graphCountEdit->text().toInt();

    //clear current colors
    data->colors.clear();

    //fill colors
    for (int i = 0; i < graphCount; ++i, ++color)
        data->colors.push_back(static_cast<Qt::GlobalColor>(color));
}

void BaseGUI::createGraphs()
{
    //create all numbers of graphs
    for (int i = 0; i < data->graphCount; ++i) {

        //clean special pen for different graphs
        QPen pen(data->colors[i]);

        //set pen width
        pen.setWidth(data->penWidth);

        //add graph
        ui->painter->addGraph();

        //add pen to graph
        ui->painter->graph(i)->setPen(pen);

    }
}

void BaseGUI::createAxis()
{
    ui->painter->xAxis->setLabel("Delta t");
    ui->painter->yAxis->setLabel("Degree");
    ui->painter->xAxis->setRange(data->startX, data->endX);
    ui->painter->yAxis->setRange(data->startY, data->endY);

    //for tick calculation
    ui->painter->xAxis->setAutoTicks(true);
    ui->painter->yAxis->setAutoTicks(true);

    ui->painter->xAxis->setAutoTickStep(true);
    ui->painter->yAxis->setAutoTickStep(true);

    ui->painter->xAxis->setAutoTickCount(data->approximateCount);
    ui->painter->yAxis->setAutoTickCount(data->approximateCount);
}

void BaseGUI::setGraphMoveble()
{
    ui->painter->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);
}

void BaseGUI::createViews()
{
    QStringList list;

    list << "All";

    for (int i = 0; i < data->graphCount; ++i)
        list << "Graph " + QString::number(i);

    ui->viewComboBox->addItems(list);
}

void BaseGUI::setupArrows()
{
    data->isIpPortShowing ? ui->showButton->setIcon(QIcon(":/ArrowsUp.png")) : ui->showButton->setIcon(QIcon(":/ArrowsDown.png"));
}

void BaseGUI::setupLogArrows()
{
    data->isLogShowing ? ui->logButton->setIcon(QIcon(":/ArrowsRight.png")) : ui->logButton->setIcon(QIcon(":/ArrowsLeft.png"));
}

void BaseGUI::updateGraphsSize()
{
    data->graphsSize.resize(data->graphCount);

    for (int i = 0; i < data->graphCount; ++i)
        data->graphsSize.at(i) = ui->painter->graph(i)->data()->size();
}

void BaseGUI::removeCheck(const float &value, uint graphNum)
{
    if (ui->removeGraphCheck->isChecked())
        ui->painter->graph(graphNum)->removeDataBefore(value);
}

void BaseGUI::setupLegend()
{
    QFont legendFont("Comic Sans MS", 10);

    ui->painter->legend->setFont(legendFont);

    for (int i = 0; i < data->graphCount; ++i)
        ui->painter->graph(i)->setName(data->namesList[i]);
}

void BaseGUI::setupNameList()
{
    data->namesList << "BackLink Platform" << "External Liverage Platrform" << "CompaNav" << "Hover Point" << "Control signal" << "Graph 6"
                    << "Graph 7" << "Graph 8" << "Graph 9" << "Graph 10";
}


void BaseGUI::closeApplication()
{
    qApp->exit();
}

void BaseGUI::addData(float x, float y, uint graphNum)
{
    //update graph data
    ui->painter->graph(graphNum)->addData(x, y);
    ui->painter->xAxis->setRange(x + data->startX, data->endX, Qt::AlignRight);
    ui->painter->yAxis->setRange(data->startY, data->endY);

    //remove data before
    removeCheck(x - data->key, graphNum);

    //update visual presentation
    ui->painter->replot();

    //collect current graph size
    updateGraphsSize();

    //there can be action to visual graph size
    //....
}

void BaseGUI::graphsNull()
{
    //remove graphs
    ui->painter->clearGraphs();

    //create new
    createGraphs();
}

void BaseGUI::readyReadDataPendingDatagram()
{
    //read all data
    while (data->socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(data->socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        data->socket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        //collected data
        processReceiveData(datagram, sender, senderPort);
    }
}

void BaseGUI::setPauseStatus()
{
    ui->statusLabel->setText("<span style=\" font-size:14px; color:red;\">Pause</span>");
}

void BaseGUI::setResumeStatus()
{
    ui->statusLabel->setText("<span style=\" font-size:14px; color:green;\">Receive</span>");
}

void BaseGUI::resume()
{
    setResumeStatus();

    data->isReceiving = true;
}

void BaseGUI::pause()
{
    setPauseStatus();

    data->isReceiving = false;
}

void BaseGUI::updateOptions()
{
    int penWidth = ui->widthEdit->text().toInt();
    int graphsCount = ui->graphCountEdit->text().toInt();

    if (data->penWidth != penWidth || data->graphCount != graphsCount) {

        //update pen width
        data->penWidth = penWidth;

        //update graps count
        if (graphsCount > 10)
            graphsCount = 10;

        data->graphCount = graphsCount;
        ui->graphCountEdit->setText(QString::number(data->graphCount));

        //update colors
        setupGraphColors();

        //update legend
        ui->painter->legend->clear();

        //create new graphs
        graphsNull();

        //update view
        ui->viewComboBox->clear();
        createViews();

        //update graphs names
        setupLegend();
    }

    //update ranges data
    data->startX = ui->fromXEdit->text().toFloat();
    data->endX = ui->toXEdit->text().toFloat();
    data->startY = ui->fromYEdit->text().toFloat();
    data->endY = ui->toYEdit->text().toFloat();

    //clear graphs
    if (ui->clearGraphCheck->isChecked())
        graphsNull();

    data->approximateCount = ui->approximateEdit->text().toInt();

    updateUdpData();

    //replot
    ui->painter->replot();
}

void BaseGUI::showUdpSettings()
{
    data->isIpPortShowing = !data->isIpPortShowing;

    setupArrows();

    data->isIpPortShowing ? udpSettings->show() : udpSettings->hide();

    updateUdpData();
}

void BaseGUI::showLog()
{
    data->isLogShowing = !data->isLogShowing;

    setupLogArrows();
}

void BaseGUI::updateUdpData()
{
    data->ip = udpSettings->ip();
    data->port = udpSettings->port();
}

void BaseGUI::showLegend(int state)
{
    state == Qt::CheckState::Checked ? ui->painter->legend->setVisible(true) : ui->painter->legend->setVisible(false);
    ui->painter->replot();
}

void BaseGUI::moveEvent(QMoveEvent *event)
{
    udpSettings->setGeometry(x() + width()/2 - 16, y() + height() + 34, udpSettings->width(), udpSettings->height()/4);
    QWidget::moveEvent(event);
}

void BaseGUI::paintEvent(QPaintEvent *event)
{
    udpSettings->setGeometry(x() + width()/2 - 16, y() + height() + 34, udpSettings->width(), udpSettings->height()/4);
    QWidget::paintEvent(event);
}
