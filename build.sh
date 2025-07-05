#!/bin/bash -e

emcc seq-checker.h tokenizer.c parser.c rmc.c validate.c solver.c seq-checker.c -c
emcc seq-checker.o tokenizer.o parser.o solver.o validate.o rmc.o -o seq-checker.js -sEXPORTED_FUNCTIONS=_getSolution,_isError,_getSolutionText,_getVersion -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToNewUTF8,UTF8ToString -sSTANDALONE_WASM --no-entry
