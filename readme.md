seq-checker
----

This is a POC solver. The idea is to experiment with WASM, emscripten and related technologies.

Building
----

If you have emscripten installed, you can issue the following command:

```
./script.sh build
```

If you don't, but you have podman installed, you can still build it:
```
./script.sh build_on_podman
```

Trying out
----

Due to limitation of wasm module loading, you have to host it on your machine. If you have python installed:
```
./script.sh host
```

[Then open the html file](http://localhost:8000/test.html)

Or try it online
----

[Here](https://frontend-stash.s3.eu-central-1.amazonaws.com/seq-checker/test.html)

Java example
----

Now it includes an example for calling it (as a native library) from java.

Build the linux library with:

```
./script.sh build_lib
```

Run the example:

```
java SeqCheckerTest.java
```

Requirements:
- Java (Tested with Temurin openjdk 23.0.2)  
- GCC 14.2.0  