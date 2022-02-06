#include <Arduino.h>
#include "mydisplay.h"

class Alarm
{
    MyDisplay &display;

public:
    Alarm(MyDisplay &dispref);
    void evaluate(uint8_t type, uint8_t flr, uint8_t room);
};
