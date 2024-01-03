..
    Copyright 2023 MicroEJ Corp. All rights reserved.
    Use of this source code is governed by a BSD-style license that can be found with this software.
..

*************
VG Test Suite
*************

This folder contains a ready-to-use project for testing `MicroVG <https://docs.microej.com/en/latest/VEEPortingGuide/vg.html>`__ implementations on a device.
This Test Suite will test vector drawing capabilites.

Specifications
--------------

- Tested Foundation Library: `MicroVG <https://repository.microej.com/modules/ej/api/microvg/>`__
- Test Suite Module: `com.microej.pack.vg#vg-testsuite <https://repository.microej.com/modules/com/microej/pack/vg/>`__

Set the VG Test Suite module version in the `module.ivy
<java-testsuite-runner-vg/module.ivy>`__ to match the pack version of the VEE Port
tested.

Please refer to `VEE Port Qualification Test Suite Versioning
<https://docs.microej.com/en/latest/PlatformDeveloperGuide/platformQualification.html#test-suite-versioning>`__
to determine the MicroVG Test Suite module version.

Requirements
-------------

See VEE Port Test Suites `documentation <../README.rst>`__.

Relationship
------------

The test suite evolves as the VG Pack evolves: bug fixes, new features, etc. 
A test suite is, therefore, compatible with a given range of VG Packs (cf VG Pack changelog: https://docs.microej.com/en/latest/VEEPortingGuide/vgChangeLog.html). 
The following table shows the relationship between the different versions of the test suite and the associated VG Packs. 
(note: Each VG Pack implements a version of MicroVG; cf VG Pack release notes: https://docs.microej.com/en/latest/VEEPortingGuide/vgReleaseNotes.html). 

The table indicates:

* the version of the project ``java-testsuite-runner-vg``,
* the version of the test suite library fetched by the project ``java-testsuite-runner-vg``,
* the compatible VG Packs to run the tests of the test suite,
* the minimum version of the MicroVG API required by the test suite. 

+-----------------+-------------------+---------------+-------------------+
| Project version | TestSuite version | VG Pack Range | MicroVG API Range |
+=================+===================+===============+===================+
| 1.1.0           | 2.4.0             | [1.4.1-2.0.0[ | [1.4.0-2.0.0[     |
+-----------------+-------------------+---------------+-------------------+
| 1.0.0           | 2.2.0             | [1.3.0-2.0.0[ | [1.3.0-2.0.0[     |
+-----------------+-------------------+---------------+-------------------+

Usage
-----

To run the testsuite on i.MXRT1170 VEE Port, you need to activate the back-copy inside the MicroUI implementation.
By default back-copy is disabled on this VEE Port to increase the UI performances.

Uncomment the following line located in `display_dma.h <../../../nxpvee-mimxrt1170-evk-bsp/projects/microej/ui/display_dma.h>`_:

::

   #define DISPLAY_DMA_ENABLED       ( FRAME_BUFFER_COUNT > 1 )


- In MicroEJ SDK, import the ``java-testsuite-runner-vg`` project in your workspace.
- Follow the configuration and execution steps described in VEE Port Test Suites `documentation <../README.rst>`__.

Test Suite Source Code Navigation
---------------------------------

See VEE Port Test Suites `documentation <../README.rst>`__.

Troubleshooting
---------------

See VEE Port Test Suites `documentation <../README.rst>`__.
