#include "seq-checker.h"
#include "string-buffer.h"

struct symbolStack* createSymbolStack(unsigned int capacity) {
	struct symbolStack* stack = (struct symbolStack*)malloc(sizeof(struct symbolStack));

	stack->symbols = (psymbol*)malloc(sizeof(psymbol) * capacity);
	stack->capacity = capacity;
	stack->sp = 0;

	return stack;
}

int pushOntoSymbolStack(struct symbolStack* stack, psymbol symbol) {
	if (stack->sp < stack->capacity) {
		stack->symbols[stack->sp] = symbol;
		stack->sp++;
		return 1;
	} else {
		return 0;
	}
}

int isStackEmpty(struct symbolStack* stack) {
	return stack->sp == 0;
}

psymbol popFromStack(struct symbolStack* stack) {
	if (isStackEmpty(stack)) {
		return NULL;
	}

	return stack->symbols[--stack->sp];
}

void freeStack(struct symbolStack* stack) {
	free(stack->symbols);
	free(stack);
}

int searchSymbolInStack(struct symbolStack* stack, psymbol symbol) {
	for (int i = 0; i < stack->sp; i++) {
		if (symbolEquals(stack->symbols[i], symbol)) {
			return i;
		}
	}

	return -1;
}

int checkSymbols(psymbol a, psymbol b, int negation) {
	int equals = symbolEquals(a, b);

	/*
		equals	|negation	|returns
		0		|0			|0
		0		|1			|1
		1		|0			|1
		1		|1			|0
	*/


	return (equals && !negation) || (!equals && negation); 
}


int isSolutionInt(struct symbolStack* stack, struct sequenceDef* sequenceDef) {
	struct rulesDef* rules = sequenceDef->rules;

	if(rules->starting != NULL) {
		if (!checkSymbols(stack->symbols[0], rules->starting, rules->notStarting)) {
			//printf("starting does not match\n");
			return 0;
		}
	}

	if(rules->ending != NULL) {
		if (!checkSymbols(stack->symbols[stack->sp - 1], rules->ending, rules->notEnding)) {
			//printf("ending does not match\n");
			return 0;
		}
	}

	for (int i = 0; i < rules->noRules; i++) {
		prule rule = rules->rules[i];

		int beforeIndex = searchSymbolInStack(stack, rule->before);
		int afterIndex = searchSymbolInStack(stack, rule->after);

		if (beforeIndex == -1 || afterIndex == -1 || (beforeIndex > afterIndex)) {
			//printf("general rule does not match %s (%d) < %s (%d)\n", rule->before->name, beforeIndex, rule->after->name, afterIndex);
			return 0;
		}
	}

	return 1;
}

int isSolution(struct symbolStack* stack, struct sequenceDef* sequenceDef) {
	int result = isSolutionInt(stack, sequenceDef);

	//printf("checking... %s -> %d\n", stackToString(stack), result); // uncomment this, to see it in action (in the browser console)

	return result;
}

//============

struct symbolsDef* restOfSymbols(struct symbolsDef *original, psymbol minus) {
	if (original->noSymbols <= 1) {
		return NULL;
	}

	struct symbolsDef *result = (struct symbolsDef*)malloc(sizeof(struct symbolsDef));

	result->noSymbols = original->noSymbols - 1;
	result->symbols = (psymbol*)malloc(sizeof(psymbol) * result->noSymbols);

	size_t dest = 0;

	for (size_t i = 0; i < original->noSymbols; i++) {
		if (!symbolEquals(original->symbols[i], minus)) {
			result->symbols[dest] = original->symbols[i];
			dest++;
		}
	}

	return result;
}

void freeSymbolsDef(struct symbolsDef* symbolsDef)  {
	free(symbolsDef->symbols);
	free(symbolsDef);
}

void solveSeq(
	struct symbolStack* stack,
	struct sequenceDef* sequenceDef,
	struct symbolsDef* symbolsToOrder,
	unsigned int *foundSolutions,
	psbuffer buffer) {

	if (*foundSolutions >= sequenceDef->limit) {
		return;
	}

	if (symbolsToOrder == NULL) {
		if(isSolution(stack, sequenceDef)) {
			if (!sequenceDef->isCountQuery) {
				char* stackAString = stackToString(stack);
				appendString(buffer, stackAString);
				appendString(buffer, "\n");
				free(stackAString);
			}
			*foundSolutions = *foundSolutions + 1;
		}
		return;
	}

	unsigned int i = 0;

	// TODO: here we could validate the semi-solution, to speed up the search
	do {
		psymbol current = symbolsToOrder->symbols[i];

		pushOntoSymbolStack(stack, current);

		struct symbolsDef* rest = restOfSymbols(symbolsToOrder, current);

		solveSeq(stack, sequenceDef, rest, foundSolutions, buffer);

		if (rest != NULL) {
			freeSymbolsDef(rest);
		}

		if (*foundSolutions >= sequenceDef->limit) {
			return;
		} else {
			popFromStack(stack);
			i++;
		}

	} while(i < symbolsToOrder->noSymbols);
}

unsigned int maxSolutions(unsigned int noSymbols) {
	unsigned int n = 1;

	for (unsigned int i = 1; i <= noSymbols; i++) {
		n *= i;
	}

	return n;
}

void solve(struct sequenceDef* sequenceDef, presult result) {
	unsigned int noSymbols = sequenceDef->symbols->noSymbols;
	struct symbolStack* stack = createSymbolStack(noSymbols);

	unsigned int limit = maxSolutions(noSymbols);

	if (!(sequenceDef->limit > 0 && limit > sequenceDef->limit && !sequenceDef->isCountQuery)) {
		sequenceDef->limit = limit;
	}

	unsigned int foundSolutions = 0;

	psbuffer buffer = newStringBuffer(10);

	solveSeq(stack, sequenceDef, sequenceDef->symbols, &foundSolutions, buffer);
	freeStack(stack);

	if(sequenceDef->isCountQuery) {
		appendUint(buffer, foundSolutions);
	} else if (foundSolutions == 0) {
		appendString(buffer, "no solutions");
		result->isError = 1;
	}

	result->text = unwrap(buffer);
}