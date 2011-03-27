#ifndef _PACKETREADER_H_
#define _PACKETREADER_H_
#include "config.h"
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <set>
#include <stdexcept>
#include <cstdint>
#include "tspacket.h"
class StreamReader;



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

//  TSPacket packet;

  std::set<uint16_t> pmt;

  std::set<TSPacketHandler *> handlers;

  void emitEvents(uint8_t *block);

 public:

  TSPacketReader(StreamReader *);

  void init();

  bool readPacket();

  void registerPacketHandler(TSPacketHandler *);

  void unregisterPacketHander(TSPacketHandler *);

  bool fillBuffer();

};


#endif // #ifndef _PACKETREADER_H_
