.. Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Compiling and installing the kernel
===================================

Your ``CMakeLists.txt`` file should look like the following:

.. literalinclude:: ../../example/CMakeLists.txt
   :language: cmake
   :linenos:

Now you should be able to install your new kernel and use it with any Jupyter client.

For the installation you first need to install dependencies, the easier way is using ``conda``:

.. code::

    conda install -c conda-forge cmake jupyter xeus xtl nlohmann_json cppzmq

Then create a ``build`` folder in the repository and build the kernel from there:

.. code::

    mkdir build
    cd build
    cmake -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX ..
    make
    make install

That's it! Now if you run the Jupyter Notebook interface you should be able to create a new Notebook selecting the ``my_kernel`` kernel. Congrats!
