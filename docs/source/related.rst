.. Copyright (c) 2017, Johan Mabille, Loic Gouarin and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Related projects
================

Xeus-based kernels
------------------

xeus-cling
~~~~~~~~~~

The `xeus-cling`_ project is a Jupyter kernel for the C++ programming language
based on the Cling C++ interpreter from CERN and Xeus, the native
implementation of the Jupyter protocol.

.. image:: xeus-cling-screenshot.png
   :alt: C++ notebook

xeus-python
~~~~~~~~~~~

The `xeus-python`_ project is a Jupyter kernel for the Python programming
language based on the Xeus implementation of the protocol.

.. image:: xeus-python-screencast.gif
   :alt: xeus-python screencast

xeus-sql
~~~~~~~~

The `xeus-sql`_ is a Jupyter kernel for general SQL implementations based on the native implementation of the Jupyter protocol xeus and SOCI.

 .. image:: xeus-sql-screencast.gif
    :alt: xeus-sql screencast

xeus-sqlite
~~~~~~~~~~~

The `xeus-sqlite`_ project is a Jupyter kernel for the SQLite.

.. image:: xeus-sqlite-screenshot.png
   :alt: xeus-sqlite screenshot

xeus-robot
~~~~~~~~~~

The `xeus-robot`_ project is a Jupyter kernel for RobotFramework

xeus-lua
~~~~~~~~

The `xeus-lua`_ project is a Jupyter kernel for Lua.

xeus-octave
~~~~~~~~~~~

The `xeus-octave`_ project is a Jupyter kernel for GNU Octave.

xeus-wren
~~~~~~~~~

The `xeus-wren`_ project is a Jupyter kernel for Wren.

xeus-cookiecutter
-----------------

The `xeus-cookiecutter`_ project can be used to generated xeus-based Jupyter kernels. It provides a base template project including an "hello world" kernel. 

SlicerJupyter
-------------

The SlicerJupyter_ project is an integration of the xeus-python kernel in the 3D-Slicer desktop application. SlicerJupyter is an example of application
taking advantage of xeus to integrate a Jupyter kernel in the event loop of a Desktop application.

xwidgets
---------

The xwidgets_ project is a C++ implementation of the Jupyter interactive
widgets protocol. The Python reference implementation is available in the
ipywidgets ipywidgets_ project.

xwidgets enables the use of the Jupyter interactive widgets in the C++
notebook, powered by the xeus-cling kernel and the cling C++ interpreter from
CERN. xwidgets can also be used to create applications making use of the
Jupyter interactive widgets without the C++ kernel per se.

.. image:: xwidgets-screencast.gif
   :alt: xwidgets screencast

.. _xeus-cling: https://github.com/jupyter-xeus/xeus-cling
.. _xeus-python: https://github.com/jupyter-xeus/xeus-python
.. _xeus-sqlite: https://github.com/jupyter-xeus/xeus-sqlite
.. _xeus-sql: https://github.com/jupyter-xeus/xeus-sql
.. _xeus-robot: https://github.com/jupyter-xeus/xeus-robot
.. _xeus-lua: https://github.com/jupyter-xeus/xeus-lua
.. _xeus-wren: https://github.com/jupyter-xeus/xeus-wren
.. _xeus-octave: https://github.com/jupyter-xeus/xeus-octave
.. _xeus-cookiecutter: https://github.com/jupyter-xeus/xeus-cookiecutter
.. _SlicerJupyter: https://github.com/Slicer/SlicerJupyter
.. _xwidgets: https://github.com/QuantStack/xwidgets
.. _ipywidgets: https://github.com/jupyter-widgets/ipywidgets
