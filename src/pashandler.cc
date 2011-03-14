

#include "pashandler.h"
#include <iostream>

struct PASEntry {
  uint8_t _data[4];
  uint16_t program_number() { uint16_t r = _data[0] << 8; return r | _data[1];  } // 16
  uint8_t reserved() {  return (_data[2] & 0xe0) >> 5; } // 3
  uint16_t PID() { uint16_t r = (_data[2] & 0x1f) << 8; return r | _data[3];  } // 13
};

struct PASTable {
  uint8_t _data[184];
  uint8_t table_id() { return _data[0]; } // 8
  bool section_syntax_indicator() { return (_data[1] & 0x80) != 0; } // 1
  bool zero() { return (_data[1] & 0x40) != 0; } // 1
  uint8_t reserved() { return (_data[1] & 0x30) >> 4; } // 2
  uint16_t section_length() { uint16_t r = (_data[1] & 0x0f) << 8; return r | _data[2];  } // 12
  uint16_t transport_stream_id() { return (_data[3] << 8) | _data[4]; }
  uint8_t reserved2() { return (_data[5] & 0xc0) >> 6; }
  uint8_t version_number() { return (_data[5] & 0x3e) >> 1; }
  bool current_next_indicator() { return (_data[5] & 0x01) != 0; }
  uint8_t section_number() { return _data[6]; }
  uint8_t last_section_number() { return _data[7]; }
  PASEntry *entry(uint8_t i) { return (PASEntry *)(&_data[8+4*i]); }
};


PASHandler::~PASHandler() {
}


bool PASHandler::canHandle(TSPacket *packet) {
  return packet->header->PID() == 0;
}


void  PASHandler::handle(TSPacket *packet) {
  PASTable *pas = (PASTable *) (packet->data + packet->data[0] +1 );
  std::cout << "Table: " << (int)pas->table_id() << " Syn: " << pas->section_syntax_indicator();
  std::cout << " Zero: " << pas->zero() << " SecLen: " << pas->section_length() << std::endl;
  std::cout << "Program: " << pas->entry(0)->program_number() << " PID: " << pas->entry(0)->PID() << std::endl;
  program = pas->entry(0)->program_number();
  pid = pas->entry(0)->PID();
  out.write((char *)packet,188);
}

