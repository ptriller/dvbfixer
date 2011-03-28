

#include "pashandler.h"
#include "crc.h"
#include "logger.h"
#include <iostream>
#include <stdexcept>

bool PASHandler::canHandle(TSPacket *packet) {
  return packet->PID() == 0;
}


void  PASHandler::handle(TSPacket *opacket) {

	PasPacket packet(opacket->_block);
	packet.parse();
	packet.validate();
	PASEntry *entry = packet.entry(0);
	if(pmtid < 0) {
		pmtid = entry->PID();
	} else if(pmtid != entry->PID()) {
		throw std::invalid_argument("Program changed. This is not supported.");
	}
}
