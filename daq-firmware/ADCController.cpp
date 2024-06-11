#include "ADCController.h"
#include "DAQ.h"    // for daqDebug and daqAbort

#define SPI_ENABLE() do { digitalWrite(_cs_n_pin, LOW); } while (0)
#define SPI_DISABLE() do { digitalWrite(_cs_n_pin, HIGH); } while (0)

#define OP_STATE_READY_TO_START     0
#define OP_STATE_WAIT_DELAY         1
#define OP_STATE_WAIT_DATA          2

/**************************************************************************/
void ADCController::begin(void)
{
    if (_begun)
        return;
    _begun = 1;
    
    // we will used pins to start and query measurements instead of SPI bus
    pinMode(_drdy_n_pin, INPUT);
    
    pinMode(_start_sync_pin, OUTPUT);
    digitalWrite(_start_sync_pin, LOW);
    
    _command(ADS114S08B_CMD_RESET);
    delay(250);
    
    // initialize the shadow registers
    _beginTransaction();
    for (int i=0; i<ADS114S08B_NUM_REGS; i++)
        _shadow[i] = _regReadAlways(i);
    _spi->endTransaction();
    _endTransactionFinal();
}

/**************************************************************************/
void ADCController::_beginTransaction(void)
{
    if (0 == _transactionLevel++)
    {
        _spi->beginTransaction(_spi_settings);
        SPI_ENABLE();
    }
    
}

/**************************************************************************/
void ADCController::_endTransaction(void)
{
    // protect against going negative
    if (_transactionLevel == 0)
        return;
    if (0 == --_transactionLevel)
    {
        SPI_DISABLE();
        _spi->endTransaction();
    }
}

/**************************************************************************/
void ADCController::_endTransactionFinal(void)
{
    SPI_DISABLE();
    _spi->endTransaction();
    _transactionLevel = 0;
}

/**************************************************************************/
void ADCController::_command(uint8_t cmd)
{
    _beginTransaction();
    _spi->transfer(cmd);
    _endTransaction();
}

/**************************************************************************/
uint16_t ADCController::_readData(void)
{
    _beginTransaction();
    uint8_t buffer[3] = { ADS114S08B_CMD_RDATA };
    SPI.transfer(buffer, 3);
    _endTransaction();
    return (buffer[1] << 8) | buffer[2];
}

/**************************************************************************/
uint8_t ADCController::_regRead(uint8_t reg)
{
    if (reg >= ADS114S08B_NUM_REGS)
        return 0;
    
    uint32_t bit = 1UL << reg;
    
    if (!(bit & ADS114S08B_VOLATILE_REG_MASK))
        return _shadow[reg];
    
    // save to shadow (not really necessary)
    return _shadow[reg] = _regReadAlways(reg);
}

/**************************************************************************/
uint8_t ADCController::_regReadAlways(uint8_t reg)
{
    uint8_t buffer[3] = { ADS114S08B_CMD_RREG(reg), 0 };
    _beginTransaction();
    _spi->transfer(buffer, 3);
    _endTransaction();
    return buffer[2];
}

/**************************************************************************/
void ADCController::_regWrite(uint8_t reg, uint8_t val)
{
    if (reg >= ADS114S08B_NUM_REGS || _shadow[reg] == val)
        return;
    
    // save to shadow and commit
    _shadow[reg] = val;
    _regWriteAlways(reg, val);
}

/**************************************************************************/
void ADCController::_regWriteAlways(uint8_t reg, uint8_t val)
{
    uint8_t buffer[3] = { ADS114S08B_CMD_WREG(reg), 0, val };
    _beginTransaction();
    SPI.transfer(buffer, 3);
    _endTransaction();
}

/**************************************************************************/
void ADCController::configAsGPIOInput(uint8_t pin)
{
    if (pin != ADS114S08B_GPIO0 &&
        pin != ADS114S08B_GPIO1 &&
        pin != ADS114S08B_GPIO2 &&
        pin != ADS114S08B_GPIO3)
        return;
    
    uint8_t bit = 1 << (pin-ADS114S08B_AIN8_GPIO0);
    
    // set direction first as input
    _regWrite(ADS114S08B_REG_GPIODAT, _regRead(ADS114S08B_REG_GPIODAT) | (bit << 4));
    
    // next set pin as GPIO
    _regWrite(ADS114S08B_REG_GPIOCON, _regRead(ADS114S08B_REG_GPIOCON) | bit);
}

/**************************************************************************/
uint8_t ADCController::readGPIO(uint8_t pin)
{
    if (pin != ADS114S08B_GPIO0 &&
        pin != ADS114S08B_GPIO1 &&
        pin != ADS114S08B_GPIO2 &&
        pin != ADS114S08B_GPIO3)
        return 0;
        
    uint8_t bit = 1 << (pin-ADS114S08B_AIN8_GPIO0);
    
    // dont read if not used as GPIO (saves a bus transfer)
    if (0 == _regRead(ADS114S08B_REG_GPIOCON) & bit)
        return 0;
        
    return (_regRead(ADS114S08B_REG_GPIODAT) & bit) != 0;
}

/**************************************************************************/
void ADCController::queueOp(ADCAnalogOp_t *newop)
{
    newop->done = 0;
    newop->_state = OP_STATE_READY_TO_START;
    
    uint8_t idx = 0;
    ADCAnalogOp_t **op = &_head;

    // find first op not in progress (should only be one);
    for ( ; *op != NULL && !(*op)->done; op = &(*op)->_next)
        idx++;
    
    // find last op with same or greater priority 
    for ( ; *op != NULL && (*op)->priority >= newop->priority; op = &(*op)->_next)
        idx++;
    
    // insert before (handles nothing in queue also)
    newop->_next = *op;
    *op = newop;
}

/**************************************************************************/
void ADCController::task(void)
{
    if (!_begun || _head == NULL)
        return;
    
    ADCAnalogOp_t *op = _head;
    switch (op->_state)
    {
        case OP_STATE_READY_TO_START:
            _beginTransaction();
            // only set registered that are used
            for (uint8_t reg=0; reg < sizeof(op->settings.regs); reg++)
                if (op->settings.regs_used & (1<<reg))
                    _regWrite(reg+ADS114S08B_REG_INPMUX, op->settings.regs[reg]);
            _endTransactionFinal();
            
            _opPrevTime = millis();
            op->_state = OP_STATE_WAIT_DELAY;
            
            // continue through...
        
        case OP_STATE_WAIT_DELAY:
            if (millis() - _opPrevTime < op->settings.setup_time)
                return;
                
            digitalWrite(_start_sync_pin, HIGH);    // start measurement
            op->_state = OP_STATE_WAIT_DATA;
            break;
        
        case OP_STATE_WAIT_DATA:
            // wait for data to be ready
            if (digitalRead(_drdy_n_pin))
                break;
                
            digitalWrite(_start_sync_pin, LOW);
            op->value = _readData();
            op->done = 1;
            
            _head = op->_next;
            break;
        
        default:
            daqAbort();
    }
}

/**************************************************************************/
void setADCAnalogSettingsReg(ADCAnalogSettings_t *settings, uint8_t reg, uint8_t value)
{
    reg -= ADS114S08B_REG_INPMUX;
    if (!settings || reg > sizeof(settings->regs))
        return;
    
    settings->regs_used |= 1 << reg;        // mark as used
    settings->regs[reg] = value;
}