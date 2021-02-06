#ifndef __PHASOR_MAIN_H__
#define __PHASOR_MAIN_H__

#include <stdint.h>
#include <sapi.h>

#include "utils.h"
#include "comunication.h"
#include "digitalization.h"

#define MAX_CYCLE_BUFFER_SIZE 100

void onTickUpdate(void* UNUSED);

#endif /* __PHASOR_MAIN_H__ */
