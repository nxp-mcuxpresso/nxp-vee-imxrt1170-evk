# Event Queue Test Suite

This folder contains a ready-to-use project for testing [Event Queue](https://docs.microej.com/en/latest/VEEPortingGuide/packEventQueue.html) implementations on a device.
This Test Suite will create some listeners and send basic/extended events with different primitive types of data.

## Specifications

- Tested Foundation Library: [Event Queue](https://forge.microej.com/artifactory/microej-developer-repository-release/ej/api/event/)
- Test Suite Module: [ej.api.event#event-testsuite](https://forge.microej.com/artifactory/microej-developer-repository-release/com/microej/pack/event/event-testsuite//)

Set the Event Queue Test Suite module version in the [build.gradle.kts](build.gradle.kts).

Please refer to [VEE Port Qualification Test Suite Versioning](https://docs.microej.com/en/latest/VEEPortingGuide/veePortQualification.html#test-suite-versioning)
to determine the Event Queue Test Suite module version.

## Requirements

- Read VEE Port Test Suites [documentation](../README.md).
- Read External resource loader Test Suites [documentation](../ext-res-loader/README.md).

## Usage

- Add the content of [src/test/c](src/test/c) folder in your BSP. Add all files of [src](src/test/c/src) folder as source files, and [inc](src/test/c/inc) folder as include path.
- Follow the configuration and execution steps described in VEE Port Test Suites [documentation](../README.md).

## Test Suite Source Code Navigation

- See VEE Port Test Suites [documentation](../README.md).

## Troubleshooting

- This testsuite does not work automatically in Simulation. You have to manually stop the Application by closing the Front panel.
- See Vee Port Test Suites [documentation](../README.md).
