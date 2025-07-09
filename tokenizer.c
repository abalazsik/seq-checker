/*
	Tokenizer, that tokenizes the tokenizables.
*/

#include <string.h>
#include "seq-checker.h"

ptoken next_token(char* script, ptoken prev, int shouldFreePrevToken) {
	unsigned int i = (prev == NULL) ? 0 : (prev->to);
	if (prev != NULL && shouldFreePrevToken) {
		free(prev);
	}

	size_t len = strlen(script);

	while(i < len && (script[i] == ' ' || script[i] == '\n' || script[i] == '\t')) {
		i++;
	}

	if (len == i) {
		return NULL;
	}

	ptoken result = (ptoken)malloc(sizeof(struct token));

	result->from = i;

	if (script[i] == '{' || script[i] == '}' || script[i] == ',' || script[i] == '<' || script[i] == ';') {
		result->to = i + 1;
		return result;
	} else {
		while(i < len && ((script[i] >= 'a' && script[i] <= 'z') || (script[i] >= 'A' && script[i] <= 'Z') || (script[i] >= '0' && script[i] <= '9'))) {
			i++;
		}
		result->to = i;
		return result;
	}

	return NULL;
}

char* substring(char* src, ptoken token) {
	size_t from = token->from;
	size_t to = token->to;

	if (from == to) {
		to++;
	}

	unsigned int len = to - from;
	char *result = (char *)malloc(sizeof(char) * len + 1);

	for (size_t i = 0; i < len; i++) {
		result[i] = src[i + from];
	}

	result[len] = 0;

	return result;
}

/*

// for manual testing

#include <stdio.h>
#define SIZE 8192

int main(int argc, char** argv) {
	
	if(argc < 2) {
		perror("Provide an input file!");
		return 1;
	}

	FILE *inputFile = fopen(argv[1], "r");

	if (!inputFile) {
		perror("Failed to open the file!");
		return 2;
	}

	char buffer[SIZE];
	
	size_t length = fread(buffer, sizeof(char), SIZE, inputFile);
	
	printf("tokens:\n");

	ptoken curr_token = NULL;

	while((curr_token = next_token(buffer, curr_token, 1)) > 0) {
		int len = curr_token->to - curr_token->from;

		printf("%d: %s\n", len, substring(buffer, curr_token));
	}

	return 0;
}
*/