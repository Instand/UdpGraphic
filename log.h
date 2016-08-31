#ifndef LOG_H
#define LOG_H

#include <QWidget>
#include <QTextBrowser>

class Log : public QWidget {

    Q_OBJECT

public:
    Log(QWidget* pwgt = 0);


private:
    QTextBrowser* logBrowser;
};

#endif // LOG_H
