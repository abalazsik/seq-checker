/*
	Tokenizer, that tokenizes the tokenizables.
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct token {
	size_t from, to;
};

typedef struct token *ptoken;

ptoken next_token(char* script, ptoken prev) {
	unsigned int i = (prev == NULL) ? 0 : (prev->to + 1);
	size_t len = strlen(script);

	while(i < len && (script[i] == ' ' || script[i] == '\n' || script[i] == '\t')) {
		i++;
	}

	if (len == i) {
		return NULL;
	}

	ptoken result = (ptoken)malloc(sizeof(struct token));

	result->from = i;

	if (script[i] == '{' || script[i] == '}' || script[i] == ',' || script[i] == '<' || script[i] == '>') {
		result->to = i + 1;
		return result;
	} else {
		while(i < len && ((script[i] >= 'a' && script[i] <= 'z') || (script[i] >= 'A' && script[i] <= 'Z'))) {
			i++;
		}
		result->to = i;
		return result;
	}

	return (ptoken)(-1);
}

/*

// for manual testing

#include <stdio.h>
#define SIZE 8192

char* copy(char* src, size_t from, size_t to) {
	char *result = (char *)malloc(sizeof(char) * (to + 1 - from));

	for (size_t i = from; i < to; i++) {
		result[i - from] = src[i];
	}

	result[(to - from) + 1] = 0;

	return result;
}

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

	while((curr_token = next_token(buffer, curr_token)) > 0) {
		int siz = curr_token->to - curr_token->from;
		//char dst[siz];
		//strncpy(dst, buffer + curr_token->from, siz );

		printf("%d: %s\n", siz, copy(buffer, curr_token->from, curr_token->to));
	}

	return 0;
}
*/