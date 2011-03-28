#include "tspacket.h"
#include <stdexcept>
#include "logger.h"

TSPacket::~TSPacket() {
}


void TSPacket::validate() const {
	if(_block[0] != 0x47) throw std::invalid_argument("Sync byte wrong !");
	if(transport_error_indicator()) throw std::invalid_argument("Transport Error");
	if(adaptionfield_length() > 184) throw std::invalid_argument("Adaptionfield too Long");
}

void TSPacket::parse() {
	if(adaptation_field_control() & 0x02) _adaptionfield = _block + 4; else _adaptionfield = NULL;
	if(adaptation_field_control() & 0x01)  {
		if(_adaptionfield != NULL)
			_payload = _block + 5 + adaptionfield_length();
		else
			_payload = _block + 4;
	} else {
		_payload = NULL;
	}
}


void TablePacket::validate() const {
	TSPacket::validate();
	if(_payload == NULL) throw std::invalid_argument("PAS Packet without payload");
	if(payload_length() < 13) throw std::invalid_argument("Payload too small.");
	if(payload_length() - _payload[0] < 12) throw std::invalid_argument("Not enough space for table.");
	if(zero()) throw std::invalid_argument("Zero bit is not zero.");
	if(!section_syntax_indicator()) throw std::invalid_argument("section_syntax_indicator is not set.");
	if(section_length() > payload_length() - _payload[0] - 4) throw std::invalid_argument("Section length > block, not supported.");
	if(section_length() < 9) throw std::invalid_argument("Section length < 11");
	if(calc_crc32() != crc32()) throw std::invalid_argument("CRC error.");

}


void TablePacket::parse() {
	TSPacket::parse();
	_table = _payload + 1 + _payload[0];
}


void PasPacket::validate() const {
	if(table_id() != 0) throw std::invalid_argument("Wrong TableId.");
	if((section_length() -9) % 4 != 0) throw std::invalid_argument("Bogus Sectionlength ( modulo 4 != 0).");
	if(entry_count() != 1) throw std::invalid_argument("I can only handle single Program streams");
}


void PmtPacket::parse() {
	_entries = _table + 12 + program_info_length();
}


void PmtPacket::validate() const {

}
