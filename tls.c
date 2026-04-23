#include "tls.h"
#include "utils.h"

typedef struct TlsSock {
	uint32_t idx;
	uint32_t rawSock;
	uintptr_t ssl;
	struct TlsSock*next;
} TlsSock;

bool tlsReady = false;
TlsSock*socks = NULL;

void tlsInit(void) {
}

void tlsHandleSocks(void) {
	return; // wip
}

/*
uint32_t tlsRawOpen(c*host, uint16_t p, TlsSock*alloc) {
	alloc->rawSock = netOpen(host,p);
	alloc->open = true;
	alloc->state = malloc(sizeof(TlsState));
	alloc->state->serverKey = NULL;
	alloc->state->sentHello = false;
	return alloc->idx;
}

uint32_t tlsOpen(c*host, uint16_t p) {
	TlsSock*prev = NULL;
	TlsSock*key = socks;
	while (key!=NULL) {
		if (!(key->open)) { // closed key => closed tcp socket => free for realloc
			if (key->state != NULL) free(key->state);
			key->state = NULL;
			return tlsRawOpen(host,p,key);
		}
		prev = key;
		key = key->next;
	}
	key = malloc(sizeof(TlsSock)); // oh BOY i LOVE memory management!!!!
	key->next = NULL;
	if (prev == NULL) { // in other words, |socks == NULL|
		socks=key;
		key->idx=0;
		return tlsRawOpen(host,p,key);
	}
	prev->next = key;
	key->idx = (prev->idx)+1;
	return tlsRawOpen(host,p,key);
}
*/
