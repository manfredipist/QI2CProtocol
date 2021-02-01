#include "pihcsr04.h"

PiHCSR04::PiHCSR04(const int &pi, const int &trigger, const int &echo, QObject *parent) : QObject(parent), pi(pi), echo(echo), trigger(trigger)
{       
    set_mode(pi, trigger, PI_OUTPUT);
    set_mode(pi, echo, PI_INPUT);

    gpio_write(pi, trigger, PI_OFF);
}

PiHCSR04::~PiHCSR04()
{

}

double PiHCSR04::getDistance()
{
    gpio_write(pi, trigger, PI_ON);
    time_sleep(10/1000/1000);
    gpio_write(pi, trigger, PI_OFF);

    unsigned int now = get_current_tick(pi);

    while(gpio_read(pi, echo) == PI_OFF && get_current_tick(pi) - now < TIMEOUT);
    unsigned int pulse_start = get_current_tick(pi);

    while(gpio_read(pi, echo) == PI_ON);
    unsigned int pulse_end = get_current_tick(pi);

    unsigned int pulse_duration = pulse_end - pulse_start;
    double distance = (pulse_duration * 17150)/1000000.0;

    if(distance>0 && distance<300)
       return QString::number(distance,'f',1).toDouble();
    else
        return -1;
}
