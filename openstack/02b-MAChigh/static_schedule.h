/**
\brief Automatically generated from schedule.json using schedule.py
\2016-02-29 14:54:11.931559
\@lkn{mvilgelm}
*/


#include "schedule.h"

void init_slotinfo();
void LKNschedule_addActiveSlots();

slotinfo_element_t entries[2]; /// @lkn{Samu} Number of entries must be equal the number of SCHEDULE_MINIMAL_6TISCH_ACTIVE_CELLS


void init_slotinfo(){

	entries[0].slotOffset = 1;
	entries[0].link_type = CELLTYPE_TXRX;
	entries[0].shared = FALSE;
	entries[0].channelOffset = 1;
	entries[0].address[0] = 0x14;
	entries[0].address[1] = 0x15;
	entries[0].address[2] = 0x92;
	entries[0].address[3] = 0xcc;
	entries[0].address[4] = 00;
	entries[0].address[5] = 00;
	entries[0].address[6] = 00;
	entries[0].address[7] = 0x03;


	entries[1].slotOffset = 2;
	entries[1].link_type = CELLTYPE_TXRX;
	entries[1].shared = FALSE;
	entries[1].channelOffset = 2;
	entries[1].address[0] = 21;
	entries[1].address[1] = 146;
	entries[1].address[2] = 204;
	entries[1].address[3] = 00;
	entries[1].address[4] = 00;
	entries[1].address[5] = 00;
	entries[1].address[6] = 00;
	entries[1].address[7] = 7;

}
