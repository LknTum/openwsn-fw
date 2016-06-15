/**
<<<<<<< HEAD
\brief Automatically generated from ./tests/schedule_net1.json using schedule.py
\2016-06-15 16:29:06.187932
\@lkn{mvilgelm}
*/


#include "schedule.h"

void init_slotinfo();

slotinfo_element_t static_schedule_entries[2];


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
	static_schedule_entries[1].channelOffset = 1;
	static_schedule_entries[1].isUpdated = FALSE;
	static_schedule_entries[1].address[0] = 0x14;
	static_schedule_entries[1].address[1] = 0x15;
	static_schedule_entries[1].address[2] = 0x92;
	static_schedule_entries[1].address[3] = 0xcc;
	static_schedule_entries[1].address[4] = 00;
	static_schedule_entries[1].address[5] = 00;
	static_schedule_entries[1].address[6] = 00;
	static_schedule_entries[1].address[7] = 0x01;


}
