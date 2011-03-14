#ifndef _OUTHANDLER_H_
#define _OUTHANDLER_H_

#include "packetreader.h"
#include <ostream>

class PMTHandler;

class OUTHandler : public TSPacketHandler {
public:
  PMTHandler *pmt;

  std::ostream &out;

  OUTHandler(PMTHandler *pmt, std::ostream &out):pmt(pmt),out(out) {}

  virtual bool canHandle(TSPacket*);

  virtual void handle(TSPacket*);
};

#endif
