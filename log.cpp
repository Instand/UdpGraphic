#include "log.h"

Log::Log(QWidget *pwgt):
    QWidget(pwgt)
{
    //setup check box group container
    buttonBox = new QGroupBox(this);
    buttonBox->setTitle("Graphs control");

    //setup elements
    firstGraphBox = new QCheckBox("Graph 1");
    secondGraphBox = new QCheckBox("Graph 2");
    thirdGraphBox = new QCheckBox("Graph 3");
    fourthGraphBox = new QCheckBox("Graph 4");
    fifthGraphBox = new QCheckBox("Graph 5");

    //set checked
    firstGraphBox->setChecked(true);
    secondGraphBox->setChecked(true);
    thirdGraphBox->setChecked(true);
    fourthGraphBox->setChecked(true);
    fifthGraphBox->setChecked(true);

    //setup button lay
    buttonLay = new QVBoxLayout(this);
    buttonLay->addWidget(firstGraphBox);
    buttonLay->addWidget(secondGraphBox);
    buttonLay->addWidget(thirdGraphBox);
    buttonLay->addWidget(fourthGraphBox);
    buttonLay->addWidget(fifthGraphBox);

    buttonBox->setLayout(buttonLay);

    //setup layout
    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(buttonBox);

    //create log box
    logBox = new QGroupBox("Log", this);

    //setup log lay
    logLay = new QVBoxLayout();

    //create log
    logBrowser = new QTextBrowser();
    logLay->addWidget(logBrowser);
    logBox->setLayout(logLay);


    mainLayout->addWidget(logBox);

    clearButton = new QPushButton("Clear log");
    mainLayout->addWidget(clearButton);

    //setup log behaviour
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    setLayout(mainLayout);


    QObject::connect(clearButton, SIGNAL(clicked(bool)), logBrowser, SLOT(clear()));
}

QVector<QAbstractButton *> Log::getButtonList() const
{
    QVector<QAbstractButton*> list;
    list.push_back(firstGraphBox);
    list.push_back(secondGraphBox);
    list.push_back(thirdGraphBox);
    list.push_back(fourthGraphBox);
    list.push_back(fifthGraphBox);

    return list;
}

void Log::addLog(const QString &log)
{
    logBrowser->append(log);
}
