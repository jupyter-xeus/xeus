.. Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Kernel file
===========

The ``kernel.json`` file is a ``json`` file used by Jupyter in order to retrieve all the available kernels.

It must be installed in the ``INSTALL_PREFIX/share/jupyter/kernels/my_kernel`` directory, we will see how to do that in the next chapter.

This ``json`` file contains:

    - ``display_name``: the name that the Jupyter client should display in its interface (e.g. on the main JupyterLab page).
    - ``argv``: the command that the Jupyter client needs to run in order to start the kernel. You should leave this value unchanged if you are not sure what you are doing.
    - ``language``: the target language of your kernel.

You can edit the ``kernel.json.in`` file as following. This file will be used by cmake for generating the actual ``kernel.json`` file which will be installed.

.. literalinclude:: my_kernel/share/jupyter/kernels/my_kernel/kernel.json.in
   :language: json
   :linenos:


.. note::
    You can provide logos that will be used by the Jupyter client. Those logos should be in files named ``logo-32x32.png`` and ``logo-64x64.png`` (``32x32`` and ``64x64`` being the size of the image in pixels), they should be placed next to the ``kernel.json.in`` file.
