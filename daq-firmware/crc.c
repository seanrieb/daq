#include "crc.h"

#define CRC16_POLY 0x1021

uint16_t crc16(uint16_t crc_seed, const void *c_ptr, uint16_t len)
{
    const uint8_t *data = c_ptr;
    uint16_t crc = crc_seed;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++)
            if (crc & 0x8000)
                crc = (crc << 1) ^ CRC16_POLY;
            else
                crc <<= 1;
    }
    return crc;
}
