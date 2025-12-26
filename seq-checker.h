#ifndef _SEQ_CHECKER_H_
#define _SEQ_CHECKER_H_

#define _SEQ_CHECKER_EXECUTABLE_VERSION_ 6

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct token {
	size_t from, to;
};

typedef struct token *ptoken;

ptoken nextToken(char* script, ptoken prev, int shouldFreePrevToken);
char* substring(char* src, ptoken token);

struct symbol {
	char* name;
};

typedef struct symbol *psymbol;

int symbolEquals(psymbol a, psymbol b);

struct rule{
	psymbol before;
	psymbol after;
};

typedef struct rule *prule;

struct symbolsDef {
	unsigned int noSymbols;
	psymbol* symbols;
};

struct rulesDef {
	psymbol starting;
	int notStarting;
	psymbol ending;
	int notEnding;
	unsigned int noRules;
	struct rule** rules;
};

struct sequenceDef {
	struct symbolsDef* symbols;
	struct rulesDef* rules;
	unsigned int limit;
	char isCountQuery;
};

struct sequenceDef* parseSequence(char* script);

struct symbolStack {
	unsigned int capacity;
	unsigned int sp;
	psymbol *symbols;
};

struct result {
	char* text;
	int isError;
};

typedef struct result *presult;

enum validationCode {
	OK,
	UNIQUE_SYMBOLS,
	COUNT_QUERY_LIMIT,
	STARTING_NOT_IN_SYMBOLS_DEFS,
	ENDING_NOT_IN_SYMBOLS_DEFS,
	STARTING_EQ_ENDING,
	RULE_BEFOER_EQ_AFTER,
	BEFORE_NOT_IN_SYMBOLS_DEFS,
	AFTER_NOT_IN_SYMBOLS_DEFS
};

char* getValidationTextByValidationCode(enum validationCode errorCode);
enum validationCode validateSequenceDef(struct sequenceDef* seqDef);
int isStackEmpty(struct symbolStack* stack);
void solve(struct sequenceDef* sequenceDef, presult result);
char* stackToString(struct symbolStack* stack);
void freeStack(struct symbolStack* stack);

#endif