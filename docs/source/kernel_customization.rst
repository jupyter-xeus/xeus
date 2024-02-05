.. Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Customizing the kernel
======================

While it is possible to create a kernel by focusing on the implementation of the interpreter,
``xeus`` also offers the possibility to customize some predefined behaviors. This can be done
via additional arguments of the ``xkernel`` constructor:

.. code::

    xkernel(const xconfiguration& config,
            const std::string& user_name,
            interpreter_ptr interpreter,
            history_manager_ptr history_manager = make_in_memory_history_manager(),
            logger_ptr logger = nullptr,
            server_builder sbuilder = make_xserver,
            debugger_builder dbuilder = make_null_debugger);

History manager
---------------

The ``xhistory_manager`` class is used to store the ``execute_request`` messages sent by the
frontend. Typical usage is when the console client connects to a kernel that has already executed
some code: it asks the ``history_manager`` for its records and prints them so that the user knows
what happened before.

``xeus`` provides a default implementation of ``xhistory_manager`` that stores the messages in
memory. It is possible to provide a different history manager by defining a class that inherits
from ``xhistory_manager`` and implements the abstract methods:

.. code::

    class file_history_manager : public xeus::xhistory_manager
    {
    public:

        file_history_manager(const std::string& file_name);
        virtual ~file_history_manager();

    private:

        
        void configure_impl() override;
        void store_inputs_impl(int line_num, const std::string& input) override;
        nl::json get_tail_impl(int n, bool raw, bool output) const override;
        nl::json get_range_impl(int session, int start, int stop, bool raw, bool output) const override;
        nl::json search_impl(const std::string& pattern, bool raw, bool output, int n, bool unique) const override;
    };

Then simply pass an instance to the kernel constructor:

.. code::

    int main(int argc, char* argv[])
    {
        // ....
        // Instantiates interpreter and config
        // ....
        auto hist = std::make_unique<file_history_manager>("my_history_file.txt");
        xeus::xkernel kernel(config,
                             xeus::get_user_name(),
                             interpreter,
                             std::move(hist));
        kernel.start();
        return 0;
    }

Logger
------

``xeus`` does not log anything by default. However, it can be useful during the development phase of a new
kernel to print the messages that are received by and sent from the kernel. Having a logger that can
be enabled on-demand is also useful to track bugs once your new kernel has been released.

``xeus`` provides a flexible logging mechanism that can be easily extended. Two default loggers are
available: one that logs to the console, and another one that logs to files. You can add your own
by defining a class that inherit from ``xlogger``. Three logging levels are provided, one for message type,
one for the content of the message and one for the full message. Loggers can be chained, meaning you can log
the message types to the console and the full messages into files:

.. code::

    int main(int argc, char* argv[])
    {
        // ....
        // Instantiates interpreter and config
        // ....
        auto logger = xeus::make_console_logger(xeus::xlogger::msg_type,
                                                xeus::make_file_logger(xeus::xlogger::full, "my_log_file.log"));
        xeus::xkernel kernel(config,
                             xeus::get_user_name(),
                             interpreter,
                             xeus::make_in_memory_history_manager(),
                             std::move(logger));
        kernel.start();
        return 0;

    }

To turn on logging, you need to define the variable environment ``XEUS_LOG`` before starting the kernel. This way,
enabling and disabling the logs do not require to rebuild the kernel.

Defining a new type of logger is as simple as defining a new type of history manager: inherit from ``xlogger``
and implement the abstract methods:

.. code::

    class my_logger : public xeus::xlogger
    {
    public:

        my_logger();
        virtual ~mylogger();

    private:

        void log_received_message_impl(const xmessage& message, channel c) const override;
        void log_sent_message_impl(const xmessage& message, channel c) const override;
        void log_iopub_message_impl(const xpub_message& message) const override;

        void log_message_impl(const std::string& socket_info,
                                      const nl::json& header,
                                      const nl::json& parent_header,
                                      const nl::json& metadata,
                                      const nl::json& content) const override;
    };

Server
------

The server is the middleware component responsible for sending and receiving the messages. While you will hardly
have to implement your own, you might need to specify a different server that the default one. The core library
``xeus`` only provides the interface for the server, implementations are provided by ``xeus-zmq`` and ``xeus-lite``.

``xeus-zmq`` actually provides three different implementations for the server:

- ``xserver_zmq`` is the default server implementation, it runs three thread, one for publishing, one for the
  heartbeat messages, and the main thread handles the shell, control and stdin sockets.
- ``xserver_control_main`` runs an additional thread for handling the shell and the stdin sockets. Therefore the main
  thread only listens to the control socket. This allow to easily implement interruption of code execution. This
  server is required if you want to plug a debugger in the kernel.
- ``xserver_shell_main`` is similar to ``xserver_control_main`` except that the main thread handles the shell and
  the stdin sockets while the additional thread listens to the control socket. This server is required if you want to
  plug a debugger that does not support native threads and requires the code to be run by the main thread.

``xeus-lite`` provides a default implementation only.
