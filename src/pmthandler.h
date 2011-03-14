#ifndef _PMTHANDLER_H_
#define _PMTHANDLER_H_

#include "config.h"
#include "packetreader.h"

class PMTHandler: public TSPacketHandler {
public:
  uint16_t pid;

  PMTHandler(uint16_t pid):pid(pid) {}

  virtual bool canHandle(TSPacket *);
  
  virtual void handle(TSPacket *);

};


#endif

