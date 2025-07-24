#include <string.h>
#include <stddef.h>
#include "seq-checker.h"

int symbolEquals(psymbol a, psymbol b) {
	if (a == b) {
		return 1;
	}

	if (a == NULL || b == NULL) {
		return 0;
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

int symbolDefsContainsSymbol(struct sequenceDef* seqDef, psymbol symbol) {
	for (size_t i = 0; i < seqDef->symbols->noSymbols; i++) {
		if (symbolEquals(seqDef->symbols->symbols[i], symbol)) {
			return 1;
		}
	}
	return 0;
}

char* getValidationTextByValidationCode(enum validationCode errorCode) {
	switch (errorCode) {
		case OK: return "Ok";
		case UNIQUE_SYMBOLS: return "symbols should be unique";
		case COUNT_QUERY_LIMIT: return "a count query should not be limited";
		case STARTING_NOT_IN_SYMBOLS_DEFS: return "symbol defs does not contain starting symbol";
		case ENDING_NOT_IN_SYMBOLS_DEFS: return "symbol defs does not contain ending symbol";
		case STARTING_EQ_ENDING: return "starting and ending cannot be the same";
		case RULE_BEFOER_EQ_AFTER: return "before, and after symbolshould not be the same in a rule definition";
		case BEFORE_NOT_IN_SYMBOLS_DEFS: return "symbol defs does not contain before symbol";
		case AFTER_NOT_IN_SYMBOLS_DEFS: return "symbol defs does not contain after symbol";
	}
	return NULL;
}

enum validationCode validateSequenceDef(struct sequenceDef* seqDef) {
	for (size_t i = 0; i < seqDef->symbols->noSymbols - 1; i++) {// unique symbols check
		for (size_t j = i + 1; j < seqDef->symbols->noSymbols; j++) {
			if (symbolEquals(seqDef->symbols->symbols[i],seqDef->symbols->symbols[j])) {
				return UNIQUE_SYMBOLS;
			}
		}	
	}

	if (seqDef->isCountQuery == 1 && seqDef->limit > 0) { // a count query should not be limited
		return COUNT_QUERY_LIMIT;
	}

	psymbol starting = seqDef->rules->starting;
	psymbol ending = seqDef->rules->ending;

	if (starting != NULL) {
		if (!symbolDefsContainsSymbol(seqDef, starting)) { 
			return STARTING_NOT_IN_SYMBOLS_DEFS; // symbol defs does not contain starting symbol
		}
	}

	if (ending != NULL) {
		if (!symbolDefsContainsSymbol(seqDef, ending)) { 
			return ENDING_NOT_IN_SYMBOLS_DEFS; // symbol defs does not contain ending symbol
		}
	}

	if (starting != NULL && ending != NULL) { 
		if (symbolEquals(starting, ending)) {
			return STARTING_EQ_ENDING; // starting and ending cannot be the same
		}
	}

	for (size_t i = 0; i < seqDef->rules->noRules; i++) {
		prule rule = seqDef->rules->rules[i];
		if (symbolEquals(rule->before, rule->after)) { 
			return RULE_BEFOER_EQ_AFTER; // rule.before != rule.after
		}
		if (!symbolDefsContainsSymbol(seqDef, rule->before)) {
			return BEFORE_NOT_IN_SYMBOLS_DEFS; // symbols contains before symbol
		}
		if (!symbolDefsContainsSymbol(seqDef, rule->after)) {
			return AFTER_NOT_IN_SYMBOLS_DEFS; // symbols contains after symbol
		}
	}
	
	return OK;
}

/*
// for manual testing
// gcc seq-checker.h tokenizer.c parser.c validate.c -o validator

#include <stdio.h>
#include <stdlib.h>

void printSeqDef(struct sequenceDef* seqDef) {
	if (seqDef == NULL) {
		printf("syntax error\n");
		return;
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
	
	if (seqDef->rules->starting != NULL) {
		printf("starting: %s\n", seqDef->rules->starting->name);
		if (seqDef->rules->notStarting) {
			printf("\tstarting is negated!\n");
		}
	}
	if (seqDef->rules->ending != NULL) {
		printf("ending: %s\n", seqDef->rules->ending->name);
		if (seqDef->rules->notEnding) {
			printf("\tending is negated!\n");
		}
	}
	printf("limit: %d\n",seqDef->limit);

	if (validateSequenceDef(seqDef)) {
		printf("sequence definition is valid\n");
	} else {
		printf("semantic error!\n");
	}

}

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

	struct sequenceDef* seqDef = parseSequence(buffer);

	printSeqDef(seqDef);
	
	return 0;
}
*/