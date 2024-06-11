#ifndef _DAQ_H_
#define _DAQ_H_

#include "ADS114S08B.h"
#include <Arduino.h>

// uncomment the following for debug (can be Serial or Serial1)
// #define DAQ_DEBUG                   Serial

// uncomment the following to toggle test measurement pin
#define DAQ_TEST_PIN                A1

#define DAQ_SERIAL                  Serial1
#define DAQ_SPI                     SPI

// Pins
#define DAQ_WDOG_PIN                9
#define DAQ_RS4XX_DE_PIN            10
#define DAQ_ACC_CS_N_PIN            4

#define DAQ_ADC1_CS_N_PIN           7
#define DAQ_ADC1_DRDY_N_PIN         3
#define DAQ_ADC1_START_SYNC_PIN     5

#define DAQ_ADC2_CS_N_PIN           8
#define DAQ_ADC2_DRDY_N_PIN         2
#define DAQ_ADC2_START_SYNC_PIN     6

#define DAQ_ADDR_ANALOG_PIN         A0

#define DAQ_ADC_DIFF1               ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN9,ADS114S08B_AIN8)
#define DAQ_ADC_DIFF2               ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN11,ADS114S08B_AIN10)
#define DAQ_ADC_DIFF3               ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN7,ADS114S08B_AIN6)
#define DAQ_ADC_SE1                 ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN0,ADS114S08B_AINCOM)
#define DAQ_ADC_SE2                 ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN1,ADS114S08B_AINCOM)
#define DAQ_ADC_SE3                 ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN2,ADS114S08B_AINCOM)
#define DAQ_ADC_SE4                 ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN3,ADS114S08B_AINCOM)
#define DAQ_ADC_SE5                 ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN4,ADS114S08B_AINCOM)
#define DAQ_ADC_SE6                 ADS114S08B_REG_INPMUX_MUX(ADS114S08B_AIN5,ADS114S08B_AINCOM)

#if defined(__cplusplus)
extern "C"
{
#endif

// Helper routines
uint8_t daqAddr(void);
void daqWdogInit(void);
void daqWdogToggle(void);
void daqAbort(void);
void daqDebug(const char *format, ...);
void dbgstr(const char* str);

#if defined(__cplusplus)
}
#endif

#ifdef DAQ_DEBUG
void daqDebug(const char *format, ...) __attribute__((format(printf,1,2)));
#else
#define daqDebug(...) { }
#endif

#endif // _DAQ_H_
