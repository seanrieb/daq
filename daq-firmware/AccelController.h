#ifndef _ACCELCONTROLLER_H_
#define _ACELLCONTROLLER_H_

#include <SPI.h>

#define ADC_RANGE_10G           1
#define ADC_RANGE_20G           2
#define ADC_RANGE_40G           3


#define ADC_FILTER_LPF_4K_1K            0
#define ADC_FILTER_LPF_2K_500           1
#define ADC_FILTER_LPF_1K_250           2
#define ADC_FILTER_LPF_500_125          3
#define ADC_FILTER_LPF_250_62P5         4
#define ADC_FILTER_LPF_125_31P25        5
#define ADC_FILTER_LPF_62P5_15P625      6
#define ADC_FILTER_LPF_31P25_7P813      7
#define ADC_FILTER_LPF_15P625_3P906     8
#define ADC_FILTER_LPF_7P813_1P953      9
#define ADC_FILTER_LPF_3P906_0P977      10

#define ADC_FILTER_HPF_NONE     0
#define ADC_FILTER_HPF_247      1
#define ADC_FILTER_HPF_62P084   2
#define ADC_FILTER_HPF_15P545   3
#define ADC_FILTER_HPF_2P862    4
#define ADC_FILTER_HPF_0P954    5
#define ADC_FILTER_HPF_0P238    6

typedef struct AccelSettings
{
    uint8_t filter;
    uint8_t range;
} AccelSettings_t;

class AccelController
{
public:
    AccelController(uint8_t cs_n_pin, SPISettings spi_settings, SPIClass *spi = &SPI) :
        _cs_n_pin(cs_n_pin), _spi_settings(spi_settings), _spi(spi) { };
        
    int8_t begin(void);
    int8_t readLastXYZ(int32_t &x, int32_t &y, int32_t &z);
    
    void setRange(uint8_t range);
    void startMeasurement(void);
    void setFilter(uint8_t lpf, uint8_t hpf);
    
    uint16_t readRawTemp(void);
    float readTemperature(void);
    
private:
    uint8_t _cs_n_pin;
    SPISettings _spi_settings;
    SPIClass *_spi;
    uint8_t _begun;
    
    // transaction level
    uint8_t _transactionLevel;
    
    // used to aggregate transactions
    void _beginTransaction(void);
    void _endTransaction(void);
    void _endTransactionFinal(void);
    
    // spi communications
    void _writeByte(uint8_t reg, uint8_t data);
    void _writeByteU16(uint8_t reg, uint16_t data);
    void _writeByteU32(uint8_t reg, uint32_t data);
    void _writeBytes(uint8_t reg, const uint8_t* data, uint32_t write_len);
    uint8_t _readByte(uint8_t reg);
    uint16_t _readByteU16(uint8_t reg);
    uint32_t _readByteU32(uint8_t reg);
    void _readBytes(uint8_t reg, uint8_t* data, uint32_t read_len);
};

#endif // _ACCELCONTROLLER_H_
