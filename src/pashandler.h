#ifndef _PASHANDLER_H_
#define _PASHANDLER_H_

#include "config.h"
#include "packetreader.h"

#include <ostream>

class PASHandler : public TSPacketHandler {
 public:
  uint16_t program;
  uint16_t pid;
  std::ostream &out;

  PASHandler(std::ostream &out): program(0),pid(0),out(out) {}

  virtual bool canHandle(TSPacket *);

  virtual void handle(TSPacket *);
  
  ~PASHandler();
};

#endif
