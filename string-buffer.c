
#include "string-buffer.h"

psbuffer newStringBuffer(unsigned int initCapacity) {
	psbuffer result = (psbuffer)malloc(sizeof(struct sbuffer));

	char* text = (char*)malloc(sizeof(char) * initCapacity);

	result->text = text;
	result->capacity = initCapacity;
	result->length = 0;

	for (unsigned i = 0; i < initCapacity; i++) {
		text[i] = 0;
	}

	return result;
}

void expandStringBufferCapacity(psbuffer buffer) {
	unsigned int newCapacity = buffer->capacity * 2;
	char* newText = (char*)malloc(sizeof(char) * newCapacity);

	memcpy(newText, buffer->text, sizeof(char) * buffer->capacity); 

	for (unsigned int i = buffer->capacity; i < newCapacity; i++) {
		newText[i] = 0;
	}

	free(buffer->text);
	buffer->text = newText;
	buffer->capacity = newCapacity;
}

void appendChar(psbuffer buffer, char ch) {
	if (buffer->length + 1 >= buffer->capacity) {
		expandStringBufferCapacity(buffer);
	}

	buffer->text[buffer->length++] = ch;
}

void appendString(psbuffer buffer, char *text) {
	size_t len = strlen(text);

	if (buffer->length + len >= buffer->capacity) {
		expandStringBufferCapacity(buffer);
	}

	strcpy((char*)(buffer->text + buffer->length), text);
	buffer->length += len;
}

void resetStringBuffer(psbuffer buffer) {
	char* text = buffer->text;

	for (unsigned i = 0; i < buffer->length; i++) {
		text[i] = 0;
	}

	buffer->length = 0;
}

void freeStringBuffer(psbuffer buffer) {
	free(buffer->text);
	free(buffer);
}

char* unwrap(psbuffer buffer) {
	char* text = buffer->text;
	free(buffer);
	return text;
}

