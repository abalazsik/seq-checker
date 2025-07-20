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
	for (size_t i = at->from; i < at->to; i++) {
		char ch = script[i];
		if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))) {
			return 0;
		}
	}

	return 1;
}

int isNumber(char* script, ptoken at) {
	if (script[at->from] == '0') {
		return 0;
	}

	for (size_t i = at->from; i < at->to; i++) {
		char ch = script[i];
		if (!(ch >= '0' && ch <= '9')) {
			return 0;
		}
	}

	return 1;
}


psymbol newSymbol(char* script, ptoken curr_token) {
	psymbol sym = (psymbol)malloc(sizeof(struct symbol));
	sym->name = substring(script, curr_token);
	return sym;
}

prule newRule(char* script, ptoken before_token, ptoken after_token) {
	prule rule = (prule)malloc(sizeof(struct rule));
	rule->before = newSymbol(script, before_token);
	rule->after = newSymbol(script, after_token);
	return rule;
}

unsigned int tokenToNumber(char* script, ptoken curr_token) {
	unsigned int result = 0;

	size_t i = curr_token->from;

	while(i < curr_token->to) {
		result = result * 10 + (script[i++] - '0');
	}

	return result;
}


/*
The expected format:
	generate sequence of {A, B, C} where { A < B } starting A ending B limit 10; 
*/

enum parserState {
	T_START = 1,
	T_GENERATE,
	T_SEQUENCE,
	T_OF,
	T_S_LEFT_BRACKET,
	T_S_SYMBOL,
	T_S_COMMA,
	T_S_RIGHT_BRACKET,
	T_WHERE,
	T_W_LEFT_BRACKET,
	T_W_BEFORE_SYMBOL,
	T_W_GT,
	T_W_AFTER_SYMBOL,
	T_W_COMMA,
	T_W_RIGHT_BRACKET,
	T_NOT_1,
	T_STARTING,
	T_STARTING_SYMBOL,
	T_NOT_2,
	T_ENDING,
	T_ENDING_SYMBOL,
	T_LIMIT,
	T_LIMIT_NUMBER,
	T_SEMICOLON
};

struct sequenceDef* parseSequence(char* script) {

	ptoken curr_token = NULL;
	
	enum parserState state = T_START;

	ptoken symbols_start_token = NULL;
	ptoken rules_start_token = NULL;
	ptoken starting_token = NULL;
	ptoken ending_token = NULL;
	ptoken limit_token = NULL;
	int has_first_not_token = 0;
	int has_second_not_token = 0;

	unsigned int noSymbols = 0;
	unsigned int noRules = 0;

	int shouldFreePrevToken = 0;

	while ((curr_token = nextToken(script, curr_token, shouldFreePrevToken)) != NULL) { // check syntax & extract pointers
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
		} else if ((state == T_S_RIGHT_BRACKET || state == T_W_RIGHT_BRACKET) && isToken(script, curr_token, "not")) {
			has_first_not_token = 1;
			state = T_NOT_1;
		} else if ((state == T_NOT_1 || state == T_S_RIGHT_BRACKET || state == T_W_RIGHT_BRACKET) && isToken(script, curr_token, "starting")) {
			state = T_STARTING;
		} else if (state == T_STARTING && isSymbol(script, curr_token)) {
			state = T_STARTING_SYMBOL;
			starting_token = curr_token;
			shouldFreePrevToken = 0;
		} else if (state == T_STARTING_SYMBOL && isToken(script, curr_token, "not")){
			state = T_NOT_2;
			has_second_not_token = 1;
		} else if ((state == T_NOT_1 || state == T_NOT_2 || state == T_S_RIGHT_BRACKET || state == T_W_RIGHT_BRACKET || state == T_STARTING_SYMBOL) && isToken(script, curr_token, "ending")) {
			state = T_ENDING;
		} else if (state == T_ENDING && isSymbol(script, curr_token)) {
			state = T_ENDING_SYMBOL;
			ending_token = curr_token;
			shouldFreePrevToken = 0;
		} else if ((state == T_S_RIGHT_BRACKET || state == T_W_RIGHT_BRACKET || state == T_STARTING_SYMBOL || state == T_ENDING_SYMBOL) && isToken(script, curr_token, "limit")) {
			state = T_LIMIT;
		} else if (state == T_LIMIT && isNumber(script, curr_token)) {
			state = T_LIMIT_NUMBER;
			limit_token = curr_token;
			shouldFreePrevToken = 0;
		} else if ((state == T_W_RIGHT_BRACKET || state == T_S_RIGHT_BRACKET || state == T_W_AFTER_SYMBOL || state == T_STARTING_SYMBOL || state == T_ENDING_SYMBOL || state == T_LIMIT_NUMBER) 
				&& isToken(script, curr_token, ";")) {
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

	if (starting_token != NULL) {
		rulesDef->starting = newSymbol(script, starting_token);
	} else {
		rulesDef->starting = NULL; // this mf made me reconsider my life decisions
	}
	
	if (ending_token != NULL) {
		rulesDef->ending = newSymbol(script, ending_token);
	} else {
		rulesDef->ending = NULL;
	}

	rulesDef->notEnding = 0;
	rulesDef->notStarting = 0;

	if (has_second_not_token) {
		rulesDef->notEnding = 1;
	}

	if (has_first_not_token) {
		if (starting_token != NULL) {
			rulesDef->notStarting = 1;
		} else {
			rulesDef->notEnding = 1;
		}
	}

	symbolsDef->symbols = (struct symbol**)malloc(sizeof(struct symbol*) * noSymbols);
	symbolsDef->noSymbols = noSymbols;

	// collect symbols
	ptoken curr_symbol_token = symbols_start_token;
	for (int i = 0; i < noSymbols; i++) {
		symbolsDef->symbols[i] = newSymbol(script, curr_symbol_token);
		if (i < noSymbols - 1) {
			curr_symbol_token = nextToken(script, curr_symbol_token, 0); // comma
			curr_symbol_token = nextToken(script, curr_symbol_token, 1);
		}
	}

	rulesDef->rules = (struct rule**)malloc(sizeof(struct rule*) * noRules);
	rulesDef->noRules = noRules;

	// collect rules
	ptoken curr_rule_token = rules_start_token;
	for (int i = 0; i < noRules; i++) {
		ptoken rule_after_token =  nextToken(script, 
			nextToken(script, curr_rule_token, 0), // '<' sign
		 1);
		rulesDef->rules[i] = newRule(script, curr_rule_token, rule_after_token);
		if (i < noRules - 1) {
			curr_rule_token = nextToken(script, 
				nextToken(script, rule_after_token, 1), // comma
			0);
		}
	}
	
	unsigned int limit = 1; // default

	if (limit_token != NULL) {
		limit = tokenToNumber(script, limit_token);
	}

	struct sequenceDef* sequenceDef = (struct sequenceDef*)malloc(sizeof(struct sequenceDef));

	sequenceDef->symbols = symbolsDef;
	sequenceDef->rules = rulesDef;
	sequenceDef->limit = limit;

	return sequenceDef;
}
