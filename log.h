#ifndef LOG_H
#define LOG_H

#include <QWidget>
#include <QGroupBox>
#include <QTextBrowser>
#include <QComboBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QVector>

class Log : public QWidget {

    Q_OBJECT

public:
    Log(QWidget* pwgt = 0);

    //get pointers to item
    QVector<QAbstractButton*> getButtonList() const;

private:

    //log elements
    QVBoxLayout* mainLayout;
    QVBoxLayout* buttonLay;
    QVBoxLayout* logLay;
    QGroupBox* buttonBox;
    QGroupBox* logBox;
    QTextBrowser* logBrowser;
    QPushButton* clearButton;

    void changeBoxNames();

public:
    // five check buttons
    QCheckBox* firstGraphBox;
    QCheckBox* secondGraphBox;
    QCheckBox* thirdGraphBox;
    QCheckBox* fourthGraphBox;
    QCheckBox* fifthGraphBox;

public slots:
    void addLog(const QString& log);
};

#endif // LOG_H
