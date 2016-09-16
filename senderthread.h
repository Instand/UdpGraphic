#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>
#include <QAtomicInteger>

class SenderThread : public QThread {

    Q_OBJECT

public:
    SenderThread(QObject* parent = 0);
    void setData1Flag(bool value);
    void setData2Flag(bool value);
    void setData3Flag(bool value);
    void dropFlags();

protected:
    virtual void run() override;

private:
    uint sleepTime = 5;
    uint waitTime = 2000;
    QAtomicInteger<bool> dataFromPlatform1 = false;
    QAtomicInteger<bool> dataFromPlatform2 = false;
    QAtomicInteger<bool> dataFromTablet = false;

signals:
    void timeout(uint t);
    void allDataReceived(uint t);
};

#endif // SENDERTHREAD_H
