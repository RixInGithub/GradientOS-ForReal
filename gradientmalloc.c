/*
	once upon a time, a FUNCTION was whispered into the GRAD MAN.
	it was a FUNCTION of ALLOCATION. it was a FUNCTION of RESOURCES CREATION.
	it was a FUNCTION of INNOVATION. it was a FUNCTION of GREATLY MANAGEMENT.
	this is the legend of GRADIENTMALLOC.

	for millenia, there was UNDEFINED BEHAVIOR and TODOS instead of MALLOC,
	harming all who dare use.
	with this, the sky would run black with terror
	and the coding land will crack with fear.
	then, her heart pounding...
	the C-EARTH will draw her final breath.

	only then, shining with hope...
	three VARIABLES appear at WORLDS' edge.
	a BYTE INDEX, an INDEX OFFSET, and an ARGUMENT FROM THE CALLER.

	only they can erase corruption
	to banish UNDEFINED BEHAVIOR and TODOS.
	only then will balance be restored,
	and the WORLD saved from uncertainty.

	today, the FOUNTAIN OF REUSABLE CODE-
	the geyser that gives this coding land form-
	stands tall at the center of the kingdom.

	but recently, another fountain has appeared on the horizon...

	and with it, the balance of INNOVATION and UNDEFINED BEHAVIOR begins to shift...
*/

#include <stddef.h>
#include <stdbool.h>
#include "gradientmalloc.h"
#include "utils.h"
#define GMHEAPSIZE 65535
#define ISINVALIDPTR(p) ((!(p))||(((unsigned char*)(p)<heap)||((unsigned char*)(p)>heap+GMHEAPSIZE)))

unsigned char heap[GMHEAPSIZE] = {0};

void*malloc(size_t n) {
	size_t byteIdx=0;
	size_t idxOff=0;
	if(n==0)return(NULL);
	n+=sizeof(size_t); // add room for header...
	// too lazy to add real blocks; i'll just do whatever and hope it works
	while (idxOff<n) {
		if(byteIdx+idxOff>=GMHEAPSIZE)return(NULL);
		if (heap[byteIdx+idxOff] != 0) {
			byteIdx+=idxOff;
			idxOff=0;
			// slightly safer...
			size_t len;
			memcpy(&len,heap+byteIdx,sizeof(size_t));
			byteIdx+=len+sizeof(size_t);
			continue;
		}
		idxOff++;
	}
	// to reference the size_t we use heap+byteIdx since idxOff is just a temporary counter to make sure that n bytes ahead is all zeroes.
	// to reference the data we add sizeof(size_t) since it comes after <amount of bytes a size_t takes up>.
	n-=sizeof(size_t);
	memcpy(heap+byteIdx, &n, sizeof(size_t));
	return(heap+byteIdx+sizeof(size_t));
}

void free(void*ptr) {
	if(ISINVALIDPTR(ptr))return; // i wish i could throw an error here...
	size_t safeLen;
	size_t len = *((size_t*)ptr-1);
	memcpy(&safeLen, &len, sizeof(size_t));
	memset((unsigned char*)ptr-sizeof(size_t), 0, safeLen+sizeof(size_t));
}

void*calloc(size_t a, size_t b) {
	void*out=malloc(a*b);
	if(out==NULL)return(out);
	memset(out, 0, a*b); // just to stay compliant...
	return(out);
}

void*realloc(void*ptr, size_t n) {
	if(ptr==NULL)return(malloc(n));
	if(ISINVALIDPTR(ptr))return(NULL);
	if (n==0) {
		free(ptr);
		return NULL;
	}
	size_t len = *((size_t*)ptr-1);
	void*moveTo=malloc(n);
	if(!(moveTo))return(NULL);
	memcpy(moveTo, ptr, (n>len)?(len):(n));
	free(ptr);
	return(moveTo);
}
