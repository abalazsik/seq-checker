#ifndef _STRING_BUFFER_H_
#define _STRING_BUFFER_H_

#include<stdlib.h>
#include<string.h>

struct sbuffer {
	unsigned int capacity;
	unsigned int length;
	char* text;
};

typedef struct sbuffer* psbuffer;

psbuffer newStringBuffer(unsigned int initCapacity);
void expandStringBufferCapacity(psbuffer buffer);
void appendChar(psbuffer buffer, char ch);
void appendString(psbuffer buffer, char *text);
void resetStringBuffer(psbuffer buffer);
void freeStringBuffer(psbuffer buffer);
char* unwrap(psbuffer buffer);

#endif