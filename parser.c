#include "seq-checker.h"

int isToken(char* script, ptoken at, char* expected) {
	size_t len = at->to - at->from;

	if (len != strlen(expected)) {
		return 0;
	}

	size_t i = 0;
	while(i < len && script[at->from + i] == expected[i]) {
		i++;
	}

	return i == len;
}

int isSymbol(char* script, ptoken at) {
	size_t len = at->to - at->from;

	for (size_t i = 0; i < len; i++) {
		char ch = script[at->from + i];
		if ((ch < 'a' && ch > 'z') || (ch < 'A' && ch > 'Z')) {
			return 0;
		}
	}

	return 1;
}

psymbol new_symbol(char* script, ptoken curr_token) {
	psymbol sym = (psymbol)malloc(sizeof(struct symbol));
	sym->name = substring(script, curr_token);
	return sym;
}

prule new_rule(char* script, ptoken before_token, ptoken after_token) {
	prule rule = (prule)malloc(sizeof(struct rule));
	rule->before = new_symbol(script, before_token);
	rule->after = new_symbol(script, after_token);
	return rule;
}

/*
The  format should be:
	generate sequence of {A, B, C} where { A < B } starting A ending B; 
*/

#define T_START		1
#define T_GENERATE	2
#define T_SEQUENCE	3
#define T_OF	4
#define T_S_LEFT_BRACKET	5
#define	T_S_SYMBOL	6
#define T_S_COMMA	7
#define T_S_RIGHT_BRACKET	8
#define T_WHERE	9
#define T_W_LEFT_BRACKET	10
#define T_W_BEFORE_SYMBOL	11
#define T_W_GT	12
#define T_W_AFTER_SYMBOL	13
#define T_W_COMMA	14
#define T_W_RIGHT_BRACKET	15
#define T_STARTING	16
#define T_STARTING_SYMBOL	17
#define T_ENDING	18
#define T_ENDING_SYMBOL 19
#define T_SEMICOLON	20
#include <stdio.h>
struct sequenceDef* parse_sequence(char* script) {

	remove_comments(script);

	ptoken curr_token = NULL;
	
	int state = T_START;

	ptoken symbols_start_token = NULL;
	ptoken rules_start_token = NULL;
	ptoken starting_token = NULL;
	ptoken ending_token = NULL;

	unsigned int noSymbols = 0;
	unsigned int noRules = 0;

	int shouldFreePrevToken = 0;

	while ((curr_token = next_token(script, curr_token, shouldFreePrevToken)) != NULL) { // check syntax & extract pointers
		shouldFreePrevToken = 1;

		//printf("%s\n", substring(script, curr_token));

		if (state == T_START && isToken(script, curr_token, "generate")) {
			state = T_GENERATE;
		} else if (state == T_GENERATE && isToken(script, curr_token, "sequence")) {
			state = T_SEQUENCE;
		} else if (state == T_SEQUENCE && isToken(script, curr_token, "of")) {
			state = T_OF;
		} else if (state == T_OF && isToken(script, curr_token, "{")) {
			state = T_S_LEFT_BRACKET;
		} else if (state == T_S_LEFT_BRACKET && isSymbol(script, curr_token)) {
			state = T_S_SYMBOL;
			noSymbols++;
			if (symbols_start_token == NULL) {
				symbols_start_token = curr_token;
				shouldFreePrevToken = 0;
			}
		} else if (state == T_S_SYMBOL && isToken(script, curr_token, ",")) {
			state = T_S_COMMA;
		} else if (state == T_S_COMMA && isSymbol(script, curr_token)) {
			state = T_S_SYMBOL;
			noSymbols++;
		} else if (state == T_S_SYMBOL && isToken(script, curr_token, "}")) {
			state = T_S_RIGHT_BRACKET; // end of symbols definition
		} else if (state == T_S_RIGHT_BRACKET && isToken(script, curr_token, "where")) {
			state = T_WHERE;
		} else if (state == T_WHERE && isToken(script, curr_token, "{")) {
			state = T_W_LEFT_BRACKET;
		} else if (state == T_W_LEFT_BRACKET && isSymbol(script, curr_token)) {
			state = T_W_BEFORE_SYMBOL;
			noRules++;
			if (rules_start_token == NULL) {
				rules_start_token = curr_token;
				shouldFreePrevToken = 0;
			}
		} else if (state == T_W_BEFORE_SYMBOL && isToken(script, curr_token, "<")) {
			state = T_W_GT;
		} else if (state == T_W_GT && isSymbol(script, curr_token) ) {
			state = T_W_AFTER_SYMBOL;
		} else if (state == T_W_AFTER_SYMBOL && isToken(script, curr_token, ",")) {
			state = T_W_COMMA;
		} else if (state == T_W_COMMA && isSymbol(script, curr_token)) {
			state = T_W_BEFORE_SYMBOL;
			noRules++;
		} else if (state == T_W_AFTER_SYMBOL && isToken(script, curr_token, "}")) {
			state = T_W_RIGHT_BRACKET;
		} else if ((state == T_S_RIGHT_BRACKET || state == T_W_RIGHT_BRACKET) && isToken(script, curr_token, "starting")) {
			state = T_STARTING;
		} else if (state == T_STARTING && isSymbol(script, curr_token)) {
			state = T_STARTING_SYMBOL;
			starting_token = curr_token;
			shouldFreePrevToken = 0;
		} else if ((state == T_S_RIGHT_BRACKET || state == T_W_RIGHT_BRACKET || state == T_STARTING_SYMBOL) && isToken(script, curr_token, "ending")) {
			state = T_ENDING;
		} else if (state == T_ENDING && isSymbol(script, curr_token)) {
			state = T_ENDING_SYMBOL;
			ending_token = curr_token;
			shouldFreePrevToken = 0;
		} else if ((state == T_S_RIGHT_BRACKET || state == T_W_AFTER_SYMBOL || state == T_STARTING_SYMBOL || state == T_ENDING_SYMBOL) && isToken(script, curr_token, ";")) {
			state = T_SEMICOLON;
		} else {
			return NULL; // syntax error
		}
	}

	if (state != T_SEMICOLON) { // unfinished command -> syntax error
		return NULL;
	}

	struct symbolsDef *symbolsDef = (struct symbolsDef*)malloc(sizeof(struct symbolsDef));
	struct rulesDef *rulesDef = (struct rulesDef*)malloc(sizeof(struct rulesDef));

	rulesDef->starting = new_symbol(script, starting_token);
	rulesDef->ending = new_symbol(script, ending_token);

	symbolsDef->symbols = (struct symbol**)malloc(sizeof(struct symbol*) * noSymbols);
	symbolsDef->noSymbols = noSymbols;

	// collect symbols
	ptoken curr_symbol_token = symbols_start_token;
	for (int i = 0; i < noSymbols; i++) {
		symbolsDef->symbols[i] = new_symbol(script, curr_symbol_token);
		if (i < noSymbols - 1) {
			curr_symbol_token = next_token(script, curr_symbol_token, 0); // comma
			curr_symbol_token = next_token(script, curr_symbol_token, 1);
		}
	}

	rulesDef->rules = (struct rule**)malloc(sizeof(struct rule*) * noRules);
	rulesDef->noRules = noRules;

	// collect rules
	ptoken curr_rule_token = rules_start_token;
	for (int i = 0; i < noRules; i++) {
		ptoken rule_after_token =  next_token(script, next_token(script, curr_rule_token, 0), 1);
		rulesDef->rules[i] = new_rule(script, curr_rule_token, rule_after_token);
		if (i < noRules - 1) {
			curr_rule_token = next_token(script, curr_rule_token, 0); // comma
			curr_rule_token = next_token(script, curr_rule_token, 1);
		}
	}

	struct sequenceDef* sequenceDef = (struct sequenceDef*)malloc(sizeof(struct sequenceDef));

	sequenceDef->symbols = symbolsDef;
	sequenceDef->rules = rulesDef;

	return sequenceDef;
}
