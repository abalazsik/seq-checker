
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

	for (unsigned int i = 0; i < newCapacity; i++) {
		newText[i] = i < buffer->capacity ? buffer->text[i] : 0;
	}

	free(buffer->text);
	buffer->text = newText;
	buffer->capacity = newCapacity;
}

void appendChar(psbuffer buffer, char ch) {
	while (buffer->length + 1 >= buffer->capacity) {
		expandStringBufferCapacity(buffer);
	}

	buffer->text[buffer->length] = ch;
	buffer->length++;
}

void appendString(psbuffer buffer, char *text) {
	size_t len = strlen(text);

	while (buffer->length + len >= buffer->capacity) {
		expandStringBufferCapacity(buffer);
	}

	for (unsigned int i = 0; i < len; i++) {
		buffer->text[buffer->length + i] = text[i];
	}

	buffer->length += len;
}

// probably the least effective int -> string algorithm, but its ok for now
void appendUint(psbuffer buffer, unsigned int value) {
	if (value > 0) {
		char ch = (value % 10) + '0';

		appendUint(buffer, value / 10);
		appendChar(buffer, ch);
	}
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

