#ifndef PIHCSR04_H
#define PIHCSR04_H

#include <QObject>
#include <QDebug>
#include <QtMath>

#include <pigpiod_if2.h>

#define TIMEOUT 5000 //ms

class PiHCSR04 : public QObject
{
    Q_OBJECT
public:
    explicit PiHCSR04(const int &pi, const int &trigger, const int &echo, QObject *parent = nullptr);
    ~PiHCSR04();
public slots:
    double getDistance();
private slots:
signals:
private:
    const int pi;

    const int echo;
    const int trigger;

};

#endif // PIHCSR04_H
