.. 
    Copyright 2022-2023 MicroEJ Corp. All rights reserved.
    Use of this source code is governed by a BSD-style license that can be found with this software.

.. |BOARD_NAME| replace:: MIMXRT1170
.. |PLATFORM| replace:: NXP VEE MIMXRT1170
.. |RTOS| replace:: FreeRTOS

.. _README: ./../../../README.md

================
|BOARD_NAME| BSP
================

This project contains the BSP sources of the |PLATFORM| for the
|BOARD_NAME|.

This document does not describe how to setup the |PLATFORM|. Please
refer to the `README`_ for that.

Build & Run Scripts
---------------------

In the directory ``scripts/`` are scripts that can be
used to build and flash the BSP.  The ``.bat`` and ``.sh`` scripts are
meant to run in a Windows and Linux environment respectively.

- The ``build*`` scripts are used to compile and link the BSP with a
  MicroEJ Application to produce a MicroEJ Firmware
  (``microej.bin``) that can be flashed on a device.

  The ``build*`` scripts work out of the box, assuming the toolchain is
  installed in the default path.

- The ``run*`` scripts are used to flash a MicroEJ Firmware
  (``microej.bin``) on a device.

The following environment variables are customizable:  

- ``ARMGCC_DIR``: Installation directory for the GCC toolchain.
  Default value: ``C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10``.

The environment variables can be defined globally by the user or in
the ``set_project_env*`` scripts.  When the ``.bat`` (``.sh``) scripts
are executed, the ``set_project_env.bat`` (``set_project_env.sh``) script
is executed if it exists.  Create and configure these files to
customize the environment locally.  Template files are provided as
example, see ``set_project_env.bat.tpl`` and ``set_project_env.sh.tpl``.

Customize the BSP
-----------------

Configuration files for this BSP are located in ``nxpvee-mimxrt1170-evk\nxpvee-mimxrt1170-evk-bsp\projects\nxpvee-ui\bsp`` folder.
Compilation flags are defined in ``nxpvee-mimxrt1170-evk\nxpvee-mimxrt1170-evk-bsp\projects\nxpvee-ui\armgcc\flags.cmake``.
CMake modules used by this BSP are declared in ``nxpvee-mimxrt1170-evk\nxpvee-mimxrt1170-evk-bsp\projects\nxpvee-ui\armgcc\CMakeLists.txt``.
Linker script is located here ``nxpvee-mimxrt1170-evk\nxpvee-mimxrt1170-evk-bsp\projects\nxpvee-ui\armgcc\MIMXRT1176xxxxx_cm7_flexspi_nor_sdram.ld``.

Flash the Board
---------------

The ``run*`` scripts can also be used to flash the device with the
MicroEJ Firmware.

System Priorities
-----------------

FreeRTOS is configured with 10 priorities.

The tasks are assigned with the following priorities (higher numerical value has a higher priority):

.. list-table::
   :header-rows: 1

   * - Priority
     - Functionality
   * - 0
     - Idle task
   * - 1
     - DHCP
   * - 2
     - UART
   * - 4
     - MicroEJ
   * - 5
     - Touch screen task, display task
   * - 6
     - FS
   * - 7
     - FreeRTOS timer
   * - 8
     - TCP/IP
