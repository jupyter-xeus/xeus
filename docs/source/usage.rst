.. Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Usage
=====

``xeus`` enables custom kernel authors to implement Jupyter kernels more easily. It takes the burden of implementing the Jupyter Kernel protocol so developers can focus on implementing the interpreter part of the Kernel.

.. note::
    In the following documentation:
      - ``interpreter`` refers to the part of the kernel responsible for executing the code, completing the code etc.
      - ``client`` refers to the Jupyter client, which can be Jupyter Notebook/JupyterLab/Jupyter console etc.
      - ``user`` refers to people using the kernel on any Jupyter client.

The easiest way to get started with a new kernel is to create a class inheriting from the base interpreter class ``xeus::xinterpreter`` and implement the private virtual methods

- ``execute_request_impl``: See execute_request_
  Code execution request from the client.
- ``complete_request_impl``: See complete_request_
  Code completion request from the client.
- ``inspect_request_impl``: See inspect_request_
  Code inspection request (using a question mark on a type for example).
- ``is_complete_request_impl``: See is_complete_request_
  Called before code execution (terminal mode) in order to check if the code is complete
  and can be executed as it is (e.g. when typing a `for` loop on multiple lines in Python, code will be considered
  complete when the `for` loop has been closed).
- ``kernel_info_request_impl``: See kernel_info_request_
  Information request about the kernel: language name (for code highlighting),
  language version, terminal banner etc.
- ``shutdown_request_impl``:
  Shutdown request from the client, this allows you to do some extra work before the kernel
  is shut down (e.g. free allocated memory).

An EchoKernel is provided as an example_, this kernel implementation only prints what it receives from the client.
You can also find real kernel implementations based on xeus:
  - `xeus-cling <https://github.com/QuantStack/xeus-cling>`_: C++ kernel
  - `xeus-python <https://github.com/QuantStack/xeus-python>`_: Python kernel
  - `JuniperKernel <https://github.com/JuniperKernel/JuniperKernel>`_: R kernel

Implementing a kernel
=====================

In most of the cases, the base kernel implementation is enough, and creating a kernel only means implementing the interpreter part.

The structure of your project should at least look like the following:

.. code::

    └── my_kernel/
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

We start by implementing the ``custom_interpreter.hpp`` and ``custom_interpreter.cpp`` files:

.. toctree::
   :maxdepth: 2

   interpreter

Then we implement the ``main.cpp`` file:

.. toctree::
   :maxdepth: 2

   main

We implement the ``kernel.json`` file:

.. toctree::
   :maxdepth: 2

   kernel_file

And we build and install our kernel:

.. toctree::
   :maxdepth: 2

   building

We provide the `example project <https://github.com/QuantStack/xeus/blob/master/docs/source/my_kernel>`_ in the source of the documentation.

Finally we write unit tests for our kernel:

.. toctree::
   :maxdepth: 2

   testing


.. _example: https://github.com/QuantStack/xeus/blob/master/example/echo_kernel
.. _execute_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#execute
.. _complete_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#completion
.. _inspect_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#introspection
.. _is_complete_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#code-completeness
.. _kernel_info_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#kernel-info
