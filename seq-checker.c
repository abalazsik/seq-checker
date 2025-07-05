#include "seq-checker.h"

struct solution {
	char* text;
	int isError;
};

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

	unsigned int size = 0; 

	for (unsigned int i = 0; i < stack->sp; i++) {
		size += strlen(stack->symbols[i]->name);
	}
	
	if (stack->sp > 1) {
		size = size + stack->sp;
	}

	char* result = (char*)malloc(sizeof(char) * (size + 1));

	unsigned int ptr = 0;

	for (unsigned int i = 0; i < stack->sp; i++) {
		char* name = stack->symbols[i]->name;
		strcpy((char*)(result + ptr), name);
		ptr = ptr + strlen(name);

		if (i < stack->sp - 1) {
			result[ptr] = '|';
		}
		ptr++;
	}
	//result[size] = 0;

	return result;
}

extern struct solution* getSolution(char* script) {
	struct solution* solution = (struct solution*)malloc(sizeof(struct solution));
	solution->isError = 1; 

	struct sequenceDef* seqDef = parse_sequence(script);

	if (seqDef == NULL) {
		solution->isError = 1;
		solution->text = "syntax error";
	} else {
		struct symbol_stack* actualSolution = solve(seqDef);

		if (isStackEmpty(actualSolution)) {
			solution->isError = 1;
			solution->text = "no solution";
		} else {
			// format 
			solution->text = stackToString(actualSolution);
		}

		freeSequenceDef(seqDef);
	}

	return solution;
}

// Helper functions we can use from js to extract the result

extern int isError(struct solution* solution) {
	return solution->isError;
}

extern char* getSolutionText(struct solution* solution) {
	return solution->text;
}

/*
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