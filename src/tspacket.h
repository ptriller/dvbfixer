#ifndef _TSPACKET_H_
#define _TSPACKET_H_

#include <cstdint>
#include <cstddef>
#include "crc.h"
struct TSPacket {

	uint8_t *_block;
	uint8_t *_adaptionfield;
	uint8_t *_payload;

	TSPacket(uint8_t *block) : _block(block) {}

	void block(uint8_t *block) { _block = block; }

	virtual ~TSPacket();

	virtual void validate() const;
	virtual void parse();

	uint8_t sync_byte() const { return _block[0]; } // 8
	void    sync_byte(uint8_t byte) { _block[0]=byte; } // 8

	bool transport_error_indicator() const { return _block[1] & 0x80; } // 1
	void transport_error_indicator(bool indicator) {  _block[1] = (indicator ? _block[1] | 0x80 : _block[1]  & 0x7f); } // 1

	bool payload_unit_start_indicator() const { return _block[1] & 0x40; } // 1
	void payload_unit_start_indicator(bool indicator) { _block[1] = (indicator ? _block[1] | 0x40 : _block[1]  & 0xbf); } // 1

	bool transport_priority() const { return _block[1] & 0x20; } // 1
	void transport_priority(bool prio) { _block[1] = (prio ? _block[1] | 0x20 : _block[1]  & 0xdf); } // 1

	uint16_t PID() const { uint16_t ret = (_block[1] & 0x1F) << 8; return ret | _block[2]; } // 13
	void     PID(uint16_t pid) {
		_block[1] = (_block[1] & 0xe0) | ((pid >> 8) & 0x1f);
		_block[2] = (pid & 0x00ff);
	}

	uint8_t transport_scrambing_control() const { return (_block[3] & 0xc0)>>6; } // 2
	void    transport_scrambing_control(uint8_t tsc) { _block[3] = ((tsc & 0x03) << 6)|(_block[3] & 0x3f); } // 2

	uint8_t adaptation_field_control() const { return (_block[3] & 0x30) >> 4; } // 2
	void    adaptation_field_control(uint8_t afc) { _block[3] = ((afc & 0x03) << 4)|(_block[3] & 0xcf); } // 2

	uint8_t continuity_counter() const { return _block[3] & 0x0f; } // 4
	void    continuity_counter(uint8_t cc) { _block[3] = (cc & 0x0f)|(_block[3] & 0xf0); } // 4

	uint8_t *adaptionfield() const { return _adaptionfield; }
	uint16_t adaptionfield_length() const { if(_adaptionfield == NULL) return 0; return _adaptionfield[0]; }

	void set_adaptionfield(uint8_t *field, uint8_t length);

	uint8_t *payload() const { return _payload; }
	uint16_t payload_length() const { if(_payload == NULL) return 0; return 184 - adaptionfield_length(); }

};


struct TablePacket: public TSPacket {

	TablePacket(uint8_t *block) : TSPacket(block) {}

	uint8_t *_table;

	virtual void validate() const;
	virtual void parse();

	uint8_t pointer_field() const { return _payload[0]; }
	void pointer_field(uint8_t pointer) { _payload[0] = pointer; _table = _payload + 1 + _payload[0]; }

	uint8_t table_id() const { return _table[0]; }
	void table_id(uint8_t id) { _table[0] = id; }

	bool section_syntax_indicator() const { return _table[1] & 0x80; }

	bool zero() const { return _table[1] & 0x40; }

	inline uint8_t reserved() const { return (_table[1] & 0x30) >> 4; }

	uint16_t section_length() const { uint16_t r = (_table[1] & 0x0f) << 8; return r | _table[2]; }

	uint32_t crc32() const {
		int offset = section_length()-1;
		uint32_t crc = _table[offset++];
		crc = (crc << 8) | _table[offset++];
		crc = (crc << 8) | _table[offset++];
		return (crc << 8) |_table[offset];
	}

	void crc32(uint32_t crc) {
		uint32_t offset = section_length()+2;
		_table[offset--] = (crc & 0x000000ff);
		_table[offset--] = (crc & 0x0000ff00) >> 8;
		_table[offset--] = (crc & 0x00ff0000) >> 16;
		_table[offset] = (crc >> 24);

	}

	uint32_t calc_crc32() const {
		return ::crc32(_table, section_length()-1);
	}
};

struct PASEntry {

	uint8_t _data[4];

	uint16_t program_number() const { uint16_t r = _data[0] << 8; return r | _data[1];  } // 16

	uint8_t reserved() const {  return (_data[2] & 0xe0) >> 5; } // 3

	uint16_t PID() const { uint16_t r = (_data[2] & 0x1f) << 8; return r | _data[3];  } // 13
};


struct PasPacket : public TablePacket {

	PasPacket(uint8_t *block) : TablePacket(block) {}

	virtual void validate() const;

	uint16_t transport_stream_id() const { uint16_t r = (_table[3] << 8); return r | _table[4]; }

	uint8_t reserved2() const { return (_table[5] & 0xc0) >> 6; }

	uint8_t version_number() const { return (_table[5] & 0x3e) >> 1; }

	bool current_next_indicator() const { return (_table[5] & 0x01) != 0; }

	uint8_t section_number() const { return _table[6]; }

	uint8_t last_section_number() const { return _table[7]; }

	uint32_t entry_count() const { return (section_length() - 9)/4; }

	PASEntry *entry(uint8_t i) { return reinterpret_cast<PASEntry *>(_table + 8+4*i); }

};



struct PmtPacket : public TablePacket {

	uint8_t *_entries;

	PmtPacket(uint8_t *block) : TablePacket(block) {}

	virtual void validate() const;

	virtual void parse();

	uint16_t program_number() const { uint16_t r = _table[3] << 8; return r | _table[4]; }

	uint8_t reserved2() const { return (_table[5] & 0xc0) >> 6; }

	uint8_t version_number() const { return (_table[5] & 0x3e) >> 1; }

	bool current_next_indicator() const { return (_table[5] & 0x01) != 0; }

	uint8_t section_number() const { return _table[6]; }

	uint8_t last_section_number() const { return _table[7]; }

	uint8_t reserved3() const { return (_table[8] & 0xe0) >> 5; }

	uint8_t pcr_pid() const { uint16_t r = (_table[8] & 0x1f) << 8; return r | _table[9]; }

	uint8_t reserved4() const { return (_table[10] & 0xf0) >> 4; }

	uint16_t program_info_length() const { uint16_t r = (_table[10] & 0x0f) << 8; return r | _table[11]; }

	void program_info_length(uint16_t len) { _table[11] = (uint8_t)(len & 0x00ff); _table[10] = ((len >> 8) % 0x0f) | (_table[10] & 0xf0); }


};
#endif
