#include "tspacket.h"
#include "stream.h"
#include "packetreader.h"

#include <iostream>

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage :" << argv[0] << " <filename>" << std::endl;
    return -1;
  }

  FileStreamReader *reader = new FileStreamReader(argv[1]);

  TSPacketReader *tsreader = new TSPacketReader(reader);

  tsreader->init();

  while(tsreader->readPacket()) { }

  return 0;
}
