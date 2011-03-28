#include "config.h"
#include "packetreader.h"
#include "stream.h"
#include "logger.h"
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
		if(queue.size() < BLOCK_SIZE) {
			if(queue.size() > 0) {
				WARN("Stream had " << queue.size() << " Bytes of garbage at the end");
			}
			return false;
		}
	}
	// Good stream, just walk on if all is well
	if(streak >=3 && queue[0] == 0x47) {
		streak++;
		std::copy(queue.begin(),queue.begin()+BLOCK_SIZE,buf);
		queue.erase(queue.begin(),queue.begin()+BLOCK_SIZE);
	} else {
		bool good = true;
		do {
			if(queue.size() < BLOCK_SIZE*(4-streak)) {
				DEBUG("Try to fill up Buffer");
				fillBuffer();
				if(queue.size() < BLOCK_SIZE) return false;
			}
			good = true;
			for(unsigned int i = 0; i < 4; ++i) {
				if(queue.size() >= (1+i)*BLOCK_SIZE && queue[i*BLOCK_SIZE] != 0x47) {
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
		WARN("Skipped " << errorbytes << " Bytes");
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
		INFO("Read: " << (totalread+len) /(1024*1024) << " Mb");
	}
	totalread += len;
	if(len > 0) {
		if(0 == (read = reader->read(buf, len))) {
			INFO("End of Stream reached");
			return false;
		}

		std::copy(buf,buf+len, std::back_inserter(queue) );

	}
	return true;
}


void TSPacketReader::emitEvents(uint8_t *block) {
	TSPacket packet(block);
	packet.parse();
	packet.validate();
	for(auto it = handlers.begin(); it != handlers.end();++it) {
		try {
			if((*it)->canHandle(&packet)) (*it)->handle(&packet);
		} catch(std::invalid_argument &e) {
			WARN("Invalid packet, rewind. :" << e.what() );
			streak = 0;
			for(uint8_t *b = block+187; b != block;--b) queue.push_front(*b);
			//      std::reverse_copy(block,block+188, std::front_inserter(queue) );
		}
	}
//	delete packet;
}


void TSPacketReader::registerPacketHandler(TSPacketHandler *handler) {
	handlers.insert(handler);
}

void TSPacketReader::unregisterPacketHander(TSPacketHandler *handler) {
	handlers.erase(handler);
}


TSPacketHandler::~TSPacketHandler() {
}
