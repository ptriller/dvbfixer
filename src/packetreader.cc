#include "config.h"
#include "packetreader.h"
#include "stream.h"
#include <algorithm>
#include <iterator>
#include <iostream>

#define BLOCK_SIZE 188

#define PREBUF 188*1000

TSPacketReader::TSPacketReader(StreamReader *reader): reader(reader) {
}

/**
 * \brief Initializes the Parser
 *
 */
void TSPacketReader::init() {
  // Not much to do here
  streak = 0;
}

/**
 * \brief Processes the next packet
 * 
 * Finds and Processes the next TS Packet, in case of a Stream error it will emit
 * an error event
 */
bool TSPacketReader::readPacket() {
  unsigned char buf[BLOCK_SIZE];
  unsigned int errorbytes = 0;
  // Find Start Marker sequence
  if(queue.size() < BLOCK_SIZE) {
    fillBuffer();
    //    std::cout << "Queue: " << queue.size() << std::endl;
    if(queue.size() < BLOCK_SIZE) {
      if(queue.size() > 0) {
	std::cout << "Stream had " << queue.size() << " Bytes of garbage at the end" << std::endl;
      }
      return false;
    }
  }
  // Good stream, just walk on if all is well
  if(streak >=3 && queue[0] == 0x47) {
    //    std::cout << "Walk on, nothing to see" << std::endl;
    streak++;
    std::copy(queue.begin(),queue.begin()+BLOCK_SIZE,buf);
    queue.erase(queue.begin(),queue.begin()+BLOCK_SIZE);
  } else {
    bool good = true;
    do {
      if(queue.size() < BLOCK_SIZE*(4-streak)) {
	std::cout << "Try to fill up " << std::endl;
	fillBuffer();
	if(queue.size() < BLOCK_SIZE) return false;
      }
      //      std::cout << "Checking" << std::endl;
      good = true;
      //      std::cout << "check streak" << std::endl;
      for(unsigned int i = 0; i < 4; ++i) {
	if(queue.size() >= (1+i)*BLOCK_SIZE && queue[i*BLOCK_SIZE] != 0x47) {
	  //	  std::cout << "Bad byte" << std::endl;
	  good = false;
	  break;
	}
      }
      if(!good) {
	errorbytes++;
	streak=0;
	queue.pop_front();
      }
    } while(!good);
    streak++;
    std::copy(queue.begin(),queue.begin()+BLOCK_SIZE,buf);
    queue.erase(queue.begin(),queue.begin()+BLOCK_SIZE); 
  }
  
  // emit error if bytes were skipped
  if(errorbytes > 0) {
    std::cout << "Skipped " << errorbytes << " Bytes" << std::endl;
  }
  
  // emit events to all handlers
  emitEvents(buf);
  return true;
}
static unsigned long long int totalread = 0;

bool TSPacketReader::fillBuffer() {
  unsigned char buf[PREBUF];
  unsigned int len = PREBUF - queue.size();
  unsigned int read = 0;
  if(totalread / (1024*1024*100) < (totalread+len) /(1024*1024*100)) {
    std::cerr << "Read: " << (totalread+len) /(1024*1024) << " Mb" << std::endl;
  }
  totalread += len;
  if(len > 0) {
    if(0 == (read = reader->read(buf, len))) {
      std::cout << "End of Stream reached" << std::endl;
      return false;
    }
    
    std::copy(buf,buf+len, std::back_inserter(queue) );
    
  }
  return true;
}


void TSPacketReader::emitEvents(unsigned char *block) {
  TSPacket packet;
  packet.header = (TSPacketHeader *)block;
  if((packet.header->adaptation_field_control() & 0x02) != 0) {
    packet.adaptation_field = (TSAdaptationField *)(block + 4);
  } else {
    packet.adaptation_field = NULL;
  }
  if((packet.header->adaptation_field_control() & 0x01) != 0) {
    if(packet.adaptation_field != NULL) {
      packet.data = block+6+packet.adaptation_field->adaptation_field_length();
    } else {
      packet.data = block+4;
    }
  } else {
    packet.data = NULL;
  }

  for(std::set<TSPacketHandler *>::iterator it = handlers.begin();
      it != handlers.end();++it) {
    try {
      if((*it)->canHandle(&packet)) (*it)->handle(&packet);
    } catch(std::invalid_argument &e) {
      std::cout << "Invalid packet, rewind." << std::endl;
      streak = 0;
      for(uint8_t *b = block+187; b != block;--b) queue.push_front(*b);
      //      std::reverse_copy(block,block+188, std::front_inserter(queue) );
    }
  }
}


void TSPacketReader::registerPacketHandler(TSPacketHandler *handler) {
  handlers.insert(handler);
}

void TSPacketReader::unregisterPacketHander(TSPacketHandler *handler) {
  handlers.erase(handler);
}


TSPacketHandler::~TSPacketHandler() {
}

