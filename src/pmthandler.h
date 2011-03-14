#ifndef _PMTHANDLER_H_
#define _PMTHANDLER_H_

#include "config.h"
#include "packetreader.h"
#include <ostream>

class PMTHandler: public TSPacketHandler {
public:

  std::set<uint16_t> streams;

  uint16_t pid;
  std::ostream &out;

  PMTHandler(uint16_t pid, std::ostream &out):pid(pid),out(out) {}

  virtual bool canHandle(TSPacket *);
  
  virtual void handle(TSPacket *);

};


#endif

