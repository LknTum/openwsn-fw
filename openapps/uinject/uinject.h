#ifndef __UINJECT_H
#define __UINJECT_H

/**
\addtogroup AppUdp
\{
\addtogroup uinject
\{
*/

#include "opentimers.h"



//=========================== define ==========================================
///@lkn{LKN-Samu} Application periodicity
///@internal [LKN-uinject-settings]
#define UINJECT_PERIOD_MS 1000  // periodicity for periodic application
	// variance and mean for the non-periodic application
#define BURST_SILENCE_MS 180000  // The duration of silence before a burst occurs no packet is sent under silence mode 
#define BURST_DURATION_MS 120000 // The duration of the burst mode
	//In total a cycle for bursty application is BURST_SILENCE_MS + BURST_DURATION_MS
#define BURST_PERIOD_MS 400	///< packet sending period in burst mode
// Too low values such as 100 creates problems
///@lkn{LKN-Murat} Application type 1-Burst 2-Non-Periodic 3-Periodic 4-No Application
///@internal [LKN-uinject-settings]
#define APPFLAG 4
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

/**
\}
\}
*/

#endif
