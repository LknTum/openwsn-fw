/**
\brief Automatically generated from schedule_net2.json using schedule.py
\2016-06-07 11:23:44.149470
\@lkn{mvilgelm}
*/


#include "schedule.h"

void init_slotinfo();

slotinfo_element_t static_schedule_entries[4];


void init_slotinfo(){

	static_schedule_entries[0].slotOffset = 1;
	static_schedule_entries[0].link_type = CELLTYPE_TXRX;
	static_schedule_entries[0].shared = FALSE;
	static_schedule_entries[0].channelOffset = 1;
	static_schedule_entries[0].isUpdated = FALSE;
	static_schedule_entries[0].address[0] = 0x14;
	static_schedule_entries[0].address[1] = 0x15;
	static_schedule_entries[0].address[2] = 0x92;
	static_schedule_entries[0].address[3] = 0xcc;
	static_schedule_entries[0].address[4] = 00;
	static_schedule_entries[0].address[5] = 00;
	static_schedule_entries[0].address[6] = 00;
	static_schedule_entries[0].address[7] = 0x99;


	static_schedule_entries[1].slotOffset = 2;
	static_schedule_entries[1].link_type = CELLTYPE_TXRX;
	static_schedule_entries[1].shared = FALSE;
	static_schedule_entries[1].channelOffset = 2;
	static_schedule_entries[1].isUpdated = FALSE;
	static_schedule_entries[1].address[0] = 0x14;
	static_schedule_entries[1].address[1] = 0x15;
	static_schedule_entries[1].address[2] = 0x92;
	static_schedule_entries[1].address[3] = 0xcc;
	static_schedule_entries[1].address[4] = 00;
	static_schedule_entries[1].address[5] = 00;
	static_schedule_entries[1].address[6] = 00;
	static_schedule_entries[1].address[7] = 0x04;


	static_schedule_entries[2].slotOffset = 3;
	static_schedule_entries[2].link_type = CELLTYPE_TXRX;
	static_schedule_entries[2].shared = FALSE;
	static_schedule_entries[2].channelOffset = 2;
	static_schedule_entries[2].isUpdated = FALSE;
	static_schedule_entries[2].address[0] = 0x14;
	static_schedule_entries[2].address[1] = 0x15;
	static_schedule_entries[2].address[2] = 0x92;
	static_schedule_entries[2].address[3] = 0xcc;
	static_schedule_entries[2].address[4] = 00;
	static_schedule_entries[2].address[5] = 00;
	static_schedule_entries[2].address[6] = 00;
	static_schedule_entries[2].address[7] = 0x05;


	static_schedule_entries[3].slotOffset = 4;
	static_schedule_entries[3].link_type = CELLTYPE_TXRX;
	static_schedule_entries[3].shared = FALSE;
	static_schedule_entries[3].channelOffset = 2;
	static_schedule_entries[3].isUpdated = FALSE;
	static_schedule_entries[3].address[0] = 0x14;
	static_schedule_entries[3].address[1] = 0x15;
	static_schedule_entries[3].address[2] = 0x92;
	static_schedule_entries[3].address[3] = 0xcc;
	static_schedule_entries[3].address[4] = 00;
	static_schedule_entries[3].address[5] = 00;
	static_schedule_entries[3].address[6] = 00;
	static_schedule_entries[3].address[7] = 0x06;


}
