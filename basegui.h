#ifndef BASEGUI_H
#define BASEGUI_H

#include <QWidget>
#include <QUdpSocket>
#include "udpsettings.h"
#include "log.h"
#include "senderthread.h"

namespace Ui {
    class BaseGUI;
}

class BaseGUI : public QWidget {
    Q_OBJECT

public:
    explicit BaseGUI(QWidget *parent = 0);
    ~BaseGUI();

private:
    Ui::BaseGUI *ui;

    //store udp gui
    UdpSettings* udpSettings;

    //visualize a log
    Log* log;

    //main thread
    SenderThread senderThread;

    struct Data;
    Data* data;

    //received data
    inline void processReceiveData(QByteArray& array, QHostAddress& address, quint16 port) noexcept;

    //setup colors
    void setupGraphColors();

    //create default graphs
    void createGraphs();

    //create axis names
    void createAxis();

    //setup axis drag and mouse move
    void setGraphMoveble();

    //create combo box
    void createViews();

    //show button setup
    void setupArrows();

    //show log button setup
    void setupLogArrows();

    //storage graps size
    void updateGraphsSize();

    //check remove flag
    void removeCheck(const float& value, uint graphNum);

    //setup legend
    void setupLegend();

    //get name list
    void setupNameList();

    //setup log list
    void setupLogButtonList();

    //calculate control summ for platform query
    QByteArray& calcControlSum(QByteArray& array);

public slots:
    void closeApplication();
    void addData(float x, float y, uint graphNum);
    void graphsNull();
    void setLogPos();

    //send a message to receive all data
    void sendMessageToReceiveData();

    void receiveTimeout(uint);
    void receiveData(uint t);

private slots:
    void readyReadDataPendingDatagram();
    void setPauseStatus();
    void setResumeStatus();
    void resume();
    void pause();
    void updateOptions();
    void showUdpSettings();
    void showLog();
    void updateUdpData();
    void showLegend(int state);
    void applyXRange();
    void applyYRange();

protected:
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // BASEGUI_H
