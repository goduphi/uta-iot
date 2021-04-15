#ifndef HIBERNATION_H_
#define HIBERNATION_H_


#include <stdint.h>
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void waitUntilWriteAllowed();
void initHibernationModule();
void hibernate(uint32_t sleep_time_in_seconds);

#endif
