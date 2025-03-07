# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Relationship

See [UI Readme](README.md) to have more information about the relationship between the version of the test suite and the UI Pack.

## [14.2.0] - 2024-11-18

### Changed

- UI Test Suite 14.2.0 that tests the UI Pack 14.2.0 (see [UI Readme](README.md)).
- Move the test ``MicroejUiValidation`` in the UI testsuite jar (and remove the nature ``java`` of this project).
- Gather all GPU tests in the package ``/com/microej/microui/test/gpu``.
- Update the [UI Readme](README.md).

## [14.1.1] - 2024-10-17

### Changed
 
- Align the version numbering to match the version number the UI Pack.
- UI Test Suite 14.1.1 that tests the UI Pack 14.1.1 (see [UI Readme](README.md)).

## [1.8.0] - 2024-10-03

### Changed

- UI Test Suite 1.8.1 that includes  a bench to check the VEE Port evolution (see [UI Readme](README.md)).

### Fixed

- Fix the comments of MicroejUiValidation to be compatible with UI Pack 14.0.0.
- Fix the test `testFlushTime()`: when a GPU is used to fill a rectangle, the time measured for the flush included the time to wait the end of fill rectangle.

## [1.7.0] - 2023-02-14

### Changed

- UI Test Suite 1.7.0 (compatibility with UI Pack 14.0.0).

### Removed

- Remove ``testBackBufferRestore``: no meaning since UI Pack 14.0.0.

## [1.4.0] - 2023-12-05

### Added

- Relationship table between the version of the test suite and the UI Pack in [UI Readme](README.md).

### Changed

- UI Test Suite 1.4.0.

## [1.2.0] - 2023-02-08

### Changed

- UI Test Suite 1.2.0.

## [1.1.0] - 2022-07-23

### Added

- Add test `testFlushTime`.
- Add UI Test Suite dependency.

### Changed

- Reformat to use standard MicroEJ Test Suite flow.
- Update the test ``testBackBufferRestore`` to be compliant with round displays and displays with specific pixels on corners.

## [1.0.0] - 2021-04-20

### Added

-  Tearing tests: show LCD tearing effect with full screen and black band.
-  Drawing time: retrieve the maximum drawing time to have the better framerate.
