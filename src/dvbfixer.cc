#include "config.h"
#include "stream.h"
#include "packetreader.h"
#include "pashandler.h"
#include "pmthandler.h"

#include <iostream>



int main(int argc, char **argv) {

  if(argc != 2) {
    std::cerr << "Usage :" << argv[0] << " <filename>" << std::endl;
    return -1;
  }

  FileStreamReader *reader = new FileStreamReader(argv[1]);

  TSPacketReader *tsreader = new TSPacketReader(reader);

  PASHandler *pas = new PASHandler();

  tsreader->registerPacketHandler(pas);
  //tsreader->registerPacketHandler(new PIDLogger(1600));

  tsreader->init();

  while(tsreader->readPacket()) {
    if(pas->pid !=  0) break;
  }

  if(pas->pid == 0) {
    std::cerr << "No Program Association Table found !" << std::endl;
    return -1;
  }
  PMTHandler *pmt = new PMTHandler(pas->pid);
  tsreader->unregisterPacketHander(pas);
  delete pas;
  tsreader->registerPacketHandler(pmt);

  while(tsreader->readPacket());


  return 0;

} 
