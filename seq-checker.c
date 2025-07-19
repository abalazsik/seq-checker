#include "seq-checker.h"
#include "string-buffer.h"

void freeSymbol(psymbol symbol) {
	free(symbol->name);
	free(symbol);
}

void freeSequenceDef(struct sequenceDef* seqDef) {
	for (int i = 0; i < seqDef->rules->noRules; i++) {
		free(seqDef->rules->rules[i]);
	}
	for (int i = 0; i < seqDef->symbols->noSymbols; i++) {
		freeSymbol(seqDef->symbols->symbols[i]);
	}
	free(seqDef->symbols);
	free(seqDef->rules);
	free(seqDef);
}

char* stackToString(struct symbol_stack* stack) {
	psbuffer buffer = newStringBuffer(20);

	for (unsigned int i = 0; i < stack->sp; i++) {
		appendString(buffer, stack->symbols[i]->name);

		if (i < stack->sp - 1) {
			appendChar(buffer, '|');
		}
	}

	return unwrap(buffer);
}

extern psolution getSolution(char* script) {
	psolution solution = (psolution)malloc(sizeof(struct solution));
	solution->isError = 0;

	struct sequenceDef* seqDef = parse_sequence(script);
	if (seqDef == NULL) {
		solution->isError = 1;
		solution->text = "syntax error";
	} else {
		solve(seqDef, solution);
		freeSequenceDef(seqDef);
	}

	//free(script);

	return solution;
}

// Helper functions we can use from js to extract the result

extern int isError(psolution solution) {
	return solution->isError;
}

extern char* getSolutionText(psolution solution) {
	return solution->text;
}

extern int getVersion() {
	return _SEQ_CHECKER_EXECUTABLE_VERSION_;
}

/*
//gcc seq-checker.h tokenizer.c parser.c rmc.c validate.c solver.c seq-checker.c string-buffer.c -o seq-checker

#include <stdio.h>
#include <stdlib.h>

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
	
	fclose(inputFile);

	struct solution* solution = getSolution(buffer);

	printf("%s\n", getSolutionText(solution));
	
	return 0;
}
*/