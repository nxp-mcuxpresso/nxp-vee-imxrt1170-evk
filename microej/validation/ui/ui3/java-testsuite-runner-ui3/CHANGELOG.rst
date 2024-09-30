CHANGELOG
=========

The format is based on `Keep a
Changelog <https://keepachangelog.com/en/1.0.0/>`__, and this project
adheres to `Semantic
Versioning <https://semver.org/spec/v2.0.0.html>`__.

Relationship
------------

See `UI Readme <README.rst>`_ to have more information about the relationship between the version of the test suite and the UI Pack.

.. _180---Unreleased:

[1.8.0] - Unreleased
--------------------

Changed
~~~~~~~

- UI Test Suite 1.8.1 that includes  a bench to check the VEE Port evolution (see `UI Readme <README.rst>`_).

Fixed
~~~~~

- Fix the comments of MicroejUiValidation to be compatible with UI Pack 14.0.0.

.. _170---2024-02-14:

[1.7.0] - 2023-02-14
--------------------

Changed
~~~~~~~

- UI Test Suite 1.7.0 (compatibility with UI Pack 14.0.0).

Removed
~~~~~~~

- Remove ``testBackBufferRestore``: no meaning since UI Pack 14.0.0.

.. _140---2023-12-05:

[1.4.0] - 2023-12-05
--------------------

Added
~~~~~

- Relationship table between the version of the test suite and the UI Pack in `UI Readme <README.rst>`_.

Changed
~~~~~~~

- UI Test Suite 1.4.0.

.. _120---2023-02-08:

[1.2.0] - 2023-02-08
--------------------

Changed
~~~~~~~

- UI Test Suite 1.2.0.

.. _110---2022-07-23:

[1.1.0] - 2022-07-23
--------------------

Added
~~~~~

- Add test `testFlushTime`.
- Add UI Test Suite dependency.

Changed
~~~~~~~

- Reformat to use standard MicroEJ Test Suite flow.
- Update the test ``testBackBufferRestore`` to be compliant with round displays and displays with specific pixels on corners.

.. _100---2021-04-20:

[1.0.0] - 2021-04-20
--------------------

Added
~~~~~

-  Tearing tests: show LCD tearing effect with full screen and black band.
-  Drawing time: retrieve the maximum drawing time to have the better framerate.

..
    Copyright 2021-2024 MicroEJ Corp. All rights reserved.
    Use of this source code is governed by a BSD-style license that can be found with this software.
