#include "opendefs.h"
#include "uinject.h"
#include "openudp.h"
#include "openqueue.h"
#include "opentimers.h"
#include "openserial.h"
#include "packetfunctions.h"
#include "scheduler.h"
#include "IEEE802154E.h"
#include "idmanager.h"

//=========================== variables =======================================

uinject_vars_t uinject_vars;
    static uint8_t flag=1;
    static uint32_t	counter=-1;  ///< incrementing counter for burst
#if 1
///@lkn{Samu} Application destination address set to the DAG root
///@internal [LKN-uinject-dest-addr]
static const uint8_t uinject_dst_addr[]   = {
   0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};
///@internal [LKN-uinject-dest-addr]
#endif

#if 0
static const uint8_t uinject_dst_addr[]   = {
   0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x14, 0x15, 0x92, 0xcc, 0x00, 0x00, 0x00, 0x01
};
#endif

//=========================== prototypes ======================================

void uinject_timer_cb(opentimer_id_t id);
void uinject_task_cb(void);

//=========================== public ==========================================

void uinject_init() {
static uint32_t 	APP_BASED_PERIOD_MS;  ///< msg periodicity constant

   // clear local variables
   memset(&uinject_vars,0,sizeof(uinject_vars_t));
   uinject_vars.counter=0;
   ///@internal [LKN-uinject-timer]
	if (APPFLAG==1) {
		if (flag==1) {
			APP_BASED_PERIOD_MS=BURST_SILENCE_MS;

			flag=0;

		}
		else{
			APP_BASED_PERIOD_MS=BURST_PERIOD_MS;

			flag=1;
		}
	}
	else if (APPFLAG==2){
		//srand(1) is assumed to be called here.
		APP_BASED_PERIOD_MS=UINJECT_PERIOD_MS;
		APP_BASED_PERIOD_MS=rand()%APP_BASED_PERIOD_MS;
		APP_BASED_PERIOD_MS+=UINJECT_PERIOD_MS/2;

	}

	else if	(APPFLAG==3)
		APP_BASED_PERIOD_MS=UINJECT_PERIOD_MS;
	else
		APP_BASED_PERIOD_MS=UINJECT_PERIOD_MS;


   ///@internal [LKN-uinject-timer]
   // start periodic timer
   uinject_vars.timerId                    = opentimers_start(
      APP_BASED_PERIOD_MS,
      TIMER_PERIODIC,TIME_MS,
      uinject_timer_cb
   );
}

void uinject_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openqueue_freePacketBuffer(msg);
}

void uinject_receive(OpenQueueEntry_t* pkt) {

   openqueue_freePacketBuffer(pkt);

   openserial_printError(
      COMPONENT_UINJECT,
      ERR_RCVD_ECHO_REPLY,
      (errorparameter_t)0,
      (errorparameter_t)0
   );
}

//=========================== private =========================================

void uinject_timer_cb(opentimer_id_t id){
   scheduler_push_task(uinject_task_cb,TASKPRIO_COAP);
}

/**
@brief Executes the core code the uninject application once the timer expires.

@lkn{Samu} Once the header of the packet is set, we increment the packet counter, and we reserve space for 5 bytes of additional information that will be stored later during the MAC layer operations.

\note timer fired, but we don't want to execute task in ISR mode instead, push
   task to scheduler with CoAP priority, and let scheduler take care of it.
*/

void uinject_task_cb() {
   OpenQueueEntry_t*    pkt;
   asn_t current_asn;

   // Debug Message
   //openserial_printData(pkt,40);

   // don't run if not synch
   if (ieee154e_isSynch() == FALSE){

return;}

   // don't run on dagroot
  if (idmanager_getIsDAGroot()) {
      opentimers_stop(uinject_vars.timerId);
      return;
   }

   // if you get here, send a packet

   // get a free packet buffer
   pkt = openqueue_getFreePacketBuffer(COMPONENT_UINJECT);
   if (pkt==NULL) {
      openserial_printInfo(COMPONENT_UINJECT,ERR_NO_FREE_PACKET_BUFFER,
                               (errorparameter_t)0,
                               (errorparameter_t)0);
      return;
   }

   pkt->owner                         = COMPONENT_UINJECT;
   pkt->creator                       = COMPONENT_UINJECT;
   pkt->l4_protocol                   = IANA_UDP;
   pkt->l4_destination_port           = WKP_UDP_INJECT;
   pkt->l4_sourcePortORicmpv6Type     = WKP_UDP_INJECT;
   pkt->l3_destinationAdd.type        = ADDR_128B;
   memcpy(&pkt->l3_destinationAdd.addr_128b[0],uinject_dst_addr,16);

   ///@internal [LKN-uinject-app]
   uinject_vars.counter++;
// Control the packet generation timer
// Non-periodic have to generate a new timer after each packet
   if (APPFLAG==2){
	opentimers_stop(uinject_vars.timerId);
	uinject_init();
	}
// Trigger burst after a silence period packet by modifying the timer parameter.
   else if (counter==-1 && APPFLAG==1){
	counter=BURST_DURATION_MS/BURST_PERIOD_MS;
	opentimers_stop(uinject_vars.timerId);
	uinject_init();
        }
// Count down till burst period ends
   else if (counter!=0 && APPFLAG==1){
	counter--;
        }
// Reset to silence after burst ends
   else if (APPFLAG==1){
	counter=-1;
	opentimers_stop(uinject_vars.timerId);
	uinject_init();
   }

   measurements_allocate(pkt);

   measurements_setSeqNum(pkt, uinject_vars.counter);

   ieee154e_getAsn(&current_asn);
   measurements_setAsn(pkt,current_asn);

   /*// OLD MEASUREMENT DATA MECHANISM
   packetfunctions_reserveHeaderSize(pkt,11*sizeof(uint8_t));
   *((uint8_t*)&pkt->payload[0]) = TXRETRIES+1;
   *((uint8_t*)&pkt->payload[1]) = uinject_vars.counter>>8;
   *((uint8_t*)&pkt->payload[2]) = uinject_vars.counter;
   *((uint8_t*)&pkt->payload[3]) = 0; //reserverd for frequency
   *((uint8_t*)&pkt->payload[4]) = 0; //reserved for RSSI
   *((uint8_t*)&pkt->payload[5]) = 0; //reserved for hop count
   *((uint8_t*)&pkt->payload[6]) = 0; //reserved for hop 1
   *((uint8_t*)&pkt->payload[7]) = 0; //reserved for hop 2
   *((uint8_t*)&pkt->payload[8]) = 0; //reserved for hop 3
   *((uint8_t*)&pkt->payload[9]) = 0; //reserved for hop 4
   *((uint8_t*)&pkt->payload[10]) = 0; //reserved for hop 5
   //*/
   if ((openudp_send(pkt))==E_FAIL) {
      openqueue_freePacketBuffer(pkt);
   }

   ///@internal [LKN-uinject-app]
   /*
   else{
      openserial_printInfo(COMPONENT_ICMPv6ECHO,ERR_UNEXPECTED_SENDDONE,
                               (errorparameter_t)4,
                               (errorparameter_t)4);
   }*/

}



//=========================== public ==========================================
void measurements_allocate(OpenQueueEntry_t* pkt){
	measurement_vars_t m;
	memset(&m,0,sizeof(measurement_vars_t));
	packetfunctions_reserveHeaderSize(pkt,sizeof(measurement_vars_t));
	*((measurement_vars_t*)&pkt->payload[0])=m;

	return;
}

/*measurement_vars_t* measurement_retrievePointer(OpenQueueEntry_t* pkt){
	return (measurement_vars_t*) pkt->payload;
}*/

bool measurements_checkIfUinjectDst(open_addr_t * pkt_dst){

  open_addr_t dst_addr;
  memset(&dst_addr, 0, sizeof(open_addr_t));

  packetfunctions_readAddress(uinject_dst_addr, ADDR_128B, &dst_addr, FALSE);

  if(packetfunctions_sameAddress( pkt_dst, &dst_addr))
    return TRUE;
  else
    return FALSE;
}

void measurements_setHopAddr(OpenQueueEntry_t* pkt, uint8_t length, uint8_t a){

  if(measurements_checkIfUinjectDst(&pkt->l3_destinationAdd)){
  	uint8_t index;
		measurement_vars_t* m;

		m=(measurement_vars_t*) &pkt->payload[length - sizeof(measurement_vars_t)];

    //pkt->payload[length - 3] = 0x99;

		index=measurement_findNextHopInfo(m,FALSE);

 /*   openserial_printError(COMPONENT_IEEE802154E,ERR_MAXTXACKPREPARE_OVERFLOWS,
                         (errorparameter_t)index,
                         (errorparameter_t)77);*/

		m->hopInfos[index].addr=a;
	}
  else {
}
	return;
}

void measurements_setHopReTxCnt(OpenQueueEntry_t* pkt, uint8_t length, uint8_t reTx){
  if(measurements_checkIfUinjectDst(&pkt->l3_destinationAdd)){
		uint8_t index;
		measurement_vars_t* m;

		m=(measurement_vars_t*) &pkt->payload[length - sizeof(measurement_vars_t)];

		index=measurement_findNextHopInfo(m,FALSE);
		m->hopInfos[index].reTx_cnt=reTx;
	}
	return;
}

void measurements_setHopFreq(OpenQueueEntry_t* pkt, uint8_t length, uint8_t f){
	if(measurements_checkIfUinjectDst(&pkt->l3_destinationAdd)){
		uint8_t index;
		measurement_vars_t* m;

		m=(measurement_vars_t*) &pkt->payload[length - sizeof(measurement_vars_t)];

		index=measurement_findNextHopInfo(m,FALSE);
		m->hopInfos[index].freq=f;
	}
	return;
}

void measurements_setHopRssi(uint8_t* payload, uint8_t length, uint8_t r){

if (length>70 && length<80){

	uint8_t index;
	measurement_vars_t* m;

  m = (measurement_vars_t*) &payload[length - sizeof(measurement_vars_t) - 2];

	m->hopInfos[measurement_findNextHopInfo(m, TRUE)].rssi=r;

}
else {

/*    openserial_printError(COMPONENT_IEEE802154E,ERR_MAXTXDATAPREPARE_OVERFLOW,
                     (errorparameter_t)99,
                     (errorparameter_t)99);
*/
}
	return;
}

void measurements_setAsn(OpenQueueEntry_t* pkt, asn_t a){
	if(measurements_checkIfUinjectDst(&pkt->l3_destinationAdd)){
		measurement_vars_t* m;
		m=(measurement_vars_t*) pkt->payload;
		m->asn=a;
	}
	return;
}

void measurements_setSeqNum(OpenQueueEntry_t* pkt, uint16_t seqNum){
	if(measurements_checkIfUinjectDst(&pkt->l3_destinationAdd)){
		measurement_vars_t* m;

		m=(measurement_vars_t*) pkt->payload;
		m->seqNumber=seqNum;
	}
	return;
}


//=========================== private =========================================
/**
@brief The function returns a entry which has address 0 or my address.
In either case the entry should be used for writing/updating retx and frequency (if sent), and rssi and address (if received).

@lkn{mvilgelm}
*/
uint8_t measurement_findNextHopInfo(measurement_vars_t* m, bool reception){
	uint8_t i;
	//TODO check asn OR add hop count
#if 1
	for(i=0;i<MAX_HOPS;i++){
    // check whether it is my address. If yes, return the entry
      if ((m->hopInfos[i].addr==0) || (m->hopInfos[i].addr==(idmanager_getMyID(ADDR_64B)->addr_64b[7])))
          break;
	}

  /*openserial_printInfo(COMPONENT_ICMPv6ECHO,ERR_UNEXPECTED_SENDDONE,
                      (errorparameter_t)i,
                      (errorparameter_t)77);*/

	if(i==MAX_HOPS){
		return HOP_OVVERIDE_INDEX;
	}
  else {
    if (reception)
      return i-1;
    else
      return i;
  }
  #endif

  //return 1;
}


