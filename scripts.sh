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

function print_help() {
	echo -e "operations: build build_on_podman host"
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
	 *)
		  echo "Unknown option $1"
		  print_help
		  exit 1
esac