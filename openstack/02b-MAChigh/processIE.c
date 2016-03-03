#include "opendefs.h"
#include "processIE.h"
#include "sixtop.h"
#include "idmanager.h"
#include "openserial.h"
#include "IEEE802154.h"
#include "openqueue.h"
#include "neighbors.h"
#include "IEEE802154E.h"
#include "schedule.h"
#include "scheduler.h"
#include "packetfunctions.h"

//=========================== variables =======================================

//=========================== public ==========================================

port_INLINE void processIE_prependMLMEIE(
      OpenQueueEntry_t* pkt,
      uint8_t           len
   ){
   payload_IE_ht payload_IE_desc;

   // reserve space
   packetfunctions_reserveHeaderSize(
      pkt,
      sizeof(payload_IE_ht)
   );

   // prepare header
   payload_IE_desc.length_groupid_type = len;
   payload_IE_desc.length_groupid_type |=
      (IEEE802154E_PAYLOAD_DESC_GROUP_ID_MLME  | IEEE802154E_PAYLOAD_DESC_TYPE_MLME);

   // copy header
   pkt->payload[0] = payload_IE_desc.length_groupid_type        & 0xFF;
   pkt->payload[1] = (payload_IE_desc.length_groupid_type >> 8) & 0xFF;
}

port_INLINE void processIE_prepend_sixtopIE(
      OpenQueueEntry_t* pkt,
      uint8_t           len
   ){
   payload_IE_ht payload_IE_desc;

   // reserve space
   packetfunctions_reserveHeaderSize(
      pkt,
      sizeof(payload_IE_ht)
   );

   // prepare header
   payload_IE_desc.length_groupid_type = len;
   payload_IE_desc.length_groupid_type |=
      (IANA_6TOP_IE_GROUP_ID  | IANA_6TOP_IE_GROUP_ID_TYPE);

   // copy header
   pkt->payload[0] = payload_IE_desc.length_groupid_type        & 0xFF;
   pkt->payload[1] = (payload_IE_desc.length_groupid_type >> 8) & 0xFF;
}


//===== prepend IEs

port_INLINE uint8_t processIE_prependSyncIE(OpenQueueEntry_t* pkt){
   mlme_IE_ht mlme_subHeader;
   uint8_t    len;

   len = 0;

   //=== sync IE

   // reserve space
   packetfunctions_reserveHeaderSize(
      pkt,
      sizeof(sync_IE_ht)
   );

   // Keep a pointer to where the ASN will be
   // Note: the actual value of the current ASN and JP will be written by the
   //    IEEE802.15.4e when transmitting
   pkt->l2_ASNpayload               = pkt->payload;

   len += sizeof(sync_IE_ht);

   //=== MLME IE

   // reserve space
   packetfunctions_reserveHeaderSize(
      pkt,
      sizeof(mlme_IE_ht)
   );

   // prepare header
   mlme_subHeader.length_subID_type = sizeof(sync_IE_ht);
   mlme_subHeader.length_subID_type |=
      (IEEE802154E_MLME_SYNC_IE_SUBID << IEEE802154E_MLME_SYNC_IE_SUBID_SHIFT)|
      IEEE802154E_DESC_TYPE_SHORT;

   // copy header
   pkt->payload[0]= mlme_subHeader.length_subID_type        & 0xFF;
   pkt->payload[1]= (mlme_subHeader.length_subID_type >> 8) & 0xFF;

   len += 2;

   return len;
}

port_INLINE uint8_t processIE_prepend_sixSubIEHeader(
    OpenQueueEntry_t*    pkt,
    uint8_t len
  ){
    mlme_IE_ht mlme_subHeader;
    //===== MLME IE header
    // reserve space
    packetfunctions_reserveHeaderSize(pkt, sizeof(mlme_IE_ht));

    // prepare header
    mlme_subHeader.length_subID_type  = len;
    mlme_subHeader.length_subID_type |=
        (IANA_6TOP_SUBIE_ID << MLME_IE_SUBID_SHIFT) |
        IEEE802154E_DESC_TYPE_SHORT;

    // copy header
    pkt->payload[0] = mlme_subHeader.length_subID_type       & 0xFF;
    pkt->payload[1] = (mlme_subHeader.length_subID_type >> 8)& 0xFF;

    return 2;
}

port_INLINE uint8_t processIE_prepend_sixGeneralMessage(
    OpenQueueEntry_t*    pkt,
    uint8_t code
    ){
    uint8_t    len = 0;
    uint8_t    temp8b;

    //===== SFID
    packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));
    *((uint8_t*)(pkt->payload)) = SFID_SF0;
    len += 1;

    pkt->l2_sixtop_returnCode = code;

    //===== version & code
    packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));
    temp8b  = IANA_6TOP_6P_VERSION;
    temp8b |= code << 4;
    *((uint8_t*)(pkt->payload)) = temp8b;
    len += 1;

    return len;
}

port_INLINE uint8_t processIE_prepend_sixSubID(OpenQueueEntry_t*    pkt){
    uint8_t    len = 0;

    //===== SFID
    packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));
    *((uint8_t*)(pkt->payload)) = IANA_6TOP_SUBIE_ID;
    len += 1;

    return len;
}

port_INLINE uint8_t processIE_prepend_sixCelllist(
    OpenQueueEntry_t*    pkt,
    cellInfo_ht*         cellList
   ){
    uint8_t    i;
    uint8_t    len;
    uint8_t    numOfCells;

    len        = 0;
    numOfCells = 0;

    //===== cell list

    for(i=0;i<SCHEDULEIEMAXNUMCELLS;i++) {
        if(cellList[i].linkoptions != CELLTYPE_OFF){
            // cellobjects:
            // - [2B] slotOffset
            // - [2B] channelOffset
            // - [1B] link_type
            packetfunctions_reserveHeaderSize(pkt,4);
            pkt->payload[0] = (uint8_t)(cellList[i].tsNum  & 0x00FF);
            pkt->payload[1] = (uint8_t)((cellList[i].tsNum & 0xFF00)>>8);
            pkt->payload[2] = (uint8_t)(cellList[i].choffset  & 0x00FF);
            pkt->payload[3] = (uint8_t)((cellList[i].choffset & 0xFF00)>>8);
            len += 4;
            numOfCells++;
        }
    }

    // record the position of cellObjects
    pkt->l2_sixtop_numOfCells  = numOfCells;
    pkt->l2_sixtop_cellObjects = pkt->payload;

    return len;
}

/**
@brief The function aggregates the information relative to the scheduling of the different motes into a single outgoing message.

@lkn{Samu} We modified the function such that additional parameters concerning the link options and target address could be added to the packet. In particular, the 6-th element of the payload (payload[5]) will be used to store the address of the link-scheduling, and the @ref linkOption contains meaningful information about the link state.

@warning Hardcoded based on our current scheduling
@todo implement dynamic loading of the schedule


@param Packet that is going to be sent.
*/
port_INLINE uint8_t processIE_prependSlotframeLinkIE(OpenQueueEntry_t* pkt){
   mlme_IE_ht mlme_subHeader;
   uint8_t           len;
   uint8_t           linkOption;
   slotOffset_t      slotOffset;
   slotOffset_t      lastSlotOffset;
   frameLength_t     frameLength;
   uint8_t i;

   len            = 0;
   linkOption     = 0;
   lastSlotOffset = SCHEDULE_MINIMAL_6TISCH_SLOTOFFSET + SCHEDULE_MINIMAL_6TISCH_ACTIVE_CELLS;

   /// @internal [LKN-prepend-link-scheduling]

   // For each link in the default schedule, add:
   // - [1B] linkOption bitmap
   // - [2B] channel offset
   // - [2B] timeslot
   // - [3B] Neighbour value -NEW

   //===== shared cells
   linkOption = (1<<FLAG_TX_S)|(1<<FLAG_RX_S)|(1<<FLAG_SHARED_S)|(1<<FLAG_TIMEKEEPING_S);
   i=1;
   for (slotOffset=lastSlotOffset;slotOffset>SCHEDULE_MINIMAL_6TISCH_SLOTOFFSET;slotOffset--) {
      packetfunctions_reserveHeaderSize(pkt,6);
      pkt->payload[0]   = (slotOffset-1)        & 0xFF;				// slotOffset
      pkt->payload[1]   = ((slotOffset-1) >> 8) & 0xFF;				//
      pkt->payload[2]   = SCHEDULE_MINIMAL_6TISCH_CHANNELOFFSET;    // channel offset
      pkt->payload[3]   = 0x00;
      pkt->payload[4]   = linkOption;                          		// linkOption bitmap
      pkt->payload[5]   = slotOffset-1; 					   		// Address value
	  i++;
      len+=6;
   }
   /// @internal [LKN-prepend-link-scheduling]

   //===== slotframe IE header

   // - [1B] number of links (6)
   // - [2B] Slotframe Size (101)
   // - [1B] slotframe handle (id)
   frameLength = schedule_getFrameLength();
   packetfunctions_reserveHeaderSize(pkt,5);
   pkt->payload[0] = schedule_getFrameNumber();
   pkt->payload[1] = schedule_getFrameHandle();
   pkt->payload[2] =  frameLength       & 0xFF;
   pkt->payload[3] = (frameLength >> 8) & 0xFF;
   pkt->payload[4] = SCHEDULE_MINIMAL_6TISCH_ACTIVE_CELLS; //number of links

   len+=5;

   //===== MLME IE header
   // - [1b] 15 short ==0x00
   // - [7b] 8-14 Sub-ID=0x1b
   // - [8b] Length

   // reserve space
   packetfunctions_reserveHeaderSize(
      pkt,
      sizeof(mlme_IE_ht)
   );

   // prepare header
   mlme_subHeader.length_subID_type = len;
   mlme_subHeader.length_subID_type |=
      (IEEE802154E_MLME_SLOTFRAME_LINK_IE_SUBID <<
         IEEE802154E_MLME_SLOTFRAME_LINK_IE_SUBID_SHIFT) |
      IEEE802154E_DESC_TYPE_SHORT;

   // copy header
   pkt->payload[0]= mlme_subHeader.length_subID_type        & 0xFF;
   pkt->payload[1]= (mlme_subHeader.length_subID_type >> 8) & 0xFF;

   len+=2;

   return len;
}

port_INLINE uint8_t processIE_prependTSCHTimeslotIE(OpenQueueEntry_t* pkt){
   uint8_t    len;
   mlme_IE_ht mlme_subHeader;

   len = 0;

   // reserve space for timeslot template ID
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));
   // write header
   *((uint8_t*)(pkt->payload)) = TIMESLOT_TEMPLATE_ID;

   len+=1;

   //===== MLME IE header

   // reserve space
   packetfunctions_reserveHeaderSize(pkt, sizeof(mlme_IE_ht));

   // prepare header
   mlme_subHeader.length_subID_type  = len;
   mlme_subHeader.length_subID_type |=
      IEEE802154E_MLME_TIMESLOT_IE_SUBID << IEEE802154E_MLME_TIMESLOT_IE_SUBID_SHIFT|
      IEEE802154E_DESC_TYPE_SHORT;

   // copy header
   pkt->payload[0] = mlme_subHeader.length_subID_type        & 0xFF;
   pkt->payload[1] = (mlme_subHeader.length_subID_type >> 8) & 0xFF;

   len += 2;

   return len;
}
port_INLINE uint8_t processIE_prependChannelHoppingIE(OpenQueueEntry_t* pkt){
   uint8_t    len;
   mlme_IE_ht mlme_subHeader;

   len = 0;

   // reserve space for timeslot template ID
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));
   // write header
   *((uint8_t*)(pkt->payload)) = CHANNELHOPPING_TEMPLATE_ID;

   len+=1;

   //===== MLME IE header

   // reserve space
   packetfunctions_reserveHeaderSize(pkt, sizeof(mlme_IE_ht));

   // prepare header
   mlme_subHeader.length_subID_type  = len;
   mlme_subHeader.length_subID_type |=
      IEEE802154E_MLME_CHANNELHOPPING_IE_SUBID << IEEE802154E_MLME_CHANNELHOPPING_IE_SUBID_SHIFT|
      IEEE802154E_DESC_TYPE_LONG;

   // copy header
   pkt->payload[0] = mlme_subHeader.length_subID_type        & 0xFF;
   pkt->payload[1] = (mlme_subHeader.length_subID_type >> 8) & 0xFF;

   len += 2;

   return len;
}

port_INLINE uint8_t processIE_prependOpcodeIE(
      OpenQueueEntry_t* pkt,
      uint8_t           uResCommandID
   ){
   uint8_t    len;
   mlme_IE_ht mlme_subHeader;

   len = 0;

   //===== command ID

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // write header
   *((uint8_t*)(pkt->payload)) = uResCommandID;

   len += 1;

   //===== MLME IE header

   // reserve space
   packetfunctions_reserveHeaderSize(pkt, sizeof(mlme_IE_ht));

   // prepare header
   mlme_subHeader.length_subID_type  = len;
   mlme_subHeader.length_subID_type |=
      MLME_IE_SUBID_OPCODE << MLME_IE_SUBID_SHIFT|
      IEEE802154E_DESC_TYPE_SHORT;

   // copy header
   pkt->payload[0] = mlme_subHeader.length_subID_type        & 0xFF;
   pkt->payload[1] = (mlme_subHeader.length_subID_type >> 8) & 0xFF;

   len += 2;

   return len;
}

port_INLINE uint8_t processIE_prependBandwidthIE(
      OpenQueueEntry_t* pkt,
      uint8_t           numOfLinks,
      uint8_t           slotframeID
   ){

   uint8_t    len;
   mlme_IE_ht mlme_subHeader;

   len = 0;

   //===== number of cells

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // write header
   *((uint8_t*)(pkt->payload)) = numOfLinks;

   len += 1;

   //===== number of cells

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // write header
   *((uint8_t*)(pkt->payload)) = slotframeID;

   len += 1;

   //===== MLME IE header

   // reserve space
   packetfunctions_reserveHeaderSize(pkt, sizeof(mlme_IE_ht));

   // prepare header
   mlme_subHeader.length_subID_type  = len;
   mlme_subHeader.length_subID_type |=
      (MLME_IE_SUBID_BANDWIDTH <<
         MLME_IE_SUBID_SHIFT) |
      IEEE802154E_DESC_TYPE_SHORT;

   // copy header
   pkt->payload[0] = mlme_subHeader.length_subID_type        & 0xFF;
   pkt->payload[1] = (mlme_subHeader.length_subID_type >> 8) & 0xFF;

   len += 2;

   return len;
}

/**
@brief Prepends the incremental UPDATE of the schedule

@lkn{Samu} Storing of the schedule in the IE DISABLED. For the moment, it prepends NOTHING
@TODO Implementation of the incremental update of the schedule.
*/
port_INLINE uint8_t processIE_prependScheduleIE(
      OpenQueueEntry_t* pkt,
      uint8_t           type,
      uint8_t           frameID,
      uint8_t           flag,
      cellInfo_ht*      cellList
   ){
   uint8_t    i;
   uint8_t    len;
   uint8_t    temp8b;
   uint8_t    numOfCells;
   mlme_IE_ht mlme_subHeader;

   len        = 0;
   numOfCells = 0;

#if 0
   //===== cell list

   for(i=0;i<SCHEDULEIEMAXNUMCELLS;i++) {
      if(cellList[i].linkoptions != CELLTYPE_OFF){
         // cellobjects:
         // - [2B] slotOffset
         // - [2B] channelOffset
         // - [1B] link_type
         packetfunctions_reserveHeaderSize(pkt,5);
         pkt->payload[0] = (uint8_t)(cellList[i].tsNum  & 0x00FF);
         pkt->payload[1] = (uint8_t)((cellList[i].tsNum & 0xFF00)>>8);
         pkt->payload[2] = (uint8_t)(cellList[i].choffset  & 0x00FF);
         pkt->payload[3] = (uint8_t)((cellList[i].choffset & 0xFF00)>>8);
         pkt->payload[4] = cellList[i].linkoptions;
         len += 5;
         numOfCells++;
      }
   }

   // record the position of cellObjects
   pkt->l2_scheduleIE_numOfCells  = numOfCells;
   pkt->l2_scheduleIE_cellObjects = pkt->payload;

   //===== number of cells

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // prepare header
   temp8b  = numOfCells;
   temp8b |= flag << 7;

   // copy header
   *((uint8_t*)(pkt->payload)) = temp8b;

   len += 1;

   //===== slotframeID

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // prepare header
   temp8b = frameID;

   // copy header
   *((uint8_t*)(pkt->payload)) = temp8b;

   len += 1;

   // record the frameID
   pkt->l2_scheduleIE_frameID = frameID;

   //===== length

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // prepare header
   temp8b = len;

   // copy header
   *((uint8_t*)(pkt->payload)) = temp8b;

   // length
   len += 1;

   //===== type

   // reserve space
   packetfunctions_reserveHeaderSize(pkt,sizeof(uint8_t));

   // prepare header
   temp8b = type;

   // copy header
   *((uint8_t*)(pkt->payload)) = temp8b;

   len += 1;

   //===== MLME IE header

   // reserve space
   packetfunctions_reserveHeaderSize(pkt, sizeof(mlme_IE_ht));

   // prepare header
   mlme_subHeader.length_subID_type  = len;
   mlme_subHeader.length_subID_type |=
      (MLME_IE_SUBID_SCHEDULE << MLME_IE_SUBID_SHIFT) |
      IEEE802154E_DESC_TYPE_SHORT;

   // copy header
   pkt->payload[0] = mlme_subHeader.length_subID_type       & 0xFF;
   pkt->payload[1] = (mlme_subHeader.length_subID_type >> 8)& 0xFF;

   len+=2;
#endif
   return len;
}

//===== retrieve IEs

/**
@brief Retrieves the incremental update of the link scheduling from a EB packet

@lkn{Samu} Retrieving the schedule from the IE DISABLED. For the moment, it retrieves NOTHING
@TODO Retrieve the incremental update of the link scheduling from a EB packet

@param Packet where to extract the link scheduling from
@return New scheduling information as active slots
*/
port_INLINE void processIE_retrieveSlotframeLinkIE(
      OpenQueueEntry_t* pkt,
      uint8_t*          ptr
   ){
   uint8_t              numSlotFrames;
   uint8_t              i;
   uint8_t              j;
   uint8_t              localptr;
   slotframeLink_IE_ht  sfInfo;
   cellInfo_ht          linkInfo;
   open_addr_t          temp_neighbor;
   frameLength_t        oldFrameLength;
   bool my_shared;
   uint8_t neighAddr;

   localptr = *ptr;
   localptr+=0; /// @lkn{Samu} maybe need to be adjusted to the real value

#if 0
   // number of slot frames 1B
   numSlotFrames = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   schedule_setFrameNumber(numSlotFrames);

   // for each slotframe
   i=0;
   while(i < numSlotFrames){

      // [1B] slotftramehandle 1B
      sfInfo.slotframehandle =*((uint8_t*)(pkt->payload)+localptr);
      localptr++;

      schedule_setFrameHandle(sfInfo.slotframehandle);

      // [2B] slotframe size
      sfInfo.slotframesize   = *((uint8_t*)(pkt->payload)+localptr);
      localptr++;
      sfInfo.slotframesize  |= (*((uint8_t*)(pkt->payload)+localptr))<<8;
      localptr++;;

      oldFrameLength = schedule_getFrameLength();
      schedule_setFrameLength(sfInfo.slotframesize);

      // [1B] number of links
      sfInfo.numlinks        = *((uint8_t*)(pkt->payload)+localptr);
      localptr++;

      if (oldFrameLength == 0) {

         for (j=0;j<sfInfo.numlinks;j++){

            // [2B] TimeSlot
            linkInfo.tsNum = *((uint8_t*)(pkt->payload)+localptr);
            localptr++;
            linkInfo.tsNum  |= (*((uint8_t*)(pkt->payload)+localptr))<<8;
            localptr++;

            // [2B] Ch.Offset
            linkInfo.choffset = *((uint8_t*)(pkt->payload)+localptr);
            localptr++;
            linkInfo.choffset  |= (*((uint8_t*)(pkt->payload)+localptr))<<8;
            localptr++;

            /// @internal [LKN-retrieve-link-scheduling]
            // [1B] LinkOption bitmap
            linkInfo.linkoptions = *((uint8_t*)(pkt->payload)+localptr);
            my_shared=(linkInfo.linkoptions & ( 1 << 6 )) >> 6; //Retrieve the 6-th bit (SHARED)
            localptr++;

	    	// [3B] Neighbour value - LKN
            neighAddr = *((uint8_t*)(pkt->payload)+localptr);
            localptr++;

	    	temp_neighbor.type         = ADDR_64B;
    	    temp_neighbor.addr_64b[0]   = 0x14;
    	    temp_neighbor.addr_64b[1]   = 0x15;
    	    temp_neighbor.addr_64b[2]   = 0x92;
    	    temp_neighbor.addr_64b[3]   = 0xcc;
    		temp_neighbor.addr_64b[4]   = 0x00;
    		temp_neighbor.addr_64b[5]   = 0x00;
    		temp_neighbor.addr_64b[6]   = 0x00;
    		temp_neighbor.addr_64b[7]   = neighAddr;

            // shared TXRX anycast slot(s)
            schedule_addActiveSlot(
               linkInfo.tsNum,                     // slot offset
               CELLTYPE_TXRX,                      // type of slot
               my_shared,                          // default false, not implemented
               linkInfo.choffset,                  // channel offset
               &temp_neighbor                      // neighbor
            );
 	   		/// @internal [LKN-retrieve-link-scheduling]
         }
      }
      i++;
      break; //TODO: this break is put since a single slotframe is managed
   }
#endif
   *ptr=localptr;
}

port_INLINE void processIE_retrieveOpcodeIE(
      OpenQueueEntry_t* pkt,
      uint8_t*          ptr,
      opcode_IE_ht*     opcodeInfo
   ){
   uint8_t localptr;

   localptr=*ptr;

   opcodeInfo->opcode = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   *ptr=localptr;
}

port_INLINE void processIE_retrieveBandwidthIE(
      OpenQueueEntry_t* pkt,
      uint8_t *         ptr,
      bandwidth_IE_ht*  bandwidthInfo
   ){
   uint8_t localptr;

   localptr=*ptr;

   // [1B] slotframeID
   bandwidthInfo->slotframeID = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   // [1B] number of cells
   bandwidthInfo->numOfLinks = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   *ptr=localptr;
}

port_INLINE void processIE_retrieveScheduleIE(
      OpenQueueEntry_t* pkt,
      uint8_t*          ptr,
      schedule_IE_ht*   scheduleInfo
   ){
   uint8_t i;
   uint8_t temp8b;
   uint8_t localptr;

   localptr=*ptr;

   // [1B] type
   scheduleInfo->type             = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   // [1B] length
   scheduleInfo->length           = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   // [1B] frameID
   scheduleInfo->frameID          = *((uint8_t*)(pkt->payload)+localptr);
   localptr++;

   // [1B] number of cell and flag
   temp8b = *((uint8_t*)(pkt->payload)+localptr);
   scheduleInfo->numberOfcells    = temp8b & 0x7F;
   scheduleInfo->flag             = (temp8b >> 7) ? TRUE : FALSE;
   localptr++;

   if(scheduleInfo->numberOfcells > SCHEDULEIEMAXNUMCELLS) {
      //log error
      return;
   }

   for (i=0;i<scheduleInfo->numberOfcells;i++){

      // [2B] TimeSlot
      scheduleInfo->cellList[i].tsNum =
         *((uint8_t*)(pkt->payload)+localptr);
      localptr++;

      scheduleInfo->cellList[i].tsNum  |=
         (*((uint8_t*)(pkt->payload)+localptr))<<8;
      localptr++;

      // [2B] Ch.Offset
      scheduleInfo->cellList[i].choffset =
         *((uint8_t*)(pkt->payload)+localptr);
      localptr++;

      scheduleInfo->cellList[i].choffset  |=
         (*((uint8_t*)(pkt->payload)+localptr))<<8;
      localptr++;

      // [1B] LinkOption bitmap
      scheduleInfo->cellList[i].linkoptions =
         *((uint8_t*)(pkt->payload)+localptr);
      localptr++;
   }
   *ptr=localptr;
}

port_INLINE void processIE_retrieve_sixCelllist(
    OpenQueueEntry_t*   pkt,
    uint8_t             ptr,
    uint8_t             length,
    cellInfo_ht*        cellList
    ){
    uint8_t i=0;
    uint8_t localptr = ptr;
    uint8_t len = length;
    while(len>0){
        cellList[i].tsNum     = *((uint8_t*)(pkt->payload)+localptr);
        cellList[i].tsNum    |= (*((uint8_t*)(pkt->payload)+localptr+1))<<8;
        cellList[i].choffset  = *((uint8_t*)(pkt->payload)+localptr+2);
        cellList[i].choffset |= (*((uint8_t*)(pkt->payload)+localptr+3))<<8;
        localptr        += 4;
        len             -= 4;
        // mark with linkoptions as ocuppied
        cellList[i].linkoptions = CELLTYPE_TX;
        i++;
    }
}
