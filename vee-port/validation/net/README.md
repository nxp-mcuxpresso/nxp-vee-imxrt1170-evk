# Network Core Engine Test Suite

This folder contains a ready-to-use project for testing the [Network Core Engine](https://docs.microej.com/en/latest/VEEPortingGuide/networkCoreEngine.html) implementation on a device.
This Test Suite will typically create TCP sockets, UDP sockets, test client side and server side...

## Specifications

- Tested Foundation Library: [NET](https://repository.microej.com/modules/ej/api/net/)
- Test Suite Module:  [com.microej.pack.net#net-1_1-testsuite](https://repository.microej.com/modules/com/microej/pack/net/net-1_1-testsuite/)

Update the NET Test Suite module version in the [build.gradle.kts](net/build.gradle.kts) to match the requirement of the tested VEE Port.

Please refer to [VEE Port Qualification Test Suite Versioning](https://docs.microej.com/en/latest/VEEPortingGuide/veePortQualification.html#test-suite-versioning)
to determine the Network Core Engine Test Suite module version.

## Requirements

- See VEE Port Test Suites [documentation](../README.md).

## Usage

- Follow the configuration and execution steps described in VEE Port Test Suites [documentation](../README.md).

## Test Suite Properties

The NET Test Suite requires specific properties depending on the implementation and test environment.
All the properties described in this section are defined in the file [microej-testsuite-common.properties](validation/microej-testsuite-common.properties)
and prefixed by `microej.java.property.`.

Common properties:

- `testsuite.preferipv6` (optional, false by default): set to true if the tests must use IPv6 by default.
- To use the service for the WiFi initialization add the following dependency in your `build.gradle.kts`: `implementation("com.microej.pack.net:net-1_1-testsuite-wifi:2.1.0")`.

  - WiFi SSID and passphrase are set with respectively with the properties `wifi.ssid` and `wifi.passphrase`.

Properties to define if `testsuite.preferipv6` is `false`:

- `wrong.machine.ipv4`: An invalid IPv4 address on the local network
- `remote.machine.ipv4`: IPv4 address of the testsuite server
- `netif.ipv4.name`: network interface name with an IPv4

Properties to define if `testsuite.preferipv6` is `true`:

- `wrong.machine.ipv6`: An invalid IPv6 address on the local network
- `remote.machine.ipv6`: IPv6 address of the testsuite server
- `netif.ipv6.name`: network interface name with an IPv6

When testing a device with a dual-stack IPv6+IPv4, the following properties must be set:

- `remote.machine.ipv4`: see above
- `netif.ipv4.name`: see above
- `remote.machine.ipv6`: see above
- `netif.ipv6.name`: see above

## OpenJDK Tests

This Test Suite runs network tests from the OpenJDK project. However some of these tests need the localhost network interface to run.
If you VEE Port or implementation does not provide this interface you can skip the OpenJDK tests.
To do so, add the pattern `**/openjdk/**/*.class` to the property `test.run.excludes.pattern` in your file `config.properties`.

## Test Suite Source Code Navigation

See VEE Port Test Suites [documentation](../README.md).

## Troubleshooting

See VEE Port Test Suites [documentation](../README.md).

### Debug Test Suite Server Application

The Test Suite runs a server application for which logs are only accessible via a TCP socket.
In order to display the logs, the following steps are required:

- Specify the log server address with the property `remoteapp.logging.host` (by default `localhost`) in `config.properties` file.
- Specify the log server port with the property `remoteapp.logging.port` (set it to `0` to disable the remote log feature ; this is the default value) in `config.properties` file.
- Start a log server. You can use netcat: `nc -l <port>` (where `<port>` is the port specified in the previous property).
