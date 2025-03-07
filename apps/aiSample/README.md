# Overview

This example shows how to use NXP AI Foundation Library.

# Usage

* A Tensorflow Lite mode is loaded at startup. This is a quantized Cifarnet model (see [cifarnet_quant_int8.tflite](src/main/resources/model/cifarnet_quant_int8.tflite)).
* The application indefinitely runs inferences on the model with 10 example images located in [src/main/resources/images](src/main/resources/images).
* Every 2 seconds: 
  * An image is decoded and converted to the input tensor format.
  * An inference is launched on the model. 
  * The application gets the image name and the model confidence from the model output tensor.
  * The image, the label and the confidence are displayed on the screen.
  * The image path, the label and the confidence are logged into the serial port.

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
