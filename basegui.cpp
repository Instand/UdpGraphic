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
    float key = 80.0f;
    int graphCount;
    int approximateCount;
    int penWidth;
    uint platformDelay = 5;
    uint tabletDelay = 10;
    uint screenNumber = 1;

    //color vector
    std::vector<Qt::GlobalColor> colors;

    //graphs size vector
    std::vector<int> graphsSize;

    //size values
    uint widgetWidth = 1227;
    uint widgetHeight = 870;
    const unsigned int headingSens = 0x6F;
    const unsigned int pitchSens = 0xAF;

    //show widget flag
    bool isIpPortShowing = false;
    bool isLogShowing = false;

    //names of graphs
    QStringList namesList;

    //udp ref
    QString ip1;
    uint port1;
    QString ip2;
    uint port2;
    QString ip3;
    uint port3;

    uint receivedDataCount = 6;
    QString box1Name;
    QString box2Name;
    QString box3Name;
    QString box4Name;
    QString box5Name;

    //received data
    QVector<float> receivedData = QVector<float>(receivedDataCount);
};

BaseGUI::BaseGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaseGUI),
    udpSettings(new UdpSettings()),
    log(new Log()),
    data(new Data())
{
    ui->setupUi(this);
    data->socket = new QUdpSocket(this);

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

    //log graphs control -> buttons names
    setupLogButtonList();

    //update box names
    getBoxNames();

    ui->painter->legend->setVisible(true);

    //connectings
    QObject::connect(ui->exitButton, &QPushButton::clicked, this, &BaseGUI::closeApplication);
    QObject::connect(data->socket, &QUdpSocket::readyRead, this, &BaseGUI::readyReadDataPendingDatagram);
    QObject::connect(ui->startButton, &QPushButton::clicked, this, &BaseGUI::resume);
    QObject::connect(ui->pauseButton, &QPushButton::clicked, this, &BaseGUI::pause);
    QObject::connect(ui->updateOptionsButton, &QPushButton::clicked, this, &BaseGUI::updateOptions);
    QObject::connect(ui->showButton, &QPushButton::clicked, this, &BaseGUI::showUdpSettings);
    QObject::connect(ui->logButton, &QPushButton::clicked, this, &BaseGUI::showLog);
    QObject::connect(ui->legendChekBox, SIGNAL(stateChanged(int)), SLOT(showLegend(int)));
    QObject::connect(&senderThread, SIGNAL(timeout(uint)), SLOT(receiveTimeout(uint)));
    QObject::connect(&senderThread, SIGNAL(allDataReceived(uint)), SLOT(receiveData(uint)));
    QObject::connect(ui->xSetButton, SIGNAL(clicked(bool)), SLOT(applyXRange()));
    QObject::connect(ui->ySetButton, SIGNAL(clicked(bool)), SLOT(applyYRange()));
    QObject::connect(ui->clearGraphButton, SIGNAL(clicked(bool)), SLOT(cleanGraphs()));
    QObject::connect(ui->screenButton, SIGNAL(clicked(bool)), SLOT(takeScreenshot()));
    QObject::connect(log->firstGraphBox, SIGNAL(stateChanged(int)), SLOT(setGraphsVisibility(int)));
    QObject::connect(log->secondGraphBox, SIGNAL(stateChanged(int)), SLOT(setGraphsVisibility(int)));
    QObject::connect(log->thirdGraphBox, SIGNAL(stateChanged(int)), SLOT(setGraphsVisibility(int)));
    QObject::connect(log->fourthGraphBox, SIGNAL(stateChanged(int)), SLOT(setGraphsVisibility(int)));
    QObject::connect(log->fifthGraphBox, SIGNAL(stateChanged(int)), SLOT(setGraphsVisibility(int)));
}

BaseGUI::~BaseGUI()
{
    delete data;
    delete udpSettings;
    delete ui;
}

void BaseGUI::processReceiveData(QByteArray &array, QHostAddress &address, quint16 port) noexcept
{
    //new concept
    QString addr = address.toString().mid(7);

    Q_UNUSED(port)

    if (addr == data->ip1) {

        bool isOk = false;
        QString hexStr = static_cast<QString>(array.toHex().toUpper());
        hexStr.remove(hexStr.length()-2, 2);
        hexStr.remove(0, 8);
        uint hexAngle = hexStr.toUInt(&isOk, 16);

        data->receivedData[0] = static_cast<float>(hexAngle)/static_cast<float>(data->pitchSens);
        senderThread.setData1Flag(true);
        log->addLog("Platform1 data received");

    }

    if (addr == data->ip2) {

        bool isOk = false;
        QString hexStr = static_cast<QString>(array.toHex().toUpper());
        hexStr.remove(hexStr.length()-2, 2);
        hexStr.remove(0, 8);
        uint hexAngle = hexStr.toUInt(&isOk, 16);

        data->receivedData[1] = static_cast<float>(hexAngle)/static_cast<float>(data->pitchSens);
        senderThread.setData2Flag(true);
        log->addLog("Platform2 data received");

    }

    if (addr == data->ip3) {

        int index = 0, step = 4;
        QVector<float> someData(3);

        for (int i = 0; i < 3; ++i) {

            float value = *(reinterpret_cast<float*>(array.mid(index, step).data()));
            someData[i] = value;
            index += step;

        }

        //copy to received data
        for (int i = 0, j = 2; i < 3; ++i, ++j)
            data->receivedData[j] = someData[i];

        senderThread.setData3Flag(true);
        log->addLog("Tablet data received");

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
    ui->painter->xAxis->setLabel("Время, с");
    ui->painter->yAxis->setLabel("Углы/Векторы, гр/пикс");
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

void BaseGUI::setupLogButtonList()
{
    auto list = log->getButtonList();

    //go to all buttons
    for (int i = 0; i < list.size(); ++i)
        list[i]->setText(data->namesList[i]);

}

QByteArray &BaseGUI::calcControlSum(QByteArray &array)
{
    unsigned int sum = 0;
    for (int i = 0; i < array.size(); ++i) {
        unsigned char temp = static_cast<unsigned char>(array[i]);
        sum = sum + static_cast<unsigned int>(temp);
    }
    ++sum;

    sum &= 0xFF;

    array.append(static_cast<char>(sum));
    return array;
}

void BaseGUI::closeApplication()
{
    qApp->exit();
}

void BaseGUI::addData(float x, float y, uint graphNum)
{
    //update graph data
    ui->painter->graph(graphNum)->addData(x, y);
   // ui->painter->xAxis->setRange(data->startX, data->endX);
    //ui->painter->yAxis->setRange(data->startY, data->endY);

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

void BaseGUI::setLogPos()
{
    log->move(x() - 295, y());
}

void BaseGUI::sendMessageToReceiveData()
{
    //platform 1 angle query
    uchar cmd[6] = {0xFF, 0x01, 0x00, 0x53, 0x00, 0x00};

    QByteArray command;
    command.append(reinterpret_cast<char*>(cmd), 6);
    data->socket->writeDatagram(calcControlSum(command), QHostAddress(data->ip1), data->port1);

    //platform 2 angle query
    QTimer::singleShot(data->platformDelay, [&]{

        uchar cmd1[6] = {0xFF, 0x01, 0x00, 0x53, 0x00, 0x00};

        QByteArray comm;
        comm.append(reinterpret_cast<char*>(cmd1), 6);
        data->socket->writeDatagram(calcControlSum(comm), QHostAddress(data->ip2), data->port2);

    });

    //tablet 3 data query
    QTimer::singleShot(data->tabletDelay, [&]{

        uchar comm1[6] = {0xC0, 0xFF, 0xFF, 0x01, 0x00, 0x00};

        QByteArray com1;
        com1.append(reinterpret_cast<char*>(comm1), 6);
        data->socket->writeDatagram(calcControlSum(com1), QHostAddress(data->ip3), data->port3);
        log->addLog("Data sended");

    });
}

void BaseGUI::receiveTimeout(uint)
{
    pause();

    //mb we will need something more
    //...
}

void BaseGUI::receiveData(uint t)
{
    data->receivedData[5] = static_cast<float>(t)/1000.0f;
    log->addLog("All Data received");
    setResumeStatus();

    for (int i = 0; i < data->graphCount; ++i)
        addData(data->receivedData[5], data->receivedData[i], i);

    sendMessageToReceiveData();
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
    ui->statusLabel->setText("<span style=\" font-size:14px; color:red;\">No connection</span>");
}

void BaseGUI::setResumeStatus()
{
    ui->statusLabel->setText("<span style=\" font-size:14px; color:green;\">Connection</span>");
}

void BaseGUI::resume()
{
    log->addLog("State: Started");

    senderThread.start();
    sendMessageToReceiveData();
}

void BaseGUI::pause()
{
    setPauseStatus();
    log->addLog("State: No connection");

    //do something if thread off all needed to shut down
    senderThread.requestInterruption();
    senderThread.wait();
    senderThread.dropFlags();

    //remove graphs data
    cleanGraphs();
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

    ui->painter->xAxis->setRange(data->startX, data->endX);
    ui->painter->yAxis->setRange(data->startY, data->endY);

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

    data->isLogShowing ? log->show() : log->hide();

    setupLogArrows();
}

void BaseGUI::updateUdpData()
{
    data->ip1 = udpSettings->ip1();
    data->port1 = udpSettings->port1();
    data->ip2 = udpSettings->ip2();
    data->port2 = udpSettings->port2();
    data->ip3 = udpSettings->ip3();
    data->port3 = udpSettings->port3();
}

void BaseGUI::showLegend(int state)
{
    state == Qt::CheckState::Checked ? ui->painter->legend->setVisible(true) : ui->painter->legend->setVisible(false);
    ui->painter->replot();
}

void BaseGUI::applyXRange()
{
    data->startX = ui->fromXEdit->text().toFloat();
    data->endX = ui->toXEdit->text().toFloat();
    ui->painter->xAxis->setRange(data->startX, data->endX);
    ui->painter->replot();
}

void BaseGUI::applyYRange()
{
    data->startY = ui->fromYEdit->text().toFloat();
    data->endY = ui->toYEdit->text().toFloat();
    ui->painter->yAxis->setRange(data->startY, data->endY);
    ui->painter->replot();
}

void BaseGUI::cleanGraphs()
{
    for (int i = 0; i < data->graphCount; ++i)
        ui->painter->graph(i)->clearData();

    data->graphsSize.clear();
    data->graphsSize.resize(data->graphCount);
}

void BaseGUI::takeScreenshot()
{
    uint number = data->screenNumber;

    forever {

        if (!QFile::exists(QApplication::applicationDirPath() + "/screenshots/screenshot" + QString::number(number) + QString(".bmp")))
            break;

        ++number;
    }

    ui->painter->saveBmp(QApplication::applicationDirPath() + "/screenshots/screenshot" + QString::number(number) + QString(".bmp"));
}

void BaseGUI::setGraphsVisibility(int state)
{
    QString name = QObject::sender()->objectName();
    bool _state = static_cast<bool>(state);

    if (name == data->box1Name)
        ui->painter->graph(0)->setVisible(_state);
    else if (name == data->box2Name)
        ui->painter->graph(1)->setVisible(_state);
    else if (name == data->box3Name)
        ui->painter->graph(2)->setVisible(_state);
    else if (name == data->box4Name)
        ui->painter->graph(3)->setVisible(_state);
    else if (name == data->box5Name)
        ui->painter->graph(4)->setVisible(_state);
}

void BaseGUI::getBoxNames()
{
    data->box1Name = log->fifthGraphBox->objectName();
    data->box2Name = log->secondGraphBox->objectName();
    data->box3Name = log->thirdGraphBox->objectName();
    data->box4Name = log->fourthGraphBox->objectName();
    data->box5Name = log->fifthGraphBox->objectName();
}

void BaseGUI::moveEvent(QMoveEvent *event)
{
    udpSettings->setGeometry(x() + width()/2 - 16, y() + height() + 34, udpSettings->width(), udpSettings->height()/4);
    log->move(x() - 295, y());
    QWidget::moveEvent(event);
}

void BaseGUI::paintEvent(QPaintEvent *event)
{
    udpSettings->setGeometry(x() + width()/2 - 16, y() + height() + 34, udpSettings->width(), udpSettings->height()/4);
    QWidget::paintEvent(event);
}
