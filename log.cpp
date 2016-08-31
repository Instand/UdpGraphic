#include "log.h"

Log::Log(QWidget *pwgt):
    QWidget(pwgt)
{

    //setup log behaviour
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}
