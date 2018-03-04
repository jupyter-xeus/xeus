# ![xeus](http://quantstack.net/assets/images/xeus.svg)

[![Travis](https://travis-ci.org/QuantStack/xeus.svg?branch=master)](https://travis-ci.org/QuantStack/xeus)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/5alkw5iiere4mox2?svg=true)](https://ci.appveyor.com/project/QuantStack/xeus)
[![Documentation Status](http://readthedocs.org/projects/xeus/badge/?version=latest)](https://xeus.readthedocs.io/en/latest/?badge=latest)
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

C++ implementation of the Jupyter Kernel protocol

## Introduction

`xeus` is a library meant to facilitate the implementation of kernels for Jupyter. It takes the
burden of implementing the Jupyter Kernel protocol so developers can focus on implementing the
interpreter part of the kernel.

An example of kernel built with xeus is [`xeus-cling`](https://github.com/QuantStack/xeus-cling),
a kernel for the C++ programming language based on the `cling` C++ interpreter.

## Installation

`xeus` has been packaged on all platforms for the conda package manager.

```
conda install xeus -c QuantStack
```

## Usage

`xeus` enables custom kernel authors to implement Jupyter kernels more easily. It takes the burden of implementing the Jupyter Kernel protocol so developers can focus on implementing the interpreter part of the Kernel.

The easiest way to get started with a new kernel is to inherit from the base interpreter class `xeus::xinterpreter` and implement the private virtual methods:

- `execute_request_impl`
- `complete_request_impl`
- `inspect_request_impl`
- `history_request_impl`
- `is_complete_request_impl`

as seen in the echo kernel provided as an example.


```cpp
#include "xeus/xinterpreter.hpp"

using xeus::xinterpreter;
using xeus::xjson;
using xeus::xhistory_arguments;

namespace echo_kernel
{
    class echo_interpreter : public xinterpreter
    {

    public:

        echo_interpreter() = default;
        virtual ~echo_interpreter() = default;

    private:

        void configure() override;

        xjson execute_request_impl(int execution_counter,
                                   const std::string& code,
                                   bool silent,
                                   bool store_history,
                                   const xjson::node_type* user_expressions,
                                   bool allow_stdin) override;

        xjson complete_request_impl(const std::string& code,
                                    int cursor_pos) override;

        xjson inspect_request_impl(const std::string& code,
                                   int cursor_pos,
                                   int detail_level) override;

        xjson history_request_impl(const xhistory_arguments& args) override;

        xjson is_complete_request_impl(const std::string& code) override;

        xjson kernel_info_request_impl() override;

        void input_reply_impl(const std::string& value) override;
    };
}
```

Kernel authors can then rebind to the native APIs of the interpreter that is being interfaced, providing richer information than with the classical approach of a wrapper kernel capturing textual output.

## Building from Source

`xeus` depends on the following libraries: [`libzmq`](https://github.com/zeromq/libzmq),
[`cppzmq`](https://github.com/zeromq/cppzmq), [`cryptopp`](https://github.com/weidai11/cryptopp),
[`nlohmann_json`](https://github.com/nlohmann/json), and [`xtl`](https://github.com/QuantStack/xtl).

|  xeus  | libzmq | cppzmq | cryptopp |   xtl  | nlohmann json |
|--------|--------|--------|----------|--------|---------------|
| master |  4.2.3 |  4.2.3 |    5.6.5 | ^0.4.0 | ^3.1.1        |
| 0.11.0 |  4.2.3 |  4.2.3 |    5.6.5 | ^0.4.0 | ^3.1.1        |
| 0.10.x |  4.2.3 |  4.2.3 |    5.6.5 | ^0.4.0 |               |
|  0.9.x |  4.2.3 |  4.2.2 |    5.6.5 | ^0.3.4 |               |
|  0.8.x |  4.2.1 |  4.2.1 |    5.6.5 | ^0.3.4 |               |


On Linux platforms, `xeus` also requires `libuuid`, which is available in all linux distributions (`uuid-dev` on Debian).

We have packaged all these dependencies for the conda package manager. The simplest way to install them with
conda is to run:

```bash
conda install cmake zeromq cppzmq cryptopp nlohmann_json -c QuantStack
```

On Linux platform, you will also need:

```bash
conda install util-linux -c conda-forge
```

Once you have installed the dependencies, you can build and install `xeus`:

```bash
cmake -D BUILD_EXAMPLES=ON -D CMAKE_BUILD_TYPE=Release
make
make install
```

If you need the `xeus` library only, you can omit the `BUILD_EXAMPLES` settings.

## Installing the Dependencies from Source

The dependencies can also be installed from source. Simply clone the directories and run the following cmake and make instructions.

### libzmq

```bash
cmake -D WITH_PERF_TOOL=OFF -D ZMQ_BUILD_TESTS=OFF -D ENABLE_CPACK=OFF
-D CMAKE_BUILD_TYPE=Release
make
make install
```

### cppzmq

`cppzmq` is a header only library:

```bash
cmake -D CMAKE_BUILD_TYPE=Release
make install
```

### json for modern cpp

`nlohmann_json` is a header only library

```bash
cmake
make install
```

### cryptopp

`cryptopp` must be built as a static library. Building cryptopp as a shared library is not supported on Windows.

```bash
cmake -D BUILD_SHARED=OFF -D BUILD_TESTING=OFF -D CMAKE_BUILD_TYPE=Release
make
make install
```

### xtl

`xtl` is a header only library:

```bash
cmake -D CMAKE_BUILD_TYPE
make install
```

## License

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
