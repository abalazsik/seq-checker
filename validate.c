#include <string.h>
#include <stddef.h>
#include "seq-checker.h"

int symbol_equals(psymbol a, psymbol b) {
	if (a == b) {
		return 1;
	}

	size_t len = strlen(a->name);

	if (len != strlen(b->name)) {
		return 0;
	}

	for (size_t i = 0; i < len; i++) {
		if (a->name[i] != b->name[i]) {
			return 0;
		}
	}

	return 1;
}

int symbol_defs_contains_symbol(struct sequenceDef* seqDef, psymbol symbol) {
	for (size_t i = 0; i < seqDef->symbols->noSymbols; i++) {
		if (symbol_equals(seqDef->symbols->symbols[i], symbol)) {
			return 1;
		}
	}
	return 0;
}

int validate_sequence_def(struct sequenceDef* seqDef) {
	for (size_t i = 0; i < seqDef->symbols->noSymbols - 1; i++) {// unique symbols check
		for (size_t j = i + 1; j < seqDef->symbols->noSymbols; j++) {
			if (symbol_equals(seqDef->symbols->symbols[i],seqDef->symbols->symbols[j])) {
				return 0;
			}
		}	
	}

	psymbol starting = seqDef->rules->starting;
	psymbol ending = seqDef->rules->ending;

	if (starting != NULL) {
		if (!symbol_defs_contains_symbol(seqDef, starting)) { 
			return 0; // symbol defs does not contains starting symbol
		}
	}

	if (ending != NULL) {
		if (!symbol_defs_contains_symbol(seqDef, ending)) { 
			return 0; // symbol defs does not contains ending symbol
		}
	}

	if (starting != NULL && ending != NULL) { 
		if (symbol_equals(starting, ending)) {
			return 0; // starting and ending cannot be the same
		}
	}

	for (size_t i = 0; i < seqDef->rules->noRules; i++) {
		prule rule = seqDef->rules->rules[i];
		if (symbol_equals(rule->before, rule->after)) { 
			return 0; // rule.before != rule.after
		}
		if (!symbol_defs_contains_symbol(seqDef, rule->before)) {
			return 0; // symbols contains before symbol
		}
		if (!symbol_defs_contains_symbol(seqDef, rule->after)) {
			return 0; // symbols contains after symbol
		}
	}

	return 1;
}

// for manual testing
// gcc seq-checker.h tokenizer.c parser.c rmc.cpp validate.c -o validator

#include <stdio.h>
#include <stdlib.h>

#define SIZE 8192

int main (int argc, char** argv) {
	
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

	fclose(inputFile);

	struct sequenceDef* seqDef = parse_sequence(buffer);

	if (seqDef == NULL) {
		printf("syntax error\n");
	}

	printf("symbols (%d):\n", seqDef->symbols->noSymbols);
	
	for (int i = 0; i < seqDef->symbols->noSymbols; i++) {
		printf("\t %s\n", seqDef->symbols->symbols[i]->name);
	}
	
	printf("rules (%d):\n", seqDef->rules->noRules);
	
	for (int i = 0; i < seqDef->rules->noRules; i++) {
		struct rule* rule = seqDef->rules->rules[i];

		printf("\t %s < %s\n", rule->before->name, rule->after->name);
	}
	
	printf("starting: %s\n", seqDef->rules->starting->name);
	printf("ending: %s\n", seqDef->rules->ending->name);
	if (validate_sequence_def(seqDef)) {
		printf("sequence definition is valid\n");
	} else {
		printf("semantic error!\n");
	}
	
	return 0;
}