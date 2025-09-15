#ifndef _ADCCONTROLLER_H_
#define _ADCCONTROLLER_H_

#include <SPI.h>
#include "ADS114S08B.h"

typedef struct ADCAnalogSettings
{
    uint8_t regs_used;              // bitmask for registers that are used
    uint8_t regs[7];                // regs array
    uint16_t setup_time;            // setup time in milli seconds
} ADCAnalogSettings_t;

typedef struct ADCAnalogOp
{
    ADCAnalogSettings settings;
    uint8_t priority;               // 0 is lowest, 1 next lowest, etc...
    uint16_t value;                 // returned value
    uint8_t done;                   // set to 1 when operation is finished
    
    // opaque data
    uint8_t _state;
    struct ADCAnalogOp *_next;
} ADCAnalogOp_t;

class ADCController
{
public:
    ADCController(uint8_t cs_n_pin, uint8_t drdy_n_pin, uint8_t start_sync_pin, SPISettings spi_settings, SPIClass *spi = &SPI) :
        _cs_n_pin(cs_n_pin), _drdy_n_pin(drdy_n_pin), _start_sync_pin(start_sync_pin),
        _spi_settings(spi_settings), _spi(spi) { };
        
    void begin(void);
    void configAsGPIOInput(uint8_t pin);
    uint8_t readGPIO(uint8_t pin);
    void queueOp(ADCAnalogOp_t *newop);
    void task(void);
    
private:
    uint8_t _cs_n_pin;
    uint8_t _drdy_n_pin;
    uint8_t _start_sync_pin;
    SPISettings _spi_settings;
    SPIClass *_spi;
    uint8_t _begun;
    
    // Analog operation queue
    ADCAnalogOp_t *_head;
    uint32_t _opPrevTime;       // used for operation delay
    
    // shadow registers
    uint8_t _shadow[ADS114S08B_NUM_REGS];
    
    // transaction level
    uint8_t _transactionLevel;
    
    // used to aggregate transactions
    void _beginTransaction(void);
    void _endTransaction(void);
    void _endTransactionFinal(void);
    
    // send command
    void _command(uint8_t);
    uint16_t _readData(void);
    
    // read/write always and shadow
    uint8_t _regRead(uint8_t reg);
    uint8_t _regReadAlways(uint8_t reg);
    void _regWrite(uint8_t reg, uint8_t val);
    void _regWriteAlways(uint8_t reg, uint8_t val);
};

inline void initADCAnalogSettings(ADCAnalogSettings_t *settings) { if (settings) memset(settings, 0, sizeof(ADCAnalogSettings_t)); }
void setADCAnalogSettingsReg(ADCAnalogSettings_t *settings, uint8_t reg, uint8_t value);

#endif // _ADCCONTROLLER_H_
