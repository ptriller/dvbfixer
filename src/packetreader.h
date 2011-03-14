#ifndef _PACKETREADER_H_
#define _PACKETREADER_H_
#include "config.h"
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <set>
#include <stdint.h>
class StreamReader;


struct TSPacketHeader {
  uint8_t  _data[4];
  uint8_t sync_byte() { return _data[0]; } // 8
  bool  transport_error_indicator() { return _data[1] & 0x80; } // 1
  bool  payload_unit_start_indicator() { return _data[1] & 0x40; } // 1
  bool  transport_priority() { return _data[1] & 0x20; } // 1
  uint16_t  PID() { uint16_t ret = (_data[1] & 0x1F) << 8; return ret | _data[2]; } // 13
  uint8_t  transport_scrambing_control() { return (_data[3] & 0xc0)>>6; } // 2
  uint8_t  adaptation_field_control() { return (_data[3] & 0x30) >> 4; } // 2
  uint8_t  continuity_counter() { return _data[3] & 0x0f; } // 4
  
} __attribute__((__packed__));


struct TSPESPacket {
}__attribute__((__packed__));

struct TSAdaptationField {
  uint8_t _data;
  uint8_t adaptation_field_length() { return (&_data)[0]; } // 8
  uint8_t *data() { return (&_data)+1; } // *
} __attribute__((__packed__));


struct TSPacket {
  struct TSPacketHeader *header;

  
  TSAdaptationField * adaptation_field;

  unsigned int data_size;
  unsigned char* data;

};

class TSPacketHandler {

 public:
  virtual bool canHandle(TSPacket *) = 0;

  virtual void handle(TSPacket *) = 0;

  virtual ~TSPacketHandler();
};


class TSPacketReader {
  
 private:
  StreamReader *reader;
  std::deque<unsigned char> queue;
  unsigned int streak;
  std::set<TSPacketHandler *> handlers;


  void emitEvents(unsigned char *block);

 public:

  TSPacketReader(StreamReader *);

  void init();

  bool readPacket();

  void registerPacketHandler(TSPacketHandler *);

  void unregisterPacketHander(TSPacketHandler *);

  bool fillBuffer();
};


#endif // #ifndef _PACKETREADER_H_
