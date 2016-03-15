#ifndef __MEASUREMENTS_H
#define __MEASUREMENTS_H

#include "opendefs.h"

//=========================== define =========================================

#define MAX_HOPS 4 /// @lkn{Samu} Maximum number of hops of the network
#define HOP_OVVERIDE_INDEX MAX_HOPS-1 /// @lkn{Samu} setup to ovveride the lastest hopInfo in case of lack of memory

//=========================== typedef =========================================

/*
	@lkn{Samu} definition of a structure data type fore measurements information.
	The total size of the data strucure is: 5+2+MAX_HOPS(=4)*4B=23B
*/

typedef struct {
	uint8_t addr;
	uint8_t reTx_cnt;
	uint8_t freq;
	uint8_t rssi;
} hopInfo_vars_t; //4B

typedef struct {
	asn_t asn;	//5B
	uint16_t seqNumber;	//2B
	//asn_t asn_stop;		//5B
	hopInfo_vars_t hopInfos[MAX_HOPS];	//MAX_HOPS*4B
} measurement_vars_t;

void measurements_allocate(OpenQueueEntry_t* pkt);
measurement_vars_t* measurement_retrievePointer(OpenQueueEntry_t* pkt);

void measurements_setHopAddr(OpenQueueEntry_t* pkt, uint8_t addr);
void measurements_setHopReTxCnt(OpenQueueEntry_t* pkt, uint8_t addr);
void measurements_setHopFreq(OpenQueueEntry_t* pkt, uint8_t freq);
void measurements_setHopRssi(OpenQueueEntry_t* pkt, uint8_t rssi);

void measurements_setAsn(OpenQueueEntry_t* pkt, asn_t asn);
void measurements_setSeqNum(OpenQueueEntry_t* pkt, uint16_t seqNum);

//=========================== Private prototypes ======================================
uint8_t measurement_findNextHopInfo();

#endif
