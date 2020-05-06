.. Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Implementing a kernel
=====================

In most of the cases, the base kernel implementation is enough, and creating a kernel only means implementing the interpreter part.

The structure of your project should at least look like the following:

.. code::

    └── example/
        ├── src/
        │   ├── custom_interpreter.cpp
        │   ├── custom_interpreter.hpp
        │   └── main.cpp
        ├── share/
        │   └── jupyter/
        │       └── kernels/
        │           └── my_kernel/
        │               └── kernel.json.in
        └── CMakeLists.txt

Implementing the interpreter
----------------------------

Let's start by editing the ``custom_interpreter.hpp`` file, it should contain the declaration of your interpreter class:

.. literalinclude:: ../../example/src/custom_interpreter.hpp
   :language: cpp

.. note::
    Almost all ``custom_interpreter`` methods return a ``nl::json`` instance. This is actually using `nlohmann json
    <https://github.com/nlohmann/json>`_ which is a modern C++ implementation of a JSON datastructure.

Code Execution
~~~~~~~~~~~~~~

Then, you would need to implement all of those methods one by one in the ``custom_interpreter.cpp`` file. The main method is of
course the ``execute_request_impl`` which executes the code whenever the client is sending an execute request.

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 22-50

The result and arguments of the execution request are described in the execute_request_ documentation.

.. note::
    The other methods are all optional, but we encourage you to implement them in order to have a fully-featured kernel.

Input request
~~~~~~~~~~~~~

For input request support, you would need to monkey-patch the language functions that prompt for a user input (``input``
and ``raw_input`` in Python, ``io.read`` in Lua etc) and call ``xeus::blocking_input_request`` instead. The second parameter
should be set to False is what the user is typing should not be visible on the screen.

.. code::

    #include "xeus/xinput.hpp"

    xeus::blocking_input_request("User name:", true);
    xeus::blocking_input_request("Password:", false);

Configuration
~~~~~~~~~~~~~

The ``configure_impl`` method allows you to perform some operations after the ``custom_interpreter`` creation and before executing
any request. This is optional, but it can be useful, for example it is used in `xeus-python <https://github.com/jupyter-xeus/xeus-python>`_
for initializing the auto-completion engine.

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 52-55

Code Completion
~~~~~~~~~~~~~~~

The ``complete_request_impl`` method allows you to implement the auto-completion logic for your kernel.

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 57-80

The result and arguments of the completion request are described in the complete_request_ documentation.

Code Inspection
~~~~~~~~~~~~~~~

Allows the kernel user to inspect a variable/class/type in the code. It takes the code and the cursor position as arguments,
it is up to the kernel author to extract the token at the given cursor position in the code in order to know for which name the
user wants inspection.

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 82-100

The result and arguments of the inspection request are described in the inspect_request_ documentation.

Code Completeness
~~~~~~~~~~~~~~~~~

This request is never called from the Notebook or from JupyterLab clients, but it is called from the Jupyter console client. It
allows the client to know if the user finished typing his code, before sending an execute request. For example, in Python, the
following code is not considered as complete:

.. code::

    def foo:

So the kernel should return "incomplete" with an indentation value of 4 for the next line.

The following code is considered as complete:

.. code::

    def foo:
        print("bar")

So the kernel should return "complete".

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 102-117

The result and arguments of the completness request are described in the is_complete_request_ documentation.

Kernel info
~~~~~~~~~~~

This request allows the client to get information about the kernel: language, language version, kernel version, etc.

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 119-129

The result and arguments of the kernel info request are described in the kernel_info_request_ documentation.

Kernel shutdown
~~~~~~~~~~~~~~~

This allows you to perform some operations before shutting down the kernel.

.. literalinclude:: ../../example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 131-133

Implementing the main entry
---------------------------

Now let's edit the ``main.cpp`` file which is the main entry for the kernel executable.


.. literalinclude:: ../../example/src/main.cpp
   :language: cpp

Kernel file
-----------

The ``kernel.json`` file is a ``json`` file used by Jupyter in order to retrieve all the available kernels.

It must be installed in the ``INSTALL_PREFIX/share/jupyter/kernels/my_kernel`` directory, we will see how to
do that in the next chapter.

This ``json`` file contains:

    - ``display_name``: the name that the Jupyter client should display in its interface (e.g. on the main JupyterLab page).
    - ``argv``: the command that the Jupyter client needs to run in order to start the kernel. You should leave this value
      unchanged if you are not sure what you are doing.
    - ``language``: the target language of your kernel.

You can edit the ``kernel.json.in`` file as following. This file will be used by cmake for generating the actual ``kernel.json``
file which will be installed.

.. literalinclude:: ../../example/share/jupyter/kernels/my_kernel/kernel.json.in
   :language: json


.. note::
    You can provide logos that will be used by the Jupyter client. Those logos should be in files named ``logo-32x32.png`` and
    ``logo-64x64.png`` (``32x32`` and ``64x64`` being the size of the image in pixels), they should be placed next to the ``kernel.json.in`` file.


Compiling and installing the kernel
-----------------------------------

Your ``CMakeLists.txt`` file should look like the following:

.. literalinclude:: ../../example/CMakeLists.txt
   :language: cmake

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

That's it! Now if you run the Jupyter Notebook interface you should be able to create a new Notebook
selecting the ``my_kernel`` kernel. Congrats!

Writing unit-tests for your kernel
----------------------------------

For writing unit-tests for you kernel, you can use the
`jupyter_kernel_test <https://github.com/jupyter/jupyter_kernel_test>`_ Python library.
It allows you to test the results of the requests you send to the kernel.

.. _execute_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#execute
.. _complete_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#completion
.. _inspect_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#introspection
.. _is_complete_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#code-completeness
.. _kernel_info_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#kernel-info
