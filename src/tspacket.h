#ifndef _TSPACKET_H_
#define _TSPACKET_H_

#include <stdint.h>

class TSPacket {
 public:
  uint8_t *_block;
  uint8_t *_adaptionfield;
  uint8_t *_payload;

  TSPacket(uint8_t *block): _block(block) {}
  virtual ~TSPacket();

  virtual void validate();
  virtual void parse();
  
  uint8_t sync_byte() { return _block[0]; } // 8
  void    sync_byte(uint8_t byte) { _block[0]=byte; } // 8

  bool transport_error_indicator() { return _block[1] & 0x80; } // 1
  void transport_error_indicator(bool indicator) {  _block[1] = (indicator ? _block[1] | 0x80 : _block[1]  & 0x7f); } // 1

  bool payload_unit_start_indicator() { return _block[1] & 0x40; } // 1
  void payload_unit_start_indicator(bool indicator) { _block[1] = (indicator ? _block[1] | 0x40 : _block[1]  & 0xbf); } // 1

  bool transport_priority() { return _block[1] & 0x20; } // 1
  void transport_priority(bool prio) { _block[1] = (prio ? _block[1] | 0x20 : _block[1]  & 0xdf); } // 1

  uint16_t PID() { uint16_t ret = (_block[1] & 0x1F) << 8; return ret | _block[2]; } // 13
  void     PID(uint16_t pid) { 
    _block[1] = (_block[1] & 0xe0) | ((pid >> 8) & 0x1f);
    _block[2] = (pid & 0x00ff);
  }

  uint8_t transport_scrambing_control() { return (_block[3] & 0xc0)>>6; } // 2
  void    transport_scrambing_control(uint8_t tsc) { _block[3] = ((tsc & 0x03) << 6)|(_block[3] & 0x3f); } // 2

  uint8_t adaptation_field_control() { return (_block[3] & 0x30) >> 4; } // 2
  void    adaptation_field_control(uint8_t afc) { _block[3] = ((afc & 0x03) << 4)|(_block[3] & 0xcf); } // 2

  uint8_t continuity_counter() { return _block[3] & 0x0f; } // 4  
  void    continuity_counter(uint8_t cc) { _block[3] = (cc & 0x0f)|(_block[3] & 0xf0); } // 4  

  uint8_t *adaptionfield() { return _adaptionfield; }
  uint16_t adaptionfield_length() { return 0; }
  
  void set_adaptionfield(uint8_t *field, uint8_t length);

  uint8_t *payload() { return _payload; }


};


/*
struct PASEntry {
  uint8_t _data[4];
  uint16_t program_number() { uint16_t r = _data[0] << 8; return r | _data[1];  } // 16
  uint8_t reserved() {  return (_data[2] & 0xe0) >> 5; } // 3
  uint16_t PID() { uint16_t r = (_data[2] & 0x1f) << 8; return r | _data[3];  } // 13
};

class PASPacket: public TSPacket {
public:
  PASPacket(uint8_t *block):TSPacket(block) {}
  
  virtual void validate();

  uint8_t table_id() { return _payload[_payload[0]+1]; } // 8
  void    table_id(uint8_t id) { _payload[_payload[0]+1]=id; } // 8

  bool section_syntax_indicator() { return (_payload[_payload[0]+2] & 0x80); } // 1
  void section_syntax_indicator(bool ssi) { _payload[_payload[0]+2] =  ssi ? (_payload[_payload[0]+2] | 0x80) : (_payload[_payload[0]+2] & 0x7f); } 

  bool zero() { return (_payload[_payload[0]+2] & 0x40) != 0; } // 1
  void zero(bool z) { _payload[5] = z ? (_payload[_payload[0]+2] | 0x40):(_payload[_payload[0]+2] & 0xbf); } // 1

  uint8_t reserved() { return (_payload[_payload[0]+2] & 0x30) >> 4; } // 2
  void    reserved(uint8_t rs) { _payload[_payload[0]+2] = (_payload[_payload[0]+2] & 0xcf ) | ((rs &0x03) << 4); }

  uint16_t section_length() { uint16_t r = (_payload[_payload[0]+2] & 0x0f) << 8; return r | _payload[_payload[0]+3];  } // 12
  void     section_length(uint16_t sl) { 
    _payload[_payload[0]+3] = (sl & 0x00ff); 
    _payload[_payload[0]+2] = (_payload[_payload[0]+2] & 0xf0) | ((sl & 0x0f00) >> 8); 
  }
  
  uint16_t transport_stream_id() { return (_data[3] << 8) | _data[4]; }
  void     transport_stream_id(uint16_t tsi) { return (_data[3] << 8) | _data[4]; }

  uint8_t reserved2() { return (_data[5] & 0xc0) >> 6; }
  uint8_t version_number() { return (_data[5] & 0x3e) >> 1; }
  bool current_next_indicator() { return (_data[5] & 0x01) != 0; }
  uint8_t section_number() { return _data[6]; }
  uint8_t last_section_number() { return _data[7]; }
  PASEntry *entry(uint8_t i) { return (PASEntry *)(&_data[8+4*i]); }
  uint32_t crc32() {
    int offset = section_length()-1;
    uint32_t crc = _data[offset++];
    crc = (crc << 8) | _data[offset++];
    crc = (crc << 8) | _data[offset++];
    return (crc << 8) |_data[offset++];
  }
};
*/
#endif
