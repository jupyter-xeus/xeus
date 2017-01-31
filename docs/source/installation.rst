.. Copyright (c) 2016, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Installation
============

With Conda
----------

``xeus`` has been packaged on all platforms for the conda package manager.

.. code::

    conda install -c conda-forge xeus 

From Source
-----------

``xeus`` depends on the following libraries:

 - libzmq_, cppzmq_, rapidjson_, cryptopp_

On linux platforms, ``xeus`` also requires libuuid, which is available in all linux distributions (``uuid-dev`` on Debian).

We have packaged all these dependencies for the conda package manager. The simplest way to install them with conda is to run:

.. code::

    conda install cmake zeromq cppzmq rapidjson cryptopp -c conda-forge .

On Linux platform, you will also need:

.. code::

    conda install libuuid -c conda-forge

Once you have installed the dependencies, you can build and install `xeus`:

.. code::

    cmake -D BUILD_EXAMPLES=ON -D CMAKE_BUILD_TYPE=Release .
    make
    make install

If you need the ``xeus`` library only, you can omit the ``BUILD_EXAMPLES`` settings.

Installing the Dependencies from Source
---------------------------------------

The dependencies can also be installed from source. Simply clone the directories and run the following cmake and make instructions.

libzmq
~~~~~~

.. code::

    cmake -D WITH_PERF_TOOL=OFF -D ZMQ_BUILD_TESTS=OFF -D ENABLE_CPACK=OFF -D CMAKE_BUILD_TYPE=Release .
    make
    make install

cppzmq
~~~~~~

`cppzmq` is a header only library:

.. code::

    cmake -D CMAKE_BUILD_TYPE=Release .
    make install

rapidjson
~~~~~~~~~

`rapidjson` is a header only library too, but requires some options to be set:

.. code::

    cmake -D RAPIDJSON_BUILD_DOC=OFF -D RAPIDJSON_BUILD_TESTS=OFF -D RAPIDJSON_BUILD_EXAMPLES=OFF -D RAPIDJSON_HAS_STDSTRING=ON -D CMAKE_BUILD_TYPE=Release .
    make install

cryptopp
~~~~~~~~~

`cryptopp` must be built as a static library, building cryptopp as a shared library is not supported on Windows.

.. code::

    cmake -D BUILD_SHARED=OFF -D BUILD_TESTING=OFF -D CMAKE_BUILD_TYPE=Release .
    make
    make install

.. _libzmq: https://github.com/zeromq/libzmq
.. _cppzmq: https://github.com/zeromq/cppzmq
.. _rapidjson: https://github.com/miloyip/rapidjson
.. _cryptopp: https://github.com/weidai11/cryptopp

