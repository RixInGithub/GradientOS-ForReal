#ifndef __TLS
#define __TLS
#include "utils.h"
extern bool tlsReady;
void tlsInit(void);
void tlsHandleSocks(void);
uint32_t tlsOpen(c*host, uint16_t p);
#endif
