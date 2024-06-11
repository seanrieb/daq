    #include "DAQ.h"
#include "ADCController.h"
#include "AccelController.h"
#include "crc.h"
#include "endian.h"

// NOTE: full duplex only

// @todo status measurement
// @todo healing of accelerometer

// uncomment the following to output measurement configs
// #define DEBUG_MEAS_CONFIG

// uncomment the following to toggle a test pin to test packet transmission rate
// #define DEBUG_TEST_PIN

// RS422/485 baud rate
// #define SERIAL_BAUD                 115200
#define SERIAL_BAUD                 1000000

/**************************************************************************/

#define MEAS_TYPE_ADCANALOG         0
#define MEAS_TYPE_ADCDIGITAL        1
#define MEAS_TYPE_ADCRATE           2
#define MEAS_TYPE_ACCELEROMETER     3
#define MEAS_TYPE_STATUS            4

typedef struct
{
    ADCController *adc;
    ADCAnalogOp_t op;
} MeasurementADCAnalog_t;

typedef struct
{
    ADCController *adc;
    uint8_t pin;
} MeasurementADCDigital_t;

typedef struct
{
    MeasurementADCDigital_t digital;
    uint32_t sample_prevTime;
    uint32_t sample_deltaT;
    uint32_t count;
    float alpha;
    float value;
    int8_t last;
} MeasurementADCRate_t;

typedef struct
{
    uint8_t range;
    uint8_t lpf;
    uint8_t hpf;
} MeasurementAccel_t;

typedef struct
{
} MeasurementStatus_t;

typedef struct
{
    uint16_t uniq;              // unique ID for transmit
    uint8_t type;               // type of measurement
    uint32_t prevTime;          // last time time this measurement occured
    uint32_t deltaT;            // target time to increment by based on rate
    union
    {
        MeasurementADCAnalog_t adca;
        MeasurementADCDigital_t adcd;
        MeasurementADCRate_t adcr;
        MeasurementAccel_t accel;
        MeasurementStatus_t status;
    } m;
} Measurement_t;

#define MAXMEAS     16
static Measurement_t _meas[MAXMEAS];
static uint8_t _nmeas;

/**************************************************************************/
#define TX_ENABLE() do { digitalWrite(DAQ_RS4XX_DE_PIN, HIGH); } while (0)
#define TX_DISABLE() do { digitalWrite(DAQ_RS4XX_DE_PIN, LOW); } while (0)
#define TX_IS_ENABLED() (digitalRead(DAQ_RS4XX_DE_PIN))
#define SERIAL_BUFLEN   63

#ifdef DEBUG_TEST_PIN
#define TOGGLE_TEST_PIN() do { digitalWrite(DAQ_TEST_PIN, !digitalRead(DAQ_TEST_PIN)); } while (0)
#else
#define TOGGLE_TEST_PIN() do { } while (0)
#endif

/**************************************************************************/
static uint8_t _daqAddr;

static ADCController _adc1(DAQ_ADC1_CS_N_PIN,
                           DAQ_ADC1_DRDY_N_PIN,
                           DAQ_ADC1_START_SYNC_PIN,
                           SPISettings(1000000, MSBFIRST, SPI_MODE1),
                           &DAQ_SPI);
                           
static ADCController _adc2(DAQ_ADC2_CS_N_PIN,
                           DAQ_ADC2_DRDY_N_PIN,
                           DAQ_ADC2_START_SYNC_PIN,
                           SPISettings(1000000, MSBFIRST, SPI_MODE1),
                           &DAQ_SPI);
                           
static AccelController _accel(DAQ_ACC_CS_N_PIN,
                              SPISettings(1000000, MSBFIRST, SPI_MODE0),
                              &DAQ_SPI);

static uint16_t _blinkyDelay = 200;

/**************************************************************************/
#define MAIN_STATE_PARSE_CONFIG         0
#define MAIN_STATE_RUN                  1

static uint8_t _mainState = MAIN_STATE_PARSE_CONFIG;

/**************************************************************************/
#ifdef DAQ_DEBUG
void daqDebug(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char str[256];
    vsnprintf(str, sizeof(str), format, ap);
    DAQ_DEBUG.println(str);
    va_end(ap);
}
#endif

/**************************************************************************/
static void _sendPacket(uint16_t uniq, uint8_t type, const void *data, uint8_t len)
{
    // make sure we do not block on sending
    if (DAQ_SERIAL.availableForWrite() < len + 7)
    {
        // @todo keep track of overflow packets
        return;
    }
    
    uint8_t header[] = {
        0xAA,
        0x55,
        _daqAddr,
        0,
        0,
        type,
        len,
    };
    
    // copy in 2 byts of uniq
    uniq = endianU16_NtoB(uniq);
    memcpy(header+3, &uniq, 2);
    
    uint16_t sum = crc16(0xFFFF, header+2, sizeof(header)-2);
    sum = crc16(sum, data, len);
    sum = endianU16_NtoB(sum);
    
    DAQ_SERIAL.write(header, sizeof(header));
    DAQ_SERIAL.write((const uint8_t *)data, len);
    DAQ_SERIAL.write((const uint8_t *)&sum, sizeof(sum));
    
    TOGGLE_TEST_PIN();
}

/**************************************************************************/
static void _addADCAnalogMeasurement(uint16_t uniq, uint16_t rate, uint16_t phase, uint8_t adc, uint8_t priority, ADCAnalogSettings_t *settings)
{
    if (_nmeas >= MAXMEAS || !rate)
        return;
    
    Measurement_t *meas = &_meas[_nmeas++];
    
#ifdef DEBUG_MEAS_CONFIG
    daqDebug("adding MEAS_TYPE_ADCANALOG uniq=%04X rate=%u", uniq, rate);
    daqDebug("       %02X %02X %06lX %04X %04X %02X %02X %02X %02X %02X %02X %02X %02X %02X %04X",
             _nmeas,
             MEAS_TYPE_ADCANALOG,
             uniq,
             rate,
             phase,
             adc,
             priority,
             settings->regs_used,
             settings->regs[0],
             settings->regs[1],
             settings->regs[2],
             settings->regs[3],
             settings->regs[4],
             settings->regs[5],
             settings->regs[6],
             settings->regs[7],
             settings->setup_time);
#endif
    
    meas->uniq = uniq;
    meas->type = MEAS_TYPE_ADCANALOG;
    meas->deltaT = 1000000 / rate;
    meas->prevTime = meas->deltaT * phase / 360;    // init with a phase delay
    meas->m.adca.adc = adc == 1 ? &_adc1 : &_adc2;
    
    // initialize op
    memcpy(&meas->m.adca.op.settings, settings, sizeof(ADCAnalogSettings_t));
    meas->m.adca.op.priority = priority;
    meas->m.adca.op.done = 2;   // use 2 to indicate its done but ready to start again
}

/**************************************************************************/
static void _configADCDigitalMeasurement(Measurement_t *meas, MeasurementADCDigital_t *digital, uint16_t uniq, uint16_t rate, uint16_t phase, uint8_t adc, uint8_t pin)
{
    if (!meas || !digital || !rate)
        return;
        
    if (pin != ADS114S08B_GPIO0 &&
        pin != ADS114S08B_GPIO1 &&
        pin != ADS114S08B_GPIO2 &&
        pin != ADS114S08B_GPIO3)
        return;
    
    meas->uniq = uniq;
    meas->deltaT = 1000000 / rate;
    meas->prevTime = meas->deltaT * phase / 360;    // init with a phase delay
    digital->adc = adc == 1 ? &_adc1 : &_adc2;
    digital->pin = pin;
}

/**************************************************************************/
static void _addADCDigitalMeasurement(uint16_t uniq, uint16_t rate, uint16_t phase, uint8_t adc, uint8_t pin)
{
    if (_nmeas >= MAXMEAS || !rate)
        return;
        
    if (pin != ADS114S08B_GPIO0 &&
        pin != ADS114S08B_GPIO1 &&
        pin != ADS114S08B_GPIO2 &&
        pin != ADS114S08B_GPIO3)
        return;
    
    Measurement_t *meas = &_meas[_nmeas++];
    
#ifdef DEBUG_MEAS_CONFIG
    daqDebug("adding MEAS_TYPE_ADCDIGITAL uniq=%04X rate=%u", uniq, rate);
    daqDebug("       %02X %02X %06lX %04X %04X %02X %02X",
             _nmeas,
             MEAS_TYPE_ADCDIGITAL,
             uniq,
             rate,
             phase,
             adc,
             pin);
#endif
    
    meas->type = MEAS_TYPE_ADCDIGITAL;
    _configADCDigitalMeasurement(meas, &meas->m.adcd, uniq, rate, phase, adc, pin);
}

/**************************************************************************/
static void _addADCRateMeasurement(uint16_t uniq, uint16_t rate, uint16_t phase, uint8_t adc, uint8_t pin, uint16_t sample_rate, float alpha)
{
    if (_nmeas >= MAXMEAS || !rate || !sample_rate || sample_rate < rate || !isfinite(alpha) || alpha < 0 || alpha > 1)
        return;
        
    if (pin != ADS114S08B_GPIO0 &&
        pin != ADS114S08B_GPIO1 &&
        pin != ADS114S08B_GPIO2 &&
        pin != ADS114S08B_GPIO3)
        return;
    
    Measurement_t *meas = &_meas[_nmeas++];
    
#ifdef DEBUG_MEAS_CONFIG
    uint16_t fval;
    memcpy(&fval, &alpha, 4);
    daqDebug("adding MEAS_TYPE_ADCRATE uniq=%04X rate=%u", uniq, rate);
    daqDebug("       %02X %02X %06lX %04X %04X %02X %02X %04X %04X",
             _nmeas,
             MEAS_TYPE_ADCRATE,
             uniq,
             rate,
             phase,
             adc,
             pin,
             sample_rate,
             fval);
#endif
    
    meas->type = MEAS_TYPE_ADCRATE;
    _configADCDigitalMeasurement(meas, &meas->m.adcr.digital, uniq, rate, phase, adc, pin);
    meas->m.adcr.sample_deltaT = 1000000 / sample_rate;
    meas->m.adcr.sample_deltaT = meas->deltaT * phase / 360;   // init with a phase delay
    meas->m.adcr.count = 0;
    meas->m.adcr.alpha = alpha;
    meas->m.adcr.value = NAN;
    meas->m.adcr.last = -1;
}

/**************************************************************************/
static void _addAccelerometerMeasurement(uint16_t uniq, uint16_t rate, uint16_t phase, uint8_t range)
{
    if (_nmeas >= MAXMEAS)
        return;
    
    // only allow 1 accel measurement
    for (int8_t i=0; i<_nmeas; i++)
        if (_meas[i].type == MEAS_TYPE_ACCELEROMETER)
            return;
    
    Measurement_t *meas = &_meas[_nmeas++];
    
#ifdef DEBUG_MEAS_CONFIG
    daqDebug("adding MEAS_TYPE_ACCELEROMETER uniq=%04X rate=%u", uniq, rate);
    daqDebug("       %02X %02X %06lX %04X %04X %02X",
             _nmeas,
             MEAS_TYPE_ACCELEROMETER,
             uniq,
             rate,
             phase,
             range);
#endif
    
    meas->uniq = uniq;
    meas->type = MEAS_TYPE_ACCELEROMETER;
    
    if (rate > 2000)
        meas->m.accel.lpf = ADC_FILTER_LPF_4K_1K;
    else if (rate > 1000)
        meas->m.accel.lpf = ADC_FILTER_LPF_2K_500;
    else if (rate > 500)
        meas->m.accel.lpf = ADC_FILTER_LPF_1K_250;
    else if (rate > 250)
        meas->m.accel.lpf = ADC_FILTER_LPF_500_125;
    else if (rate > 125)
        meas->m.accel.lpf = ADC_FILTER_LPF_250_62P5;
    else if (rate > 62)
        meas->m.accel.lpf = ADC_FILTER_LPF_125_31P25;
    else if (rate > 31)
        meas->m.accel.lpf = ADC_FILTER_LPF_62P5_15P625;
    else if (rate > 15)
        meas->m.accel.lpf = ADC_FILTER_LPF_31P25_7P813;
    else if (rate > 7)
        meas->m.accel.lpf = ADC_FILTER_LPF_15P625_3P906;
    else if (rate > 3)
        meas->m.accel.lpf = ADC_FILTER_LPF_7P813_1P953;
    else
        meas->m.accel.lpf = ADC_FILTER_LPF_3P906_0P977;
    
    meas->deltaT = (250 << meas->m.accel.lpf) / 2;  // sample ready at twice the rate
    meas->prevTime = meas->deltaT * phase / 360;    // init with a phase delay
    
    if (range > 20)
        meas->m.accel.range = ADC_RANGE_40G;
    else if (range > 10)
        meas->m.accel.range = ADC_RANGE_20G;
    else
        meas->m.accel.range = ADC_RANGE_10G;
    
    meas->m.accel.hpf = ADC_FILTER_HPF_NONE;
}

/**************************************************************************/
void setup()
{
    // Initialize ADC and Accel pins
    pinMode(DAQ_ADC1_CS_N_PIN, OUTPUT);
    digitalWrite(DAQ_ADC1_CS_N_PIN, HIGH);
    
    pinMode(DAQ_ADC2_CS_N_PIN, OUTPUT);
    digitalWrite(DAQ_ADC2_CS_N_PIN, HIGH);
    
    pinMode(DAQ_ACC_CS_N_PIN, OUTPUT);
    digitalWrite(DAQ_ACC_CS_N_PIN, HIGH);
    
#ifdef DEBUG_TEST_PIN
    pinMode(DAQ_TEST_PIN, OUTPUT);
    digitalWrite(DAQ_TEST_PIN, LOW);
#endif
    
    // Start SPI
    SPI.begin();
    
    // Initialize RS422/485 communications
    DAQ_SERIAL.begin(SERIAL_BAUD);
    
    // Transmit enable pin of RS422/485 data
    pinMode(DAQ_RS4XX_DE_PIN, OUTPUT);
    TX_ENABLE();
    
    daqWdogInit();
    
#ifdef DAQ_DEBUG
    // debug port
    DAQ_DEBUG.begin(115200);
    while (!DAQ_DEBUG)
    {
        daqWdogToggle();
        delay(400);
    }
#endif
    
    // init of remainder of DAQ board
    _daqAddr = daqAddr();
    daqDebug("\n===========");
    daqDebug("DAQ ADDR: %u", _daqAddr);
}

/**************************************************************************/
static void _blinkyTask(void)
{
    static uint32_t nextTime;
    uint32_t now = millis();
    if (now > nextTime)
    {
        daqWdogToggle();
        nextTime += _blinkyDelay;
    }
}

/**************************************************************************/
static void _adcAnalogTriggered(Measurement_t *meas)
{
    // only queue ops that are done
    if (meas->m.adca.op.done)
        meas->m.adca.adc->queueOp(&meas->m.adca.op);
}

/**************************************************************************/
static void _adcDigitalTriggered(Measurement_t *meas)
{
    uint8_t value = meas->m.adcd.adc->readGPIO(meas->m.adcd.pin) ? 1 : 0;
    _sendPacket(meas->uniq, meas->type, &value, sizeof(value));
}

/**************************************************************************/
static void _adcRateTriggered(Measurement_t *meas)
{
    // take in to account 2 transitions and useconds
    float x = 500000.0 * meas->m.adcr.count / meas->deltaT;
    
    if (isfinite(meas->m.adcr.value))
    {
        meas->m.adcr.value *= 1 - meas->m.adcr.alpha;
        meas->m.adcr.value += meas->m.adcr.alpha * x;
    }
    else
        meas->m.adcr.value = x;
    
    // convert to a 16-bit unsigned 
    uint16_t value = round(10 * meas->m.adcr.value);
    value = endianU16_NtoB(value);
    _sendPacket(meas->uniq, meas->type, &value, sizeof(value));

    // reset counter
    meas->m.adcr.count = 0;
}

/**************************************************************************/
static void _accelTriggered(Measurement_t *meas)
{
    int32_t values[3];
    if (!_accel.readLastXYZ(values[0], values[1], values[2]))
        return;
    values[0] = endianS32_NtoB(values[0]);
    values[1] = endianS32_NtoB(values[1]);
    values[2] = endianS32_NtoB(values[2]);
    _sendPacket(meas->uniq, meas->type, values, sizeof(values));
//     daqDebug("%ld %ld %ld", values[0], values[1], values[2]);
}

/**************************************************************************/
static void _adcAnalogInit(Measurement_t *meas, uint32_t now)
{
    meas->m.adca.adc->begin();
    meas->prevTime += now;
    meas->prevTime += 100000;       // 100ms delay
}

/**************************************************************************/
static void _adcDigitalInit(Measurement_t *meas, uint32_t now)
{
    meas->m.adcd.adc->begin();
    meas->m.adcd.adc->configAsGPIOInput(meas->m.adcd.pin);
    meas->prevTime += now;
    meas->prevTime += 100000;       // 100ms delay
}

/**************************************************************************/
static void _adcRateInit(Measurement_t *meas, uint32_t now)
{
    meas->m.adcr.digital.adc->begin();
    meas->m.adcr.digital.adc->configAsGPIOInput(meas->m.adcd.pin);
    meas->prevTime += now;
    meas->prevTime += 100000;       // 100ms delay
    meas->m.adcr.sample_prevTime = meas->prevTime + meas->m.adcr.sample_deltaT;
}

/**************************************************************************/
static void _accelInit(Measurement_t *meas, uint32_t now)
{
    while (_accel.begin())
    {
        daqDebug("accel init fail");
        daqWdogToggle();
        delay(100);
        daqWdogToggle();
        delay(100);
        daqWdogToggle();
        delay(100);
        daqWdogToggle();
        delay(500);
    }
    
    _accel.setRange(meas->m.accel.range);
    _accel.setFilter(meas->m.accel.lpf, meas->m.accel.hpf);
    _accel.startMeasurement();
    meas->prevTime += now;
    meas->prevTime += 100000;       // 100ms delay
}

/**************************************************************************/
static void _adcAnalogMeasurementTask(Measurement_t *meas, uint32_t now)
{
    if (meas->m.adca.op.done != 1)
        return;
    
    meas->m.adca.op.done = 2;
    uint16_t value = endianU16_NtoB(meas->m.adca.op.value);
    _sendPacket(meas->uniq, meas->type, &value, sizeof(value));
}

/**************************************************************************/
static void _adcRateMeasurementTask(Measurement_t *meas, uint32_t now)
{
    // dont run if not time
    if (now - meas->m.adcr.sample_prevTime <= meas->m.adcr.sample_deltaT)
        return;
    
    // update next time to run
    meas->m.adcr.sample_prevTime += meas->m.adcr.sample_deltaT;
    
    // read the value
    int8_t value = meas->m.adcr.digital.adc->readGPIO(meas->m.adcr.digital.pin) ? 1 : 0;
    
    // nothing to do if the same
    if (value == meas->m.adcr.last)
        return;
    
    // check if not the first time, if not increment
    if (meas->m.adcr.last >= 0)
        meas->m.adcr.count++;
    
    // always save
    meas->m.adcr.last = value;
}

/**************************************************************************/
static void (*_kInitTable[])(Measurement_t *, uint32_t) = {
    _adcAnalogInit,
    _adcDigitalInit,
    _adcRateInit,
    _accelInit,
    0,
};

static void (*_kTriggeredTable[])(Measurement_t *) = {
    _adcAnalogTriggered,
    _adcDigitalTriggered,
    _adcRateTriggered,
    _accelTriggered,
    0,
};

static void (*_kMeasurementTaskTable[])(Measurement_t *, uint32_t) = {
    _adcAnalogMeasurementTask,
    0,
    _adcRateMeasurementTask,
    0,
    0,
};

/**************************************************************************/
static void _configLoadCell(void)
{
    ADCAnalogSettings settings;
    initADCAnalogSettings(&settings);
    
    // turn on 2.5V reference
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_REF, 
                            ADS114S08B_REG_REF_REFCON_ON |
                            ADS114S08B_REG_REF_REFSEL_INT_2V5);
    
    // set data rate and single shot
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_DATARATE, 
                            ADS114S08B_REG_DATARATE_CLK_INTERNAL |
                            ADS114S08B_REG_DATARATE_MODE_SINGLE_SHOT |
                            ADS114S08B_REG_DATARATE_RESERVED |
                            ADS114S08B_REG_DATARATE_DR_4000);
    
    // set PGA gain of 128
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_PGA,
                            ADS114S08B_REG_PGA_EN |
                            ADS114S08B_REG_PGA_GAIN128);
    
    // disconnect any current sources
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_IDACMUX,
                            ADS114S08B_REG_IDACMUX_DISCONNECTED);
    
    // disconnect any vbias
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_VBIAS,
                            ADS114S08B_REG_VBIAS_OFF);
    
    // try to do all of these at the same phase
    
    // load cell X
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_INPMUX, DAQ_ADC_DIFF1);
    _addADCAnalogMeasurement(0x0001, 10, 0, 1, 0, &settings);
    
    // load cell Y
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_INPMUX, DAQ_ADC_DIFF2);
    _addADCAnalogMeasurement(0x0002, 10, 0, 1, 0, &settings);
    
    // load cell Z
    setADCAnalogSettingsReg(&settings, ADS114S08B_REG_INPMUX, DAQ_ADC_DIFF1);
    _addADCAnalogMeasurement(0x0003, 10, 0, 2, 0, &settings);
}

/**************************************************************************/
static void _config1(void)
{
//     _addAccelerometerMeasurement(0xFC0000, 100, 0, 40);
    _addADCRateMeasurement(0, 10, 0, 1, ADS114S08B_GPIO0, 1000, 0.8);
    _addADCDigitalMeasurement(0x0809, 10, 0, 1, ADS114S08B_GPIO1);
    _configLoadCell();
}

/**************************************************************************/
static void (*_kConfigTable[10])(void) = {
    _config1,
    _config1,
    _config1,
    _config1,
    _config1,
    _config1,
    _config1,
    _config1,
    _config1,
    _config1,
};

/**************************************************************************/
void loop()
{
    uint32_t now = micros();

    _blinkyTask();
    
    switch (_mainState)
    {
        case MAIN_STATE_PARSE_CONFIG:
            if (_kConfigTable[_daqAddr])
                _kConfigTable[_daqAddr]();
                
            for (uint8_t i=0; i<_nmeas; i++)
                if (_meas[i].type <= MEAS_TYPE_STATUS && _kInitTable[_meas[i].type])
                    _kInitTable[_meas[i].type](&_meas[i], now);
            
            _blinkyDelay = 50;
            _mainState = MAIN_STATE_RUN;
            break;
            
        case MAIN_STATE_RUN:
            // always run the ADC tasks
            _adc1.task();
            _adc2.task();
    
            for (uint8_t i=0; i<_nmeas; i++)
            {
                Measurement_t *meas = &_meas[i];
                
                // dont do anything with measurement if not correct type
                if (_meas[i].type > MEAS_TYPE_STATUS)
                    continue;
                
                // run measurement tasks
                if (_kMeasurementTaskTable[_meas[i].type])
                    _kMeasurementTaskTable[_meas[i].type](&_meas[i], now);
                
                // see if we trigger and run triggered task
                if (now - meas->prevTime > meas->deltaT)
                {
                    if (_kTriggeredTable[_meas[i].type])
                        _kTriggeredTable[_meas[i].type](&_meas[i]);
                    meas->prevTime += meas->deltaT;
                }
            }
            break;
        
        default:
            daqAbort();
    }
}
