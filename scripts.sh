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
	OG_SOURCES="seq-checker.h tokenizer.c parser.c rmc.c validate.c solver.c seq-checker.c string-buffer.c"
	BINDINGS="SeqChecker.h java-binding.c"
	gcc -shared -fPIC -I$JAVA_HEADERS -I$JAVA_HEADERS/linux $OG_SOURCES $BINDINGS -o seq-checker.so
}

function print_help() {
	echo -e "operations: \n build\n build_on_podman\n host\n build_lib"
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
	 *)
		  echo "Unknown option $1"
		  print_help
		  exit 1
esac