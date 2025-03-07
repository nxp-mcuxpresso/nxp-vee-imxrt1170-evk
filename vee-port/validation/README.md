# VEE Port Test Suites

The [VEE Port Test Suites](https://docs.microej.com/en/latest/VEEPortingGuide/veePortQualification.html#vee-port-test-suite)
 validate the Abstraction Layer that implements the Low Level APIs of a Foundation Libraries by automatically running Java tests on the device.

## Requirements

The VEE Port Test Suites require a minimal configuration:

- A VEE Port built with the SDK 5 or 6.
- A device connected to the PC both for programming and to get output traces. See VEE Port specific documentation for setup. 
- If the VEE Port requires output traces redirection (e.g., when your device dumps the standard output on a serial port), use the
  [Serial to Socket Transmitter](https://docs.microej.com/en/latest/SDK6UserGuide/serialToSocketTransmitter.html) tool.

## Test Suites Usages

The Test Suites projects can be used in 2 different ways:

- as independent projects, pointing to an unexisting built VEE Port.
- as part of a VEE Port multi-project.

## Test Suite as Independent Project

The Test Suite projects available in this folder can be used as independent projects to test a VEE Port occasionally.
This requires to have the tested VEE Port already built.

### Project Configuration

A Test Suite Project requires configurations:

- In your prefered IDE, import the Test Suite you want to execute.
- Open the `build.gradle.kts` file located at the root of the project.
- In the `dependencies` block, set the absolute path of the VEE Port being tested.
  This can be the VEE Port archive file, or the parent folder of the `release.properties` file and 
  the VEE Port sources, see [Setup a Platform for Tests](https://docs.microej.com/en/latest/ApplicationDeveloperGuide/testsuite.html#setup-a-platform-for-tests).
- Configure the Test Suite, refer to the `Test Suite Configuration` section for more information.
  
### Execution
  
- Execute the `test` Gradle task on the Test Suite Project either in Command Line or via the IDE).
- Please refer to [VEE Port Qualification documentation](https://docs.microej.com/en/latest/VEEPortingGuide/veePortQualification.html) 
  to know more about Test Suites and how to get reports.

## Test Suite as part of a VEE Port Multi-Project

The Test Suite projects available in this folder can be integrated in a VEE Port project to test it continuously.
This means the Test Suites are executed as part of the build, so the build of the VEE Port is sucessful only if all the Test Suites pass.

This is the recommended way to run the Test Suites, especially when using a Continuous Integration environment.
This makes sure the Test Suites are executed after each changes, which reduces the risk of regressions.

### Project Configuration

In your VEE Port multi-project:

- copy/paste the projects of the Test Suites you want to execute to validate it.
  The convention is to create a folder `validation` which contains the Test Suite projects, but any other orgnization is possible.
- update the `settings.gradle.kts` file of the multi-project to include the Test Suite projects.
  For example, if the 2 Test Suite projects for Core and FS have been copied in the `validation` folder of the VEE Port multi-project::

    ```
    rootProject.name = "my-veeport"
    include("vee-port", "front-panel")

    include("vee-port:validation:core")
    include("vee-port:validation:fs")
    ```
  
Then for each added Tests Suite project:

- Open the `build.gradle.kts` file of the Test Suite project.
- In the `dependencies` block, add the dependency to the VEE Port with a project dependency:

    ```
    dependencies {
      microejVee(project(":vee-port"))
    }
    ```

- Configure the Test Suite, refer to the `Test Suite Configuration` section for more information.

### Execution

If the Test Suite projects are added in the VEE Port multi-project, you can execute the `build` Gradle task 
from the root project. This builds the VEE Port, then executes all the Test Suites:

```
./gradlew build
```

You can also execute each Test Suite individually by going in the Test Suite project and executing the `test` Gradle task::

```
./gradlew test
```

## Test Suite Configuration

A Test Suite project provides 2 configurations:

- The Test Suite global configuration. This includes the configuration of the Test Suite execution, 
  such as the log level, the number of retry for failed tests, ... Follow these steps to do the configuration: 
  
  - Duplicate the `config.properties` file located at the root of the project, and rename it to `config.properties`.
  - Open the `config.properties` file.
  - Fill the options marked as `[required]`, particularly:

    - [BSP Connection](https://docs.microej.com/en/latest/VEEPortingGuide/platformCreation.html#bsp-connection): check the VEE Port specific documentation to get the kind of configured BSP connection.
    - Trace Redirection (if the VEE Port requires output traces redirection): set options in sync with the Serial to Socket Transmitter options.

- The Tests configuration. This is the configuration of each test, which means all the [Application Options](https://docs.microej.com/en/latest/ApplicationDeveloperGuide/applicationOptions.html).
  They are declared in the `validation/microej-testsuite-common.properties` file. 
  By default, options are initialized with suitable values to successfully execute the whole Test Suite and should not be changed.
  However in some particular cases, you can adjust Test Suite specific options or memory settings.
  Please refer to the section named `Test Suite Properties` in the `README` of the specific Test Suite.

## Test Suite Source Code Navigation

The README, CHANGELOG and source code of each Test Suite is available online:

1. Download the desired Test Suite Module: For example [com.microej.pack.fs#fs-testsuite](https://repository.microej.com/modules/com/microej/pack/fs/fs-testsuite/) (refer to [VEE Port Qualification Test Suite Versioning](https://docs.microej.com/en/latest/VEEPortingGuide/veePortQualification.html#test-suite-versioning) to determine the Test Suite module version).
2. Unzip the Test Suite `RIP` (it is a zip file): For example `fs-testsuite-[version].rip`.
3. Open the Java archive `JAR` (it is a zip file): For example `/content/javaLibs/fs-testsuite-[version].jar/`.
4. Open the desired test: For example `/com/microej/fs/tests/constructors/TestFileInputStream.java`.

## Troubleshooting

The next sections describe the typical issues you may encounter when executing a VEE Port Test Suite.

### No activity on input stream

The following error indicates that no output have been seen by the `Serial To Socket` tool.

- Ensure that the COM port configured for the `Serial To Socket` tool is correct.
- Refers to the documentation of your VEE Port for how to connect the output traces to the Test Suite engine.

    ```
    [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] 1 : TraceAnalyzer ERROR :
    [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] [M3] - No activity on input stream since 75 s.
    [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] 
    [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] 2 : TraceAnalyzer ERROR :
    [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] [M5] - No encountered success/failure tag.
    [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] 
    [testsuite:javaTestsuite] BUILD FAILED
    [testsuite:javaTestsuite] C:\Program Files\MicroEJ\MicroEJ-SDK-20.12\rcp\configuration\org.eclipse.osgi\11\data\repositories\microej-build-repository\com\is2t\easyant\plugins\microej-testsuite\3.4.0\microej-testsuite-harness-jpf-emb-3.4.0.xml:85: TraceAnalyzer ends with errors.
    ```

### Could not open port 'COMxxx'

The following error indicates that the COM port can not be opened:

```
serial.serialutil.SerialException: could not open port 'COM7': FileNotFoundError(2, 'The system cannot find the file specified.', None, 2)
```

- Ensure that the COM port configured for the `Serial To Socket` tool is correct.
- Only one application may open a given COM port. Ensure no other application is using this COM port.
- COM port may changed when the device is unplugged. 
  Ensure that the COM port configured for the `Serial To Socket` tool is correct.

### No loaded VEE Port

```
  * What went wrong:
  Execution failed for task ':loadVee'.
  > No VEE defined.
    Possible options to load a VEE are:
    - (1) Declare a Module dependency with the `microejVee` configuration (e.g. `microejVee("com.mycompany:myvee:M.m.p")').
    - (2) Define a local VEE directory as File dependency with the `microejVee` configuration (e.g. `microejVee(files("path/to/vee/source"))').  - (3) Define a local VEE archive as File dependency with the `microejVee` configuration (e.g. `microejVee(files("path/to/vee/file.zip"))').
```

Ensure the VEE Port is defined in the ``dependencies`` block of the ``build.gradle.kts`` file 
and points to the VEE Port archive or to the VEE Port ``source`` folder.

### Could not connect to localhost:5555

```
   [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] 1 : SocketTraceAnalyzerError ERROR :
   [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] [M1] - Could not connect to localhost:5555
   [testsuite:javaTestsuite] [traceAnalyzer:socketTraceAnalyzer] 
   [testsuite:javaTestsuite] BUILD FAILED
```

- Ensure the `Serial To Socket` tool is started and configured to listen on
  the port 5555.  The host and port properties used by the Test Suite
  Engine are configured in `config.properties`.

### The Test Suite runs but some tests are failing

- Ensure the Low Level Implementations tested by the Test Suite are correctly implemented.
- Check your VEE Port documentation on Test Suites. The VEE Port may already
  provide specific configuration (`config.properties` and
  `microej-testsuite-common.properties`) with some test filters (limitations or known issues).
