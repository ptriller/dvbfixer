#include "config.h"
#include "stream.h"
#include "packetreader.h"
#include "pashandler.h"
#include "pmthandler.h"
#include "outhandler.h"

#include <iostream>
#include <fstream>


int main(int argc, char **argv) {

  if(argc != 3) {
    std::cerr << "Usage :" << argv[0] << " <filename> <output>" << std::endl;
    return -1;
  }

  std::fstream out(argv[2], std::fstream::out);
  FileStreamReader *reader = new FileStreamReader(argv[1]);

  TSPacketReader *tsreader = new TSPacketReader(reader);

  PASHandler *pas = new PASHandler(out);

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
  //  return 0;
  PMTHandler *pmt = new PMTHandler(pas->pid,out);
  OUTHandler *output = new OUTHandler(pmt,out);
  tsreader->unregisterPacketHander(pas);
  delete pas;
  tsreader->registerPacketHandler(pmt);
  tsreader->registerPacketHandler(output);

  while(tsreader->readPacket()) {
    
  };

  out.close();
  return 0;

} 
