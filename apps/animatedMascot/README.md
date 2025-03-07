# Overview

This example shows how to run a Vector Graphics animation.

# Usage

* An animated mascot animation is running indefinitely on the screen.
* A touch animation is launched when the user press the screen.
* The size of the touch animation depends on where the user touch the screen.

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
