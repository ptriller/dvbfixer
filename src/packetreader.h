#ifndef _PACKETREADER_H_
#define _PACKETREADER_H_
#include "config.h"
#include <string>
#include <fstream>
#include <deque>

class StreamReader;

struct TSPacket;

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


  void findNextFrame();

 public:

  TSPacketReader(StreamReader *);

  void init();

  bool readPacket();

  void registerPacketHandler(TSPacketHandler *);

  void unregisterPacketHander(TSPacketHandler *);

};


#endif // #ifndef _PACKETREADER_H_
