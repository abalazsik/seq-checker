#ifndef _SEQ_CHECKER_BINDING_H_
#define _SEQ_CHECKER_BINDING_H_

//hide the actual details from the """user"""
typedef void* psolution;

psolution getSolution(char* script);
int isError(psolution solution);
char* getSolutionText(psolution solution);
int getVersion();

#endif