#ifndef _SEQ_CHECKER_H_
#define _SEQ_CHECKER_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void remove_comments(char* buffer);

struct token {
	size_t from, to;
};

typedef struct token *ptoken;

ptoken next_token(char* script, ptoken prev, int shouldFreePrevToken);
char* substring(char* src, ptoken token);

struct symbol {
	char* name;
};

typedef struct symbol *psymbol;

int symbol_equals(psymbol a, psymbol b);

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
	psymbol ending;
	unsigned int noRules;
	struct rule** rules;
};

struct sequenceDef {
	struct symbolsDef* symbols;
	struct rulesDef* rules;
};

struct sequenceDef* parse_sequence(char* script);

struct symbol_stack {
	unsigned int capacity;
	unsigned int sp;
	psymbol *symbols;
};

int isStackEmpty(struct symbol_stack* stack);
struct symbol_stack* solve(struct sequenceDef* sequenceDef);
char* stackToString(struct symbol_stack* stack);

#endif