#ifndef _PASHANDLER_H_
#define _PASHANDLER_H_

#include "config.h"
#include <cstdint>
#include <ostream>
#include "packetreader.h"


class PASHandler : public TSPacketHandler {
public:
	int pmtid;

	PASHandler():pmtid(-1) {}

	virtual bool canHandle(TSPacket *);

	virtual void handle(TSPacket *);

};

#endif
