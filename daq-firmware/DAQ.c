#include "daq.h"
#include <stdarg.h>

typedef struct
{
    uint16_t counts;
    uint8_t addr;
} count_addr_t;

const static count_addr_t _addr_table[] =
{
    {  0, 0},
    {111, 8},
    {208, 4},
    {341, 2},
    {441, 6},
    {512, 1},
    {541, 9},
    {570, 5},
    {614, 3},
    {652, 7}
};
#define NADDR (sizeof(_addr_table)/sizeof(count_addr_t))

/**************************************************************************/
uint8_t daqAddr(void)
{
    uint16_t cnts = analogRead(DAQ_ADDR_ANALOG_PIN);
    
    for (int i=0; i<NADDR-1; i++)
        if (cnts < (_addr_table[i].counts+_addr_table[i+1].counts)/2)
            return _addr_table[i].addr;
    
    return _addr_table[NADDR-1].addr;
}

/**************************************************************************/
void daqWdogInit(void)
{
    pinMode(DAQ_WDOG_PIN, OUTPUT);
}

/**************************************************************************/
void daqWdogToggle(void)
{
    digitalWrite(DAQ_WDOG_PIN, !digitalRead(DAQ_WDOG_PIN));
}

/**************************************************************************/
void daqAbort(void)
{
    daqWdogInit();
    delay(100);
    while (1)
    {
        for (int i=0; i<4; i++)
        {
            digitalWrite(DAQ_WDOG_PIN, LOW);
            delay(100);
            digitalWrite(DAQ_WDOG_PIN, HIGH);
            delay(100);
            digitalWrite(DAQ_WDOG_PIN, LOW);
            delay(100);
            digitalWrite(DAQ_WDOG_PIN, HIGH);
            delay(500);
        }
        delay(2500);
    }
}
