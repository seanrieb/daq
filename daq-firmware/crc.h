#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

uint16_t crc16(uint16_t crc_seed, const void *c_ptr, uint16_t len);

#if defined(__cplusplus)
}
#endif

#endif // _CRC_H_
