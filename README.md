# ![xeus](docs/source/xeus.svg)
[![GithubActions](https://github.com/jupyter-xeus/xeus/actions/workflows/main.yml/badge.svg)](https://github.com/jupyter-xeus/xeus/actions/workflows/main.yml)
[![Documentation Status](http://readthedocs.org/projects/xeus/badge/?version=latest)](https://xeus.readthedocs.io/en/latest/?badge=latest)
[![Zulip](https://img.shields.io/badge/social_chat-zulip-blue.svg)](https://jupyter.zulipchat.com/#narrow/channel/539737-Jupyter-Kernels)

C++ implementation of the Jupyter Kernel protocol

## Introduction

`xeus` is a library meant to facilitate the implementation of kernels for Jupyter. It takes the
burden of implementing the Jupyter Kernel protocol so developers can focus on implementing the
interpreter part of the kernel.

Several Jupyter kernels are built upon xeus, such as [`xeus-cling`](https://github.com/jupyter-xeus/xeus-cling),
a kernel for the C++ programming language, and [`xeus-python`](https://github.com/jupyter-xeus/xeus-python), an alternative Python kernel for Jupyter.

## Installation

`xeus` has been packaged on all platforms for the mamba (or conda) package manager.

```
mamba install xeus -c conda-forge
```

## Documentation

To get started with using `xeus`, check out the full documentation

http://xeus.readthedocs.io/

## Usage

`xeus` enables custom kernel authors to implement Jupyter kernels more easily. It takes the burden of implementing the Jupyter Kernel protocol so developers can focus on implementing the interpreter part of the Kernel.

The easiest way to get started with a new kernel is to inherit from the base interpreter class `xeus::xinterpreter` and implement the private virtual methods:

- `configure_impl`
- `execute_request_impl`
- `complete_request_impl`
- `inspect_request_impl`
- `is_complete_request_impl`
- `kernel_info_request_impl`
- `shutdown_request_impl`

as seen in the [documentation](http://xeus.readthedocs.io/).


```cpp
#include "xeus/xinterpreter.hpp"

#include "nlohmann/json.hpp"

using xeus::xinterpreter;

namespace nl = nlohmann;

namespace custom
{
    class custom_interpreter : public xinterpreter
    {
    public:

        custom_interpreter() = default;
        virtual ~custom_interpreter() = default;

    private:

        void configure_impl() override;

        void execute_request_impl(send_reply_callback cb,
                                  int execution_counter,
                                  const std::string& code,
                                  execute_request_config config,
                                  nl::json user_expressions) override;

        nl::json complete_request_impl(const std::string& code,
                                       int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;
    };
}
```

Kernel authors can then rebind to the native APIs of the interpreter that is being interfaced, providing richer information than with the classical approach of a wrapper kernel capturing textual output.

## Building from Source

`xeus` depends on [`nlohmann_json`](https://github.com/nlohmann/json).

|  xeus   | nlohmann json |
|---------|---------------|
| master  |    ^3.11.0    |
| 5.x     |    ^3.11.0    |

Versions prior to version 5 depend on the following libraries: [`nlohmann_json`](https://github.com/nlohmann/json) and [`xtl`](https://github.com/xtensor-stack/xtl).

|  xeus   |   xtl          | nlohmann json |
|---------|----------------|---------------|
| master  | >=0.7.0,<0.8.0 |    ^3.11.0    |
| 4.x     | >=0.7.0,<0.8.0 |    ^3.11.0    |
| 3.x     | >=0.7.0,<0.8.0 |    ^3.2.0     |

Versions prior to version 3 also depend on the following libraries: [`ZeroMQ`](https://github.com/zeromq/libzmq),
[`cppzmq`](https://github.com/zeromq/cppzmq), and [`OpenSSL`](https://github.com/openssl/openssl).


|  xeus   | ZeroMQ  | cppzmq  |   xtl          | nlohmann json | OpenSSL |
|---------|---------|---------|----------------|---------------|---------|
| 2.4.1   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.4.0   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.3.1   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.3.0   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.2.0   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.1.1   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.1.0   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 2.0.0   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 1.0.4   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 1.0.3   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 1.0.2   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 1.0.1   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |
| 1.0.0   |  ^4.2.5 |  ^4.7.1 | >=0.7.0,<0.8.0 |      ^3.2.0   |  ^1.0.1 |

On Linux platforms, `xeus` also requires `libuuid`, which is available in all linux distributions (`uuid-dev` on Debian).

We have packaged all these dependencies on conda-forge. The simplest way to install them is to run:

```bash
mamba install cmake pkg-config xtl nlohmann_json -c conda-forge
```

On Linux platforms, you will also need `libuuid`:

```bash
mamba install libuuid -c conda-forge
```

Once you have installed the dependencies, you can build and install `xeus`.
From the `build` directory, run:

```bash
cmake -D CMAKE_BUILD_TYPE=Release ..
make
make install
```

## Installing the Dependencies from Source

The dependencies can also be installed from source. Simply clone the directories and run the following cmake (cmake >= 3.8)  and make instructions.

### ZeroMQ

[ZeroMQ](https://github.com/zeromq/libzmq) is the messaging library underlying the Jupyter kernel protocol.

```bash
cmake -D WITH_PERF_TOOL=OFF -D ZMQ_BUILD_TESTS=OFF -D ENABLE_CPACK=OFF
-D CMAKE_BUILD_TYPE=Release
make
make install
```

## OpenSSL

[OpenSSL](https://www.openssl.org/) is packaged for most package managers (apt-get, rpm, mamba).
We recommend making use of an off-the-shelf build of OpenSSL for your system.

For more information on building OpenSSL, check out the official [OpenSSL wiki](https://wiki.openssl.org/index.php/Compilation_and_Installation).

### cppzmq

[cppzmq](https://github.com/zeromq/cppzmq) is a header only library:

```bash
cmake -D CMAKE_BUILD_TYPE=Release
make install
```

### json for modern cpp

[nlohmann_json](https://github.com/nlohmann/json) is a header only library

```bash
cmake
make install
```

### xtl

[xtl](https://github.com/xtensor-stack/xtl) is a header only library:

```bash
cmake -D CMAKE_BUILD_TYPE
make install
```

## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) to know how to contribute and set up a development environment.

## License

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
