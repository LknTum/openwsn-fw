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
#define UINJECT_PERIOD_MS 700

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
