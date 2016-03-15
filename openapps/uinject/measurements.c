#include "measurements.h"

//=========================== public ==========================================
void measurements_allocate(OpenQueueEntry_t* pkt){
	packetfunctions_reserveHeaderSize(pkt,sizeof(measurement_vars_t));
	return;
}

measurement_vars_t* measurement_retrievePointer(OpenQueueEntry_t* pkt){
	return (measurement_vars_t*) pkt->l4_payload[0];
}

void measurements_setHopAddr(OpenQueueEntry_t* pkt, uint8_t a){
	uint8_t index;
	measurement_vars_t* m;

	m=measurement_retrievePointer(pkt);
	index=measurement_findNextHopInfo(m);
	m->hopInfos[index].addr=a;

	return;
}

void measurements_setHopReTxCnt(OpenQueueEntry_t* pkt, uint8_t reTx){
	uint8_t index;
	measurement_vars_t* m;

	m=measurement_retrievePointer(pkt);
	index=measurement_findNextHopInfo(m);
	m->hopInfos[index].reTx_cnt=reTx;

	return;
}

void measurements_setHopFreq(OpenQueueEntry_t* pkt, uint8_t f){
	uint8_t index;
	measurement_vars_t* m;

	m=measurement_retrievePointer(pkt);
	index=measurement_findNextHopInfo(m);
	m->hopInfos[index].freq=f;

	return;
}

void measurements_setHopRssi(OpenQueueEntry_t* pkt, uint8_t r){
	uint8_t index;
	measurement_vars_t* m;

	m=measurement_retrievePointer(pkt);
	index=measurement_findNextHopInfo(m);
	m->hopInfos[index].rssi=r;

	return;
}

void measurements_setAsn(OpenQueueEntry_t* pkt, asn_t a){
	measurement_vars_t* m;

	m=measurement_retrievePointer(pkt);
	m->asn=a;
	return;
}

void measurements_setSeqNum(OpenQueueEntry_t* pkt, uint16_t seqNum){
	measurement_vars_t* m;

	m=measurement_retrievePointer(pkt);
	m->seqNumber=seqNum;
	return;
}


//=========================== private =========================================
uint8_t measurement_findNextHopInfo(measurement_vars_t* m){
	uint8_t i;
	//TODO check asn OR add hop count

	if(ieee154e_asnDiff(&m->asn)!=0)
	for(i=0;i<MAX_HOPS;i++){
		if((m->hopInfos[i].addr==0) && (m->hopInfos[i].freq==0)
			&& (m->hopInfos[i].reTx_cnt==0) && (m->hopInfos[i].rssi==0)){
			break;
		}
	}

	if(i==MAX_HOPS){
		return HOP_OVVERIDE_INDEX;
	}else{
		return i;
	}
}
