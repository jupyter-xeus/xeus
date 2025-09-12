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


The `xeus-cookiecutter`_ project provides a template for a xeus-based kernel, and includes the base structure for a xeus-based kernel.

Implementing the interpreter
----------------------------

Let's start by editing the ``custom_interpreter.hpp`` file, it should contain the declaration of your interpreter class:

.. literalinclude:: ./example/src/custom_interpreter.hpp
   :language: cpp

.. note::
    Almost all ``custom_interpreter`` methods return a ``nl::json`` instance. This is actually using `nlohmann json
    <https://github.com/nlohmann/json>`_ which is a modern C++ implementation of a JSON datastructure.

In the following sessions we will see details about each one of the methods that need to be implemented in order to have a functional kernel. The user can opt for using the reply API that will appropriately create replies to send to the kernel, or create the replies themselves.

Code Execution
~~~~~~~~~~~~~~

You can implement all the methods described here in the ``custom_interpreter.cpp`` file. The main method is of
course the ``execute_request_impl`` which executes the code whenever the client is sending an execute request.

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 22-57

The result and arguments of the execution request are described in the execute_request_ documentation.

.. note::
    The other methods are all optional, but we encourage you to implement them in order to have a fully-featured kernel.

Within this method the use of create_error_reply_ and create_successful_reply_ might be useful.

Input request
~~~~~~~~~~~~~

For input request support, you would need to monkey-patch the language functions that prompt for a user input (``input``
and ``raw_input`` in Python, ``io.read`` in Lua etc) and call ``xeus::blocking_input_request`` instead. The first parameter
should be forwarded from the execution_request implementation. The third parameter should be set to False if what the user
is typing should not be visible on the screen.

.. code::

    #include "xeus/xinput.hpp"

    xeus::blocking_input_request(request_context, "User name:", true);
    xeus::blocking_input_request(request_context, "Password:", false);

Configuration
~~~~~~~~~~~~~

The ``configure_impl`` method allows you to perform some operations after the ``custom_interpreter`` creation and before executing
any request. This is optional, but it can be useful, for example it is used in `xeus-python <https://github.com/jupyter-xeus/xeus-python>`_
for initializing the auto-completion engine.

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 50-53

Code Completion
~~~~~~~~~~~~~~~

The ``complete_request_impl`` method allows you to implement the auto-completion logic for your kernel.

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 55-68

The result and arguments of the completion request are described in the complete_request_ documentation.

Code Inspection
~~~~~~~~~~~~~~~

Allows the kernel user to inspect a variable/class/type in the code. It takes the code and the cursor position as arguments,
it is up to the kernel author to extract the token at the given cursor position in the code in order to know for which name the
user wants inspection.

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 70-85

The result and arguments of the inspection request are described in the inspect_request_ documentation and the create_inspect_reply_ might be useful to create a reply within specifications.

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

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 87-90

The result and arguments of the completness request are described in the is_complete_request_ documentation. Both create_default_complete_reply_ and create_is_complete_reply_ methods are recommended.

Kernel info
~~~~~~~~~~~

This request allows the client to get information about the kernel: language, language version, kernel version, etc.

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 92-101

The result and arguments of the kernel info request are described in the kernel_info_request_ documentation. The create_info_reply_ method will help you to provide complete information about your kernel.

Kernel shutdown
~~~~~~~~~~~~~~~

This allows you to perform some operations before shutting down the kernel.

.. literalinclude:: ./example/src/custom_interpreter.cpp
   :language: cpp
   :dedent: 4
   :lines: 103-106

Kernel replies
--------------

Error reply
~~~~~~~~~~~

Creates a default error reply to the kernel or allows custom input. The signature of the method is the following:

.. code::
     nl:\:json create_error_reply(const std:\:string& ename,
                                  const std:\:string& evalue,
                                  const std:\:vector<std:\:string>& trace_back)

Where ``evalue`` is exception value, ``ename`` is exception name and ``trace_back`` a vector of strings with the exception stack.

Successful reply
~~~~~~~~~~~~~~~~

Creates a default success reply to the kernel or allows custom input. The signature of the method is the following:

.. code::
    nl:\:json create_successful_reply(const std:\:vector<nl:\:json>& payload,
                                     const nl:\:json& user_expressions)

Where ``payload`` is a way to trigger frontend actions from the kernel (payloads are deprecated but since there are still no replecement for it you might need to use it). You can find more information about the different kinds of payloads in the `official documentation <https://jupyter-client.readthedocs.io/en/stable/messaging.html#payloads-deprecated>`_. ``data`` is a dictionary which the keys is a ``MIME_type`` (this is the type of data to be shown it must be a valid MIME type, for a list of the possibilities check MDN_, note that you're not limited by these types) and the values are the content of the information intended to be displayed in the frontend. And ``user_expressions`` is a dictionary of strings of arbitrary code, more information about it on the `official documentation <https://jupyter-client.readthedocs.io/en/stable/messaging.html#execute>`_.

Complete reply
~~~~~~~~~~~~~~

Creates a custom completion reply to the kernel. The signature of the method is the following:

.. code::
    nl:\:json create_complete_reply(const std:\:vector<std:\:string>& matches,
                                   const int cursor_start,
                                   const int cursor_end,
                                   const nl:\:json metadata)

Where ``matches`` the list of all matches to the completion request, it's a mandatory argument. ``cursor_start`` and ``cursor_end`` mark the range of text that should be replaced by the above matches when a completion is accepted, typically ``cursor_end`` is the same as ``cursor_pos`` in the request and both these arguments are mandatory for the implementation of the method. ``metadata`` a dictionary of strings that contains information that frontend plugins might use for extra display information about completions.

In case you do not wish to implement completion in your kernel, instead of creating a complete reply you can use the ``create_successful_reply`` with its default arguments.

Is complete reply
~~~~~~~~~~~~~~~~~

Creates a default is complete reply to the kernel or allows custom input. The signature of the method is the following:

.. code::
    nl:\:json create_is_complete_reply(const std:\:string& status,
                                        const std:\:string& indent)

``status`` one of the following 'complete', 'incomplete', 'invalid', 'unknown'. ``indent`` if status is 'incomplete', indent should contain the characters to use to indent the next line. This is only a hint: frontends may ignore it and use their own autoindentation rules. For other statuses, this field does not exist.

Create info reply
~~~~~~~~~~~~~~~~~

.. code::
    nl:\:json create_info_reply(const std:\:string& protocol_version,
                               const std:\:string& implementation,
                               const std:\:string& implementation_version,
                               const std:\:string& language_name,
                               const std:\:string& language_version,
                               const std:\:string& language_mimetype,
                               const std:\:string& language_file_extension,
                               const std:\:string& language_pygments_lexer,
                               const std:\:string& language_codemirror_mode,
                               const std:\:string& language_nbconvert_exporter,
                               const std:\:string& banner,
                               const bool& debugger,
                               const nl:\:json& help_links)

Thorough information about the kernel's infos variables can be found in the Jupyter kernel docs_.

Outputs and display
-------------------

The ``xinterpreter`` class provides several methods for sending data to be displayed
to the frontend(s), that you can call from the implementation of your interpreter class:

- ``publish_stream``: this method is used to send data that should be print on the standard
  output streams (``stdout`` and ``stderr`` in Python, ``std::cout`` and ``std::cerr`` in C++).
  The usual way to have it called when executing user code is to redirect standard streams. This
  method should not be called when executing code in silent mode (i.e. when ``execute_request_impl``
  is called with a ``config`` argument whose ``silent`` member is ``true``).
- ``publish_execution_input``: this method sends the executed code to all the frontends connected
  to the kernel. Like ``publish_stream``, it should not be called when executing code in silent mode.
  This method is already called in the ``execute_request`` method of the ``xinterpreter`` class and
  there should be no need to call it from your implementation. It is provided for backward compatibility
  purpose.
- ``publish_exeuction_result``: this sends the result of the execution to all the frontends connected
  to the kernel. It should be called when the execution is successful, and the code was not executed
  in silent mode.
- ``publish_execution_error``: this method sends an execution error to all the frontends. It should be
  called when the code failed to execute and was not executed in silent mode.
- ``display_data``: this method sends data to be displayed to all the frontends. It should be called
  from executing a special function in the user code (``display`` in Python and C++, ``display_data``
  in MatLab). This function should be called even if the code is executed in silent mode.
- ``update_diplay_data``: when a ``display_id`` is specified for a display, it can be updated later
  with a call to this method. Like ``display_data``, this method should be called even if the code
  is executed in silent mode.

Implementing the main entry
---------------------------

Now let's edit the ``main.cpp`` file which is the main entry for the kernel executable.


.. literalinclude:: ./example/src/main.cpp
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

.. literalinclude:: ./example/share/jupyter/kernels/my_kernel/kernel.json.in
   :language: json


.. note::
    You can provide logos that will be used by the Jupyter client. Those logos should be in files named ``logo-32x32.png`` and
    ``logo-64x64.png`` (``32x32`` and ``64x64`` being the size of the image in pixels), they should be placed next to the ``kernel.json.in`` file.


Compiling and installing the kernel
-----------------------------------

Your ``CMakeLists.txt`` file should look like the following:

.. literalinclude:: ./example/CMakeLists.txt
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

.. _xeus-cookiecutter: https://github.com/jupyter-xeus/xeus-cookiecutter
.. _execute_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#execute
.. _complete_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#completion
.. _inspect_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#introspection
.. _is_complete_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#code-completeness
.. _kernel_info_request: https://jupyter-client.readthedocs.io/en/stable/messaging.html#kernel-info
.. _MDN: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
.. _docs: https://jupyter-client.readthedocs.io/en/stable/messaging.html#kernel-info
.. _create_error_reply: https://github.com/jupyter-xeus/xeus/blob/7c6f3f61598b91a4e4a541a9ed7ba2033422af3a/include/xeus/xhelper.hpp#L33
.. _create_successful_reply: https://github.com/jupyter-xeus/xeus/blob/7c6f3f61598b91a4e4a541a9ed7ba2033422af3a/include/xeus/xhelper.hpp#L38
