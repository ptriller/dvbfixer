
#include "packetreader.h"
#include "stream.h"
#include <stdexcept>
#include <algorithm>
#include <iterator>

#define BLOCK_SIZE 188

#define PREBUF 188*3

TSPacketReader::TSPacketReader(StreamReader *reader): reader(reader) {
}


void TSPacketReader::init() {
  unsigned char buf[PREBUF];
  if(PREBUF != reader->read(buf, PREBUF)) {
    throw std::runtime_error("File too short !");
  }
  std::copy(buf,buf+PREBUF, std::back_inserter(queue) );
  
  findNextFrame();
}

void TSPacketReader::findNextFrame() {
  
}
