#include "senderthread.h"
#include <QElapsedTimer>

SenderThread::SenderThread(QObject *parent):
    QThread(parent)
{

}

void SenderThread::setData1Flag(bool value)
{
    dataFromPlatform1 = value;
}

void SenderThread::setData2Flag(bool value)
{
    dataFromPlatform2 = value;
}

void SenderThread::setData3Flag(bool value)
{
    dataFromTablet = value;
}

void SenderThread::run()
{

    QElapsedTimer timer;
    timer.start();
    uint wait = waitTime;

    forever {

        //check timeout of connection
        if (timer.hasExpired(wait)) {
            emit timeout(timer.elapsed());
            return;
        }

        //check shut down query
        if (QThread::currentThread()->isInterruptionRequested())
            return;

        //check data receving
        if (dataFromPlatform1 && dataFromPlatform2 && dataFromTablet) {

            uint someTime = timer.elapsed();
            emit allDataReceived(someTime);
            dropFlags();

            //we should wait a signal
            wait += someTime;

        }

        QThread::usleep(sleepTime);
    }

}

void SenderThread::dropFlags()
{
    dataFromPlatform1 = false;
    dataFromPlatform2 = false;
    dataFromTablet = false;
}
