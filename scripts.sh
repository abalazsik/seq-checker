#!/bin/bash -e

function build() {
	emcc rmc.cpp -o rmc.html -sEXPORTED_FUNCTIONS=_remove_comments -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,stringToNewUTF8,UTF8ToString
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