#!/bin/bash -e

emcc tokenizer.c parser.c rmc.c validate.c solver.c seq-checker.c string-buffer.c -c
emcc seq-checker.o tokenizer.o parser.o solver.o validate.o rmc.o string-buffer.o -o seq-checker.js -sEXPORTED_FUNCTIONS=_getSolution,_isError,_getSolutionText,_getVersion -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToNewUTF8,UTF8ToString -sSTANDALONE_WASM --no-entry
