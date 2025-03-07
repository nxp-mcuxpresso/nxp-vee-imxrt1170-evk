# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

### [3.4.0] - 2024-12-13

### Added

- Add check for ``LLBSP_IMPL_atomic_exchange``.
- Add check for floating-point NaN bit pattern.
- Add check for ``LLMJVM_IMPL_getTimeNanos``
- Add check that application time modification does not impact monotonic time.

### Changed

- Remove invalid test that was comparing monotonic time and application time.

## [3.3.0] - 2024-10-03

### Changed

- Add property ``com.microej.core.tests.can.set.system.time`` allowing to skip checks on application time setting in test ``testMonotonicTime``.
- Switch to Util.platformTimeMillis() in test testTime to avoid shifting issues
- Update the default pattern that prevents a test from being re-executed on failure.
  The new string is ``MicroEJ START``, which is the one printed in the latest VEE Ports (formerly ``VM start``).
  See ``microej.testsuite.retry.unless`` option in ``config.properties.tpl`` files.

## [3.2.0] - 2023-06-14

### Added

- Add check for floating-point arithmetic.
- Add check for floating-point parse and toString with scientific notation.
- Add check for SNI native calling convention (ABI).

## [3.1.0] - 2022-07-23

### Added

- Add check for ``LLMJVM_IMPL_setApplicationTime``.
- Test Thread.sleep() with max number of milliseconds (Long.MAX_VALUE) does not cause an infinite loop in the MicroEJ Core due to time conversion overflow.
- Add check for ``LLMJVM_IMPL_getCurrentTime``, which should not go back in time.
- Add check for C malloc heap configuration which can be used when parsing float.
- Add check of ``LLMJVM_IMPL_scheduleRequest``, ``LLMJVM_IMPL_getCurrentTime``, and ``LLMJVM_IMPL_getTimeNanos`` precision

### Changed

- Split floating-point tests into 3 JUnit tests (FPU, parser, formatter)
  to allow for fine-grained ignored tests list.
- Reformat to use standard MicroEJ Test Suite flow.
- Add tolerance on floating-point parsing.

### Fixed

- Fix assertions to conform with JUnit semantic of expected/actual.

## [3.0.2] - 2021-04-14

### Fixed

- Ensure an error message is shown when the Round Robin test fails.

## [3.0.1] - 2021-02-12

### Fixed

- Removed a comment about ``testFloat`` and ``testDouble`` functions
  in ``MicroEJCoreValidation.java``.  This is documented in the README
  of the project.

## [3.0.0] - 2020-06-02

### Changed

-  Previous test functions are now JUnit test cases

## [2.4.0] - 2019-12-18

### Added

-  New floating-point related tests: parse/toString for float/double

### Changed

-  Review round robin check test
-  Update license

## [2.3.0] - 2019-10-01

Initial revision with Changelog.

### Added

-  Property ``MJVMPortValidation.clock.seconds`` to configure the time
   in seconds of the visible clock test (default to 10s).

## [1.0.0] - 2017-09-29

### Added

- Initial revision

