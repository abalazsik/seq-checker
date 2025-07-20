#ifndef _SEQ_CHECKER_BINDING_H_
#define _SEQ_CHECKER_BINDING_H_

//hide the actual details from the """user"""
typedef void* presult;

presult getSolution(char* script);
int isError(presult result);
char* getResultText(presult result);
int getVersion();

#endif