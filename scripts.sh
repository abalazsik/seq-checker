#!/bin/bash -e

function build() {
	source ./build.sh
}

function build_on_podman() {
	podman run --rm -v $(pwd):/src docker.io/emscripten/emsdk sh build.sh
}

function host() {
	python3 -m http.server
}

function generateHeader() {
	javac -h . SeqChecker.java
}

function build_lib() {
	generateHeader
	JAVA_HEADERS="$JAVA_HOME/include"
	echo $JAVA_HEADERS
	OG_SOURCES="seq-checker.h tokenizer.c parser.c validate.c solver.c seq-checker.c string-buffer.c"
	BINDINGS="SeqChecker.h java-binding.c"
	gcc -shared -fPIC -I$JAVA_HEADERS -I$JAVA_HEADERS/linux $OG_SOURCES $BINDINGS -o seq-checker.so
}

function build_tester() {
	gcc seq-checker.h tokenizer.c parser.c validate.c solver.c seq-checker.c string-buffer.c -D_SEQ_CHECKER_TESTER_ -g -O0 -o seq-checker-tester
}

function memcheck() {
	build_tester
	valgrind --leak-check=yes --track-origins=yes ./seq-checker-tester dressUp.test
}

function print_help() {
	echo -e "operations: \n build\n build_on_podman\n host\n build_lib\n build_tester\n memcheck\n"
}

if [ -z $1 ]
then
	print_help
	exit 0
fi

case $1 in
	 "build")
		  build ;;
	 "build_on_podman")
		  build_on_podman ;;
	 "host")
		  host ;;
	 "build_lib")
		  build_lib ;;
	 "build_tester")
		  build_tester ;;
	 "memcheck")
	 	  memcheck ;;
	 *)
		  echo "Unknown option $1"
		  print_help
		  exit 1
esac