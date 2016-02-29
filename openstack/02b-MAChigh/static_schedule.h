/**
\brief Automatically generated from schedule.py
\2016-02-29 09:10:59.032186
*/


#include "schedule.h"

void init_slotinfo();

slotinfo_element_t entries[2];


void init_slotinfo(){

	entries[0].slotOffset = 1;
	entries[0].link_type = CELLTYPE_TXRX;
	entries[0].shared = FALSE;
	entries[0].channelOffset = 1;
	entries[0].address[0] = 21;
	entries[0].address[1] = 146;
	entries[0].address[2] = 204;
	entries[0].address[3] = 00;
	entries[0].address[4] = 00;
	entries[0].address[5] = 00;
	entries[0].address[6] = 00;
	entries[0].address[7] = 04;


	entries[1].slotOffset = 1;
	entries[1].link_type = CELLTYPE_TXRX;
	entries[1].shared = FALSE;
	entries[1].channelOffset = 1;
	entries[1].address[0] = 21;
	entries[1].address[1] = 146;
	entries[1].address[2] = 204;
	entries[1].address[3] = 00;
	entries[1].address[4] = 00;
	entries[1].address[5] = 00;
	entries[1].address[6] = 00;
	entries[1].address[7] = 04;


}
