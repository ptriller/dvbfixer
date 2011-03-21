#include "tspacket.h"
#include <stdexcept>


TSPacket::~TSPacket() {
}


void TSPacket::validate() {
  if(_block[0] != 0x47) throw std::invalid_argument("Sync byte wrong !");
  if(transport_error_indicator()) throw std::invalid_argument("Transport Error");
  
}

void TSPacket::parse() {
}
