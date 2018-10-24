.. Copyright (c) 2016, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Installation
============

With Conda
----------

``xeus`` has been packaged on all platforms for the conda package manager.

.. code::

    conda install xeus -c conda-forge

From Source
-----------

``xeus`` depends on the following libraries:

 - libzmq_, cppzmq_, OpenSSL_ and xtl_

On linux platforms, ``xeus`` also requires libuuid, which is available in all linux distributions.

We have packaged all these dependencies for the conda package manager. The simplest way to install them with conda is to run:

.. code::

    conda install cmake zeromq cppzmq OpenSSL xtl -c conda-forge

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

OpenSSL
~~~~~~~

`OpenSSL` has been packaged for most platforms and package manager. It should generally not be required for the user to build it.

xtl
~~~

`xtl` is a header only library:

.. code::

    cmake -DCMAKE_BUILD_TYPE=Release .
    make install

.. _libzmq: https://github.com/zeromq/libzmq
.. _cppzmq: https://github.com/zeromq/cppzmq
.. _OpenSSL: https://github.com/OpenSSL/OpenSSL
.. _xtl: https://github.com/QuantStack/xtl

