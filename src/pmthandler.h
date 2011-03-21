#ifndef _PMTHANDLER_H_
#define _PMTHANDLER_H_

#include "config.h"
#include "packetreader.h"
#include <fstream>

class PMTHandler: public TSPacketHandler {
public:

  std::set<uint16_t> streams;

  uint16_t pid;
  std::fstream &out;

  PMTHandler(uint16_t pid, std::fstream &out):pid(pid),out(out) {}

  virtual bool canHandle(TSPacket *);
  
  virtual void handle(TSPacket *);

};


#endif

