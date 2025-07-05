#!/bin/bash -e

function build() {
	emcc seq-checker.h tokenizer.c parser.c rmc.c validate.c solver.c seq-checker.c -c
	emcc seq-checker.o tokenizer.o parser.o solver.o validate.o rmc.o -o seq-checker.js -sEXPORTED_FUNCTIONS=_getSolution,_isError,_getSolutionText -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToNewUTF8,UTF8ToString -sSTANDALONE_WASM --no-entry
}

function host() {
	python3 -m http.server
}

function print_help() {
	echo -e "operations: build host"
}

if [ -z $1 ]
then
	print_help
	exit 0
fi

case $1 in
	 "build")
		  build ;;
	 "host")
		  host ;;
	 *)
		  echo "Unknown option $1"
		  print_help
		  exit 1
esac