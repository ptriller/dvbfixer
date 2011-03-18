#include "pmthandler.h"

#include "crc.h"
#include <iostream>
#include <string.h>

struct PMTEntry {
  uint8_t _data[184];
  uint8_t stream_type() { return _data[0]; }
  uint8_t reserved() { return (_data[1] & 0xe0) >> 5; }
  uint16_t elementary_PID() { uint16_t r = (_data[1] &0x1f) << 8; return r | _data[2]; }
  uint8_t reserved2() { return (_data[3] & 0xf0) >> 4; }
  uint16_t ES_info_length() { uint16_t r = (_data[3] & 0x0f) << 8; return r | _data[4]; }
};

struct PMTTable {
  uint8_t _data[183];

  uint8_t table_id() { return _data[0]; }
  bool section_syntax_indicator() { return (_data[1] & 0x80) != 0; }
  bool zero() { return (_data[1] & 0x40) != 0; }
  uint8_t reserved() { return (_data[1] & 0x30) >> 4; }
  uint16_t section_length() { uint16_t r = (_data[1] & 0x0f) << 8; return r | _data[2]; }
  void section_length(uint16_t len) { _data[2] = (uint8_t)(len & 0x00ff); _data[1] = ((len >> 8) & 0x0f) | (_data[1] & 0xf0); }
  uint16_t program_number() { uint16_t r = _data[3] << 8; return r | _data[4]; }
  uint8_t reserved2() { return (_data[5] & 0xc0) >> 6; }
  uint8_t version_number() { return (_data[5] & 0x3e) >> 1; }
  bool current_next_indicator() { return (_data[5] & 0x01) != 0; }
  uint8_t section_number() { return _data[6]; }
  uint8_t last_section_number() { return _data[7]; }
  uint8_t reserved3() { return (_data[8] & 0xe0) >> 5; }
  uint8_t pcr_pid() { uint16_t r = (_data[8] & 0x1f) << 8; return r | _data[9]; }
  uint8_t reserved4() { return (_data[10] & 0xf0) >> 4; }
  uint16_t program_info_length() { uint16_t r = (_data[10] & 0x0f) << 8; return r | _data[11]; }
  void program_info_length(uint16_t len) { _data[11] = (uint8_t)(len & 0x00ff); _data[10] = ((len >> 8) % 0x0f) | (_data[10] & 0xf0); }
  uint32_t crc32() {
    int offset = section_length()-1;
    uint32_t crc = _data[offset++];
    crc = (crc << 8) | _data[offset++];
    crc = (crc << 8) | _data[offset++];
    return (crc << 8) |_data[offset++];
  }

  void crc32(uint32_t crc) {
    int offset = section_length()+2;
    _data[offset--] = crc & 0xff;
    _data[offset--] = (crc >> 8)  & 0xff;
    _data[offset--] = (crc >> 16) & 0xff;
    _data[offset--] = (crc >> 24) & 0xff;
  }

  PMTEntry *firstPMT() {  
    uint8_t *e = _data+12+program_info_length(); 
    return e < _data+section_length() ? (PMTEntry *)e : NULL; 
  }
  PMTEntry *nextPMT(PMTEntry *p) {
    uint8_t *e = p->_data + 5+p->ES_info_length();
    return e < _data+section_length() ? (PMTEntry *)e : NULL; 
  }
  
};


bool PMTHandler::canHandle(TSPacket *packet) {

  return packet->header->PID() == pid && packet->header->payload_unit_start_indicator();
}
  
void PMTHandler::handle(TSPacket *packet) {
  uint8_t newblock[188];
  
  PMTTable *pmt = (PMTTable *) (packet->data + packet->data[0] + 1 );

  memcpy(newblock,packet->_data,188);
  TSPacket newpacket;
  TSPacketReader::parseBlock(newblock,newpacket);
  PMTTable *newpmt = (PMTTable *) (newpacket.data + newpacket.data[0] + 1 );
  
  uint16_t newsize = 9+newpmt->program_info_length();
  uint8_t *newentry = (uint8_t *)pmt->firstPMT();
  PMTEntry *entry = pmt->firstPMT();
  streams.clear();
  streams.insert(pmt->pcr_pid());
  unsigned int crc1 = crc32(pmt->_data, pmt->section_length()-1);
  std::cout << "CRC: 0x" << std::hex << pmt->crc32() << std::dec << std::endl;
  std::cout << "Info len: " << pmt->program_info_length() << std::endl;
  std::cout << "CRC1: 0x" << std::hex << crc1 << std::dec << std::endl;
  if(crc1 != pmt->crc32()) {
    throw std::invalid_argument("CRC Error in PAS packet !");
  }
  
  std::cout << "PCR: " << (unsigned int)pmt->pcr_pid() << std::endl;
  while(entry != NULL) {
    std::cout << "Type: " << (int)entry->stream_type() << " PID: " << entry->elementary_PID() << std::endl;
    uint16_t len=0;
    switch((unsigned int)entry->stream_type()) {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x1b:
      std::cout << "Adding: " <<  entry->elementary_PID() << std::endl;
      streams.insert(entry->elementary_PID());
      len = 5+entry->ES_info_length();
      std::cout << "Kept len:  " << len << std::endl;
      for(int i = 0; i < len;++i) newentry[i] = entry->_data[i];
      newentry += len;
      newsize += len;
      break;
    default:
      len = 5+entry->ES_info_length();
      std::cout << "Skipped len:  " << len << std::endl;
      break;
    }
    entry = pmt->nextPMT(entry);
  }
  std::cout << "Len: " << newsize + 4 << std::endl;
  std::cout << "OldLen: " << pmt->section_length() << std::endl;
  newpmt->section_length(newsize+4);
  newpmt->crc32(crc32(newpmt->_data, newpmt->section_length()-1));
  out.write((char *)newblock,188);
}
