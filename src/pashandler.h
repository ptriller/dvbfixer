#ifndef _PASHANDLER_H_
#define _PASHANDLER_H_

#include "config.h"
#include "packetreader.h"


class PASHandler : public TSPacketHandler {
 public:
  uint16_t program;
  uint16_t pid;

  PASHandler(): program(0),pid(0) {}

  virtual bool canHandle(TSPacket *);

  virtual void handle(TSPacket *);
  
  ~PASHandler();
};

#endif
