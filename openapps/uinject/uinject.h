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
#define UINJECT_PERIOD_MS 1000
#define BURST_SILENCE_MS 10000
#define BURST_DURATION_MS 10000
#define BURST_PERIOD_MS 400	///< interval for sending burst packets
// Too low values such as 100 creates problems
///@lkn{LKN-Murat} Application type 1-Burst 2-Non-Periodic 3-Periodic
///@internal [LKN-uinject-settings]
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

/**
\}
\}
*/

#endif
