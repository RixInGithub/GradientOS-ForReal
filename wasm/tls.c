#include "tls.h"
#include "utils.h"

// gradienttls-only imports
__attribute__((import_module("env"), import_name("callSubtleCrypto"))) int subtle(c*,bool); // subtle's args: |s| = |crypto.subtle| (shortcut), |cKeys| = per gradientos session client keys
__attribute__((import_module("env"), import_name("subtleCryptoCallDone"))) bool subtleDone(int);
__attribute__((import_module("env"), import_name("subtleCryptoCall2Str"))) c*subtle2Str(c*); // uses |malloc|
__attribute__((import_module("env"), import_name("subtleCryptoCall2Buf"))) c*subtle2Buf(c*); // uses |malloc|

typedef struct {
	c*serverKey;
	bool sentHello;
	bool recvHello;
} TlsState;

typedef struct TlsSock {
	uint32_t idx;
	uint32_t rawSock;
	bool open;
	TlsState*state;
	struct TlsSock*next;
} TlsSock;

bool tlsReady = true;
TlsSock*socks = NULL;
struct {
	int h;
	bool done;
	c k[65];
} pubKey;

void tlsInit(void) {
	debug("obtaining pub key...");
	pubKey.done = false;
	pubKey.h = subtle("exportKey(\"raw\",cKeys.publicKey)",true);
}

void tlsHandleSocks(void) {
	c str[LEN1] = {0}; // oh boy
	c str2[LEN2] = {0}; // NO NO NO NO NO, WAIT WAIT WAIT WAIT WAIT
	if (!(pubKey.done)) { // state machine time!
		if (!(subtleDone(pubKey.h))) return; // i have no keys to hello with!!!
		concatStr(str,"h,h[");
		concatStr(str,i2s(pubKey.h,str2));
		concatStr(str,"]");
		c*buf = subtle2Buf(str);
		memset(str,0,LEN1);
		memset(str2,0,LEN2);
		// concatStr(str,"key addr: ");
		// concatStr(str,i2s((int)(pubKey.k),str2));
		// debug(str);
		// memset(str,0,LEN1);
		// memset(str2,0,LEN2);
		if (*buf==0) debug("HELP");
		memcpy(pubKey.k,buf,sizeof(pubKey.k));
		free(buf);
		pubKey.done = true;
		tlsReady = true;
		debug("obtained public key!");
	}
	if (socks==NULL) return; // don't bother
	TlsSock*key = socks;
	while (key!=NULL) {
		if ((key->open)&&(key->state!=NULL)) {
			TlsState*s = key->state;
			// ...
		}
		key = key->next;
	}
}

// set b4 call:
// |idx|, |next|
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
