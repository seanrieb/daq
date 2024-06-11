#include "AccelController.h"
#include "ADXL357.h"
#include "DAQ.h"

#define SPI_ENABLE() do { digitalWrite(_cs_n_pin, LOW); } while (0)
#define SPI_DISABLE() do { digitalWrite(_cs_n_pin, HIGH); } while (0)

/**************************************************************************/
int8_t AccelController::begin(void)
{
    if (_begun)
        return 0;
        
    if (_readByte(ADXL357_REG_PARTID) != 0xED)
        return -1;
    
    _writeByte(ADXL357_REG_RESET, 0x52);
    delay(200);
    
    _begun = 1;
    
    return 0;
}

/**************************************************************************/
int8_t AccelController::readLastXYZ(int32_t &x, int32_t &y, int32_t &z)
{
    if (0 == (_readByte(ADXL357_REG_STATUS) & 1))
        return 0;
    
    // read until FIFO is empty
    uint8_t data[9];
    do
    {
        _readBytes(ADXL357_REG_FIFO_DATA, data, 9);
    } while (_readByte(ADXL357_REG_STATUS) & 1);
    
    
    x = ((uint32_t)data[0]<<24)|((uint32_t)data[1]<<16)|((uint32_t)data[2]<<8);
    x = x >> 12;
    y = ((uint32_t)data[3]<<24)|((uint32_t)data[4]<<16)|((uint32_t)data[5]<<8);
    y = y >> 12;
    z = ((uint32_t)data[6]<<24)|((uint32_t)data[7]<<16)|((uint32_t)data[8]<<8);
    z = z >> 12;
    
    return 1;
}

/**************************************************************************/
void AccelController::setRange(uint8_t range)
{
    if (range < ADC_RANGE_10G || range > ADC_RANGE_40G)
        return;
    uint8_t value = _readByte(ADXL357_REG_RANGE);
    value &= 0xFC;
    value |= range;
    _writeByte(ADXL357_REG_RANGE, value);
}

/**************************************************************************/
void AccelController::startMeasurement(void)
{
    uint8_t value = _readByte(ADXL357_REG_POWER_CTL);
    value &= 0xFE;
    _writeByte(ADXL357_REG_POWER_CTL, value);
}

/**************************************************************************/
void AccelController::setFilter(uint8_t lpf, uint8_t hpf)
{
    if (lpf > ADC_FILTER_LPF_3P906_0P977 || hpf > ADC_FILTER_HPF_0P238)
        return;
    _writeByte(ADXL357_REG_FILTER, (hpf<<4) | lpf);
}

/**************************************************************************/
uint16_t AccelController::readRawTemp(void)
{
    uint8_t hob1, hob2, lob;
    
    // loop since temp can be updated between reading 1 and 2
    hob2 = _readByte(ADXL357_REG_TEMP2);
    do
    {
        hob1 = hob2;
        lob = _readByte(ADXL357_REG_TEMP1);
        hob2 = _readByte(ADXL357_REG_TEMP2);
    } while (hob1 != hob2);
    
    return (hob1 << 8) | lob;
}

/**************************************************************************/
float AccelController::readTemperature(void)
{
    return 25 + ((int32_t)readRawTemp() - 1852) / -9.05;
}

/**************************************************************************/
void AccelController::_writeByte(uint8_t reg, uint8_t data)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    uint8_t buffer[2] = { reg<<1, data };
    _spi->transfer(buffer, 2);
    SPI_DISABLE();
    _spi->endTransaction();
}

/**************************************************************************/
void AccelController::_writeByteU16(uint8_t reg, uint16_t data)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    uint8_t buffer[3] = { reg<<1, data>>8, data&0xFF };
    _spi->transfer(buffer, 3);
    SPI_DISABLE();
    _spi->endTransaction();
}

/**************************************************************************/
void AccelController::_writeByteU32(uint8_t reg, uint32_t data)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    uint8_t buffer[5] = { reg<<1, data>>24, (data>>16)&0xFF, (data>>8)&0xFF, data&0xFF };
    _spi->transfer(buffer, 5);
    SPI_DISABLE();
    _spi->endTransaction();
}

/**************************************************************************/
void AccelController::_writeBytes(uint8_t reg, const uint8_t* data, uint32_t write_len)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    _spi->transfer(reg<<1);
    _spi->transfer(data, write_len);
    SPI_DISABLE();
    _spi->endTransaction();
}

/**************************************************************************/
uint8_t AccelController::_readByte(uint8_t reg)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    _spi->transfer(reg<<1|1);
    uint8_t value = _spi->transfer(0);
    SPI_DISABLE();
    _spi->endTransaction();
    return value;
}

/**************************************************************************/
uint16_t AccelController::_readByteU16(uint8_t reg)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    _spi->transfer(reg<<1|1);
    uint16_t value = _spi->transfer(0) << 8;
    value |= _spi->transfer(0);
    SPI_DISABLE();
    _spi->endTransaction();
    return value;
}

/**************************************************************************/
uint32_t AccelController::_readByteU32(uint8_t reg)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    _spi->transfer(reg<<1|1);
    uint32_t value = _spi->transfer(0) << 24;
    value |= _spi->transfer(0) << 16;
    value |= _spi->transfer(0) << 8;
    value |= _spi->transfer(0);
    SPI_DISABLE();
    _spi->endTransaction();
    return value;
}

/**************************************************************************/
void AccelController::_readBytes(uint8_t reg, uint8_t* data, uint32_t read_len)
{
    _spi->beginTransaction(_spi_settings);
    SPI_ENABLE();
    _spi->transfer(reg<<1|1);
    _spi->transfer(data, read_len);
    SPI_DISABLE();
    _spi->endTransaction();
}
