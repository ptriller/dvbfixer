#include "outhandler.h"
#include "pmthandler.h"


bool OUTHandler::canHandle(TSPacket *packet) {
  return pmt->streams.find(packet->header->PID()) != pmt->streams.end();
}

void OUTHandler::handle(TSPacket *packet) {
  out.write((char *)packet->_data, 188);
}
