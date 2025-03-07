# SSL Test Suite

This folder contains a ready-to-use project for testing [Secure Socket Layer](https://docs.microej.com/en/latest/VEEPortingGuide/ssl.html) implementations on a device.
This Test Suite will typically test secure sockets using SSL/TLS protocol.

## Specifications

- Tested Foundation Library: [SSL](https://repository.microej.com/modules/ej/api/ssl/)
- Test Suite Module: [com.microej.pack.net#net-ssl-2_2-testsuite](https://repository.microej.com/modules/com/microej/pack/net/net-ssl-2_2-testsuite/)

Update the SSL Test Suite module version in the [build.gradle.kts](build.gradle.kts) to match the requirement of the tested VEE Port.

Please refer to [VEE Port Qualification Test Suite Versioning](https://docs.microej.com/en/latest/VEEPortingGuide/veePortQualification.html#test-suite-versioning)
to determine the SSL Test Suite module version.

## Requirements

- See VEE Port Test Suites [documentation](../README.md).

## Usage

- Follow the configuration and execution steps described in VEE Port Test Suites [documentation](../README.md).

## Test Suite Properties

The NET Test Suite requires specific properties depending on the implementation and test environment.
All the properties described in this section are defined in the file [microej-testsuite-common.properties](validation/microej-testsuite-common.properties)
and prefixed by `microej.java.property.`.

- WiFi SSID and passphrase are set with respectively with the properties `wifi.ssid` and `wifi.passphrase`.

- Application options specific to SSL:

  - `remote.machine.ip`: Set this property with the value of the IP address of the machine that runs the tests.
  - `remoteapp.timeout`: Amount of time (in milliseconds) that the remote test will wait before interrupting its execution.
  - `microej.ssl.testsuite.ssl.stack`: Property to set to `mbedtls` when running tests on VEE Ports with a mbedTLS backend.

## Test Suite Source Code Navigation

See VEE Port Test Suites [documentation](../README.md).

## Troubleshooting

See VEE Port Test Suites [documentation](../README.md).
