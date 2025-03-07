# Overview

This example shows a simple MicroUI animation and how to use [MicroEJ Native Interfaces Mechanisms](https://docs.microej.com/en/latest/VEEPortingGuide/native.html).

# Usage

* A rectangle animation is running indefinitely on the screen.
* The rectangle positions are retrieved by methods implemented with native code using [Native Interfaces Mechanisms](https://docs.microej.com/en/latest/VEEPortingGuide/native.html).
  * This has been done for demonstration's sake.
  * Non-implemented Java native methode are stored inside [SimpleGFXNatives.java](src/main/java/com/nxp/example/simplegfx/SimpleGFXNatives.java).
  * For the Simulator, native methods are implemented in a VEE Port mock with same package, class, name and parameters (see [SimpleGFXNatives.java](../../vee-port/mock/src/main/java/com/nxp/example/simplegfx/SimpleGFXNatives.java)).
  * For the Device, native methods are implemented in C following the [naming convention](https://docs.microej.com/en/latest/VEEPortingGuide/sni.html#naming-convention) (see [simple_gfx_app_imp.c](../../bsp/vee/src/bsp/simple_gfx_app_imp.c)).
  * For more information about native methods, please have a look at MicroEJ [Managed Code Communication training](https://docs.microej.com/en/latest/Trainings/trainingCManagedCodeCommunication.html).

## Run on simulator

In Visual Studio Code:
- Open the Gradle tool window by clicking on the elephant icon on the left side,
- Expand the tasks of the chosen demo project,
- Double-click on the `microej` > `runOnSimulator` task,
- The application starts, the traces are visible in the Run view.

Alternative ways to run in simulation are described in the [Run on Simulator](https://docs.microej.com/en/latest/SDK6UserGuide/runOnSimulator.html) documentation.

## Run on device

Make sure to properly setup the VEE Port environment before going further.
Refer to the [VEE Port README](../../README.md) for more information.

In Visual Studio Code:
- Open the Gradle tool window by clicking on the elephant icon on the left side,
- Expand the tasks of the chosen demo project,
- Double-click on the `microej` > `runOnDevice` task,
- The device is flashed. Use the appropriate tool to retrieve the execution traces.

Alternative ways to run on device are described in the [Run on Device](https://docs.microej.com/en/latest/SDK6UserGuide/runOnDevice.html) documentation.

# Dependencies

_All dependencies are retrieved transitively by Gradle._

# Source

N/A

# Restrictions

None.
 
---  
_Markdown_   
_Copyright 2025 MicroEJ Corp. All rights reserved._  
_Use of this source code is governed by a BSD-style license that can be found with this software._
