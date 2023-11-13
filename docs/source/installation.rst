.. Copyright (c) 2016, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Installation
============

 The framework is made of three libraries:

- ``xeus``: The core implementation of the Jupyter protocol
- ``xeus-zmq``: A library to author kernels that run in dedicated processes
- ``xeus-lite``: A library to author kernels that run in the browser 

Both ``xeus-zmq`` and ``xeus-lite`` depend on ``xeus``.

Installing xeus-zmq
-------------------

With Mamba or Conda
~~~~~~~~~~~~~~~~~~~

``xeus`` and ``xeus-zmq`` have been packaged on all platforms for the mamba (or conda) package manager.

.. code::

    mamba install xeus-zmq -c conda-forge

From Source
~~~~~~~~~~~

``xeus`` depends on the following libraries:
 - xtl_ and nlohmann_json_

``xeus-zmq`` depends on the following libraries:
 - libzmq_, cppzmq_, OpenSSL_ and `xeus`

On linux platforms, ``xeus`` also requires libuuid, which is available in all linux distributions.

We have packaged all these dependencies on conda-forge. The simplest way to install them is to run:

.. code::

    mamba install cmake zeromq cppzmq OpenSSL xtl nlohmann_json -c conda-forge

On Linux platform, you will also need:

.. code::

    mamba install libuuid -c conda-forge

Once you have installed the dependencies, you can build and install `xeus`:

.. code::

    cmake -D CMAKE_BUILD_TYPE=Release .
    make
    make install

You can then build and install `xeus-zmq`:

.. code::

    cmake -D CMAKE_BUILD_TYPE=Release
    make
    make install

Installing the Dependencies from Source
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The dependencies can also be installed from source. Simply clone the directories and run the following cmake and make instructions.

- libzmq


.. code::

    cmake -D WITH_PERF_TOOL=OFF -D ZMQ_BUILD_TESTS=OFF -D ENABLE_CPACK=OFF -D CMAKE_BUILD_TYPE=Release .
    make
    make install

- cppzmq

`cppzmq` is a header only library:

.. code::

    cmake -D CMAKE_BUILD_TYPE=Release .
    make install

- OpenSSL

`OpenSSL` has been packaged for most platforms and package manager. It should generally not be required for the user to build it.

- nlohmann_json

`nlohmann_json` is a header only library:

.. code::

    cmake -DCMAKE_BUILD_TYPE=Release .
    make install

- xtl

`xtl` is a header only library:

.. code::

    cmake -DCMAKE_BUILD_TYPE=Release .
    make install

Installing xeus-lite
--------------------

TODO

.. _libzmq: https://github.com/zeromq/libzmq
.. _cppzmq: https://github.com/zeromq/cppzmq
.. _OpenSSL: https://github.com/OpenSSL/OpenSSL
.. _nlohmann_json: https://github.com/nlohmann/json
.. _xtl: https://github.com/xtensor-stack/xtl

