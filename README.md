# ![xeus](http://quantstack.net/assets/images/xeus.svg)

C++ implementation of the Jupyter Kernel protocol

## Introduction

`xeus` is a library meant to facilitate the implementation of kernels for Jupyter. It takes the
burden of implementing the Jupyter Kernel protocol so developers can focus on implementing the
interpreter part of the Kernel.

## Manual Building and installation

`xeus` depends on the following libraries:

 - [libzmq](https://github.com/zeromq/libzmq)
 - [cppzmq](https://github.com/zeromq/cppzmq)
 - [rapidjson](https://github.com/miloyip/rapidjson)
 - [cryptopp](https://github.com/weidai11/cryptopp)

Depending on your system and the installation method you use, cmake may not be able to find them.
In that case, a solution is to build these dependencies with cmake. On windows, you can use the
"NMake Makefiles" generator so you don't have to open Visual Studio to compile and install. Be
sure to run cmake from the Developer Command Prompt for Visual Studio.

On Unix operating systems, `xeus` also depends on `libuuid`, which is generally provided as a package for common linux distribution (`uuid-dev` on Debian).

### libzmq

```bash
cmake -D WITH_PERF_TOOL=OFF -D ZMQ_BUILD_TESTS=OFF -D ENABLE_CPACK=OFF
-D CMAKE_BUILD_TYPE=Release
make
make install
```
Note: this ensures you build the library only, tests and performance tools are not required
for building `xeus`.

### cppzmq

`cppzmq` is a header only library:

```bash
cmake -D CMAKE_BUILD_TYPE=Release
make install
```

### rapidjson

`rapidjson` is a header only library too, but requires some options to be set:

```bash
cmake -D RAPIDJSON_BUILD_DOC=OFF -D RAPIDJSON_BUILD_TESTS=OFF
-D RAPIDJSON_BUILD_EXAMPLES=OFF -D RAPIDJSON_HAS_STDSTRING=ON
-D CMAKE_BUILD_TYPE=Release
make install
```

### cryptopp

`cryptopp` must be built as a static library, the shared library build doest' work on
Windows.

```bash
cmake -D BUILD_SHARED=OFF -D BUILD_TESTING=OFF -D CMAKE_BUILD_TYPE=Release
make
make install
```

### xeus

Once you've built and installed the dependencies, you can build `xeus`:

```bash
cmake -D BUILD_EXAMPLES=ON -D CMAKE_BUILD_TYPE=Release
make
make install
```

If you need the `xeus` library only, you can omit the `BUILD_EXAMPLES` settings.

