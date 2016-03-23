#ifndef __UINJECT_H
#define __UINJECT_H

/**
\addtogroup AppUdp
\{
\addtogroup uinject
\{
*/

#include "opentimers.h"
#include "opendefs.h"


//=========================== define ==========================================
///@lkn{LKN-Samu} Application periodicity
///@internal [LKN-uinject-settings]
#define UINJECT_PERIOD_MS 5000  // periodicity for periodic application
	// variance and mean for the non-periodic application
#define BURST_SILENCE_MS 10000  // The duration of silence before a burst occurs no packet is sent under silence mode
#define BURST_DURATION_MS 10000 // The duration of the burst mode
	//In total a cycle for bursty application is BURST_SILENCE_MS + BURST_DURATION_MS
#define BURST_PERIOD_MS 400	///< packet sending period in burst mode
// Too low values such as 100 creates problems
///@lkn{LKN-Murat} Application type 1-Burst 2-Non-Periodic 3-Periodic
#define APPFLAG 3
#define ENABLE_APPS TRUE
//=========================== typedef =========================================

typedef struct {
   opentimer_id_t       timerId;  ///< periodic timer which triggers transmission
   uint16_t             counter;  ///< incrementing counter which is written into the packet
} uinject_vars_t;


///@internal [LKN-uinject-settings]

//=========================== prototypes ======================================

void uinject_init(void);
void uinject_sendDone(OpenQueueEntry_t* msg, owerror_t error);
void uinject_receive(OpenQueueEntry_t* msg);


//=========================== define =========================================

#define MAX_HOPS 4 /// @lkn{Samu} Maximum number of hops of the network
#define HOP_OVVERIDE_INDEX MAX_HOPS-1 /// @lkn{Samu} setup to ovveride the lastest hopInfo in case of lack of memory

typedef struct {
   uint8_t addr;
   uint8_t reTx_cnt;
   uint8_t freq;
   uint8_t rssi;
} hopInfo_vars_t; //4B

typedef struct {
   asn_t asn;  //5B
   uint16_t seqNumber;  //2B
   //asn_t asn_stop;    //5B
   hopInfo_vars_t hopInfos[MAX_HOPS];  //MAX_HOPS*4B
} measurement_vars_t;

//=========================== typedef =========================================

/*
	@lkn{Samu} definition of a structure data type fore measurements information.
	The total size of the data strucure is: 5+2+MAX_HOPS(=4)*4B=23B
*/


void measurements_allocate(OpenQueueEntry_t* pkt);
//measurement_vars_t* measurement_retrievePointer(OpenQueueEntry_t* pkt);

void measurements_setHopAddr(OpenQueueEntry_t* pkt, uint8_t length, uint8_t addr);
void measurements_setHopReTxCnt(OpenQueueEntry_t* pkt, uint8_t length, uint8_t addr);
void measurements_setHopFreq(OpenQueueEntry_t* pkt, uint8_t length, uint8_t freq);
void measurements_setHopRssi(uint8_t* payload, uint8_t length, uint8_t rssi);

void measurements_setAsn(OpenQueueEntry_t* pkt, asn_t asn);
void measurements_setSeqNum(OpenQueueEntry_t* pkt, uint16_t seqNum);

//=========================== Private prototypes ======================================
uint8_t measurement_findNextHopInfo(measurement_vars_t* m, bool reception);
bool measurements_checkIfUinjectDst(open_addr_t * pkt_dst);



/**
\}
\}
*/

#endif
