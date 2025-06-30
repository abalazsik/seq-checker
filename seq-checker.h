#ifndef _SEQ_CHECKER_H_
#define _SEQ_CHECKER_H_

#include <stddef.h>

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

#endif