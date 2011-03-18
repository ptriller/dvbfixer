#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

unsigned int crc32(const uint8_t *d, unsigned int len, unsigned int crc=0xffffffff);


#endif // _CRC_H_
