# UI Test Suite

This folder contains a ready-to-use project for testing [MicroUI](https://docs.microej.com/en/latest/VEEPortingGuide/ui.html) implementations on a device.
This Test Suite will test the UI Port (implementation of LLAPI `LLUI_DISPLAY_IMPL`) and the drawing capabilities over a GPU.

**Note**: These tests only concern the VEE Ports made against the MicroEJ UI Packs 13.0.0 and higher.

Additionally, the [Tool-Java-Touch](https://github.com/MicroEJ/Tool-Java-Touch) project allows to test the correct behavior of MicroUI in a Java application.

## Specifications

- Tested Foundation Library: [MicroUI](https://repository.microej.com/modules/ej/api/microui/)
- Test Suite Module: [com.microej.pack.ui#ui-testsuite](https://repository.microej.com/modules/com/microej/pack/ui/ui-testsuite/)

Set the UI Test Suite module version in the [build.gradle.kts](build.gradle.kts) to match the pack version of the tested VEE Port.

The test suite evolves as the UI Pack evolves: bug fixes, new features, etc. 
A test suite is, therefore, compatible with a given range of UI Packs (cf UI Pack changelog: https://docs.microej.com/en/latest/VEEPortingGuide/uiChangeLog.html). 
The following table shows the relationship between the different versions of the test suite and the associated UI Packs. 

Notes: 

* Each UI Pack implements a version of MicroUI; cf UI Pack release notes: https://docs.microej.com/en/latest/VEEPortingGuide/uiReleaseNotes.html).
* Since UI Pack 14.1.1, the version numbering matches the version number the UI Pack. 

The table indicates:

* the version of the test suite project,
* the version of the test suite library fetched by the project,
* the compatible UI Packs to run the tests of the test suite,
* the minimum version of the MicroUI API required by the test suite.

| Project version  | TestSuite version        | UI Pack Range            | MicroUI API Range | Testsuite changes                                                      |
|------------------|--------------------------|--------------------------|-------------------|------------------------------------------------------------------------|
| 14.1.1 and above | Identical to the UI Pack | Identical to the UI Pack | [3.6.0-4.0.0[     |                                                                        |
| 1.8.0            | 1.8.1                    | [14.0.0-15.0.0[          | [3.5.0-4.0.0[     | Add a bench to check the VEE Port evolution                            |
| 1.7.0            | 1.7.0                    | [14.0.0-15.0.0[          | [3.5.0-4.0.0[     | Add Buffer Refresh Strategies tests                                    |
| 1.4.0            | 1.4.0                    | [13.7.0-14.0.0[          | [3.0.3-4.0.0[     | Add image formats with premultiplied alpha values                      |
| [1.3.0-1.3.1]    | 1.3.0                    | [13.4.1-14.0.0[          | [3.0.3-4.0.0[     | Allow setting the image heap size according to the display buffer size |
| 1.2.0            | 1.2.0                    | [13.4.1-14.0.0[          | [3.0.3-4.0.0[     | Add tests for circles and ellipses                                     |
| 1.1.0            | 1.1.0                    | [13.0.0-14.0.0[          | [3.0.3-4.0.0[     | Add tests for GPU ports                                                |
| 1.0.0            | 1.0.0                    | [13.0.0-14.0.0[          | [3.0.3-4.0.0[     | Initial version                                                        |

## Requirements

See VEE Port Test Suites [documentation](../README.md).

Skip Tests
----------

Some tests might be skipped depending on the GPU: the MicroUI testsuite checks all the MicroUI specification.
A GPU may not support some drawings, for instance the usage of some image formats.
As a consequence, the associated tests can be skippered to make the testsuite go faster.

This is an example of property to skip the test `EllipseReference.testCircles()`:

```
microej.testsuite.properties.microej.java.property.tests.EllipseReference.testCircles.skip=true
```

All *GPU* tests are gathered in the same package `com.microej.microui.test.gpu`, which can be excluded as a whole if the BSP does not provide a GPU:

```
excludeTestsMatching("com.microej.microui.test.gpu*")
```

## Usage

- Follow the configuration and execution steps described in VEE Port Test Suites [documentation](../README.md).

## UI Port Validation

### Introduction

The test `com.microej.microui.test.MicroejUiValidation` checks the minimal UI port over a BSP (and does not check the usage of a GPU).

### Tearing

A tearing effect (flickering) visible on LCD indicates a synchronization issue with the LCD tearing signal (TE).

* The "Full screen" test toggles black and white screens. If the flush job is properly synchronized and quite fast, the LCD appears gray.
  The LCD appearing cut in multiple parts indicates a synchronization issue of the flush job.

* The "Black band" test moves a black band on a white background.
  The band being cut in multiple parts indicates a synchronization issue of the flush job.

### Drawing Time

This test is useful when the implementation of `LLUI_DISPLAY_IMPL_flush` uses a buffer policy with a [SWAP mode](https://docs.microej.com/en/latest/VEEPortingGuide/uiDisplay.html#buffer-policy).
Tbe "Drawing time" test determinates the maximum drawing time (in milliseconds) to attain a given framerate.
The possible framerates depend on the LCD and on the [buffer refresh strategy](https://docs.microej.com/en/latest/VEEPortingGuide/uiBRS.html).
When the drawing time exceeds the maximum drawing time, the framerate is halved when the flush job is synchronized with the LCD tearing signal.

### Flush Time

The implementation of the function `LLUI_DISPLAY_IMPL_flush` must be as fast as possible: it is a non-blocking function (see the function specification).
The implementation has to launch a parallel task (be it a software task or a hardware process) to perform the operation and returns.
Once the task has finished, its end callback has to call the function `LLUI_DISPLAY_flushDone`.


This test checks that the implementation of `LLUI_DISPLAY_IMPL_flush` is non-blocking.


## Bench

### Introduction

The test `com.microej.microui.bench.RunBench` is useful to evaluate the performance of the evolution of a VEE Port.

The evolution can be:

* the update of the UI Pack,
* the use or not of a GPU,
* the update of a third-party library,
* the modification of the memory layout,
* etc.

The bench is divided in three main benchs:

* **Destination Display**: Perform several drawings in the display buffer.
* **Destination Image heap**: Perform several drawings in the image heap (using a MicroUI BufferedImage with the same dimension as the display).
* **Image Runtime Decoder**: Bench several runtime image decoders. 

Each test gives a score, with a smaller number indicating a faster performance.
This score can be used to evaluate a new iteration; the test will fail if the speed of the new iteration variates too much from the previous version (within a set tolerance).

Each bench can be customized, enabled or disabled.
See the chapter [Configuration: Global Options](#configuration-global-options).

To launch the test `com.microej.microui.bench.RunBench`, see the chapter [Run the Bench](#run-the-bench).

### Benches *Destination*

The benches *Destination* perform exactly the same drawings in the display buffer and in the image heap. 
It is useful to compare the impact of the location of the buffers.

The drawings consist in:

* drawing the full screen area with a *fill rectangle* and with a *draw image*,
* performing several aliased drawings (line, circle, etc.),
* performing several anti-aliased drawings,
* drawing text with several font sizes and BPP,
* drawing images (several RAW formats, transparent or opaque), with or witout a global opacity,
* drawing flipped images (same conditions),
* drawing images with a rotation (same conditions),
* drawing images with a scaling (same conditions).

Each sub-bench also gives a score, but this score is not compared with the score of a previous iteration.
These sub-scores can be useful to understand why a *Destination* score has changed between two iterations.
By default, the sub-scores are not printed (see the configuration *verbose* to display them).

Each sub-bench can be customized, enabled or disabled.
See the chapter [Configuration: Destination](#configuration-destination).

### Bench *Runtime Image Decoder*

This bench decodes opaque and transparent images for a given runtime image decoder (for instance, a PNG decoder).

Each image is decoded to several output formats:

* default format: the format privileged by the decoder itself,
* all MicroUI OutputFormats.

This allows to evaluate the impact of a potential post-decoding step to adapt the user expected output format with the decoder output format (if the runtime decoder cannot decode directly in the user expected output format).
 
The list of decoders and the list of output formats can be customized. See the chapter [Configuration: Decoders](#configuration-decoders).

### Configuration File

The bench has been designed to avoid changing the source code to configure it.
It uses several [system properties](https://docs.microej.com/en/latest/ApplicationDeveloperGuide/classpath.html#system-properties) listed in the properties.list file `/com/microej/microui/bench/_AllTests_RunBench.properties.list`.

Change the values in this file, or add global properties when launching the application with the `-D` option to use your own value.
All properties **must** be prefixed with `microej.java.property.` to be visibe to application.
For example, `microej.java.property.ej.microui.bench.display=true` will enable benching the display.


### Configuration: Global Options

To enable or disable a bench, set the properties `ej.microui.bench.xxx` to `true` or `false`; where `xxx` is:

* `display`: to bench the display buffer
* `imageheap`: to bench the image heap
* `decoder`: to bench the runtime image decoders

By default, all benches are disabled.

To compare the score of each bench with a previous iteration, set a positive value for the properties `ej.microui.bench.score.xxx`; where `xxx` is `display`, `imageheap` or `decoder`.

A bench needs a calibration time and a run duration.
Adjust the bench times thanks to the properties `ej.microui.bench.calibration.time.ms` and `ej.microui.bench.run.time.ms`.
A shorter run time will make the bench run faster, but will output a less precise score.

Specify the destination area (useful for a rounded display) using to the properties `ej.microui.bench.dest.xxx` where `xxx` is `x`, `y`, `width` and `height`.

Some tests use properties that take a list as their value (for instance: image formats).
The default separator for values in lists is the comma.
It can be changed by the property `ej.microui.bench.list.separator`.

The bench is considered as passed if the new score close to that of the previous iteration, within a set tolerance range.
Adjust this tolerance (in percentage) with the property `ej.microui.bench.score.tolerance.percentage`.

Adjust the verbosity with the property `ej.microui.bench.verbose`.

### Configuration: Destination

To enable or disable a sub-bench (see the chapter [Benches Destination](#benches-destination)), set the properties `ej.microui.bench.xxx` to `true` or `false`, where `xxx` is:

* `fullscreen`: to bench the *fill full screen*
* `aliased`: to bench the aliased drawings
* `antialiased`: to bench the anti-aliased drawings
* `font`: to bench the drawing of strings
* `image`: to bench the drawing of images
* `image.flip`: to bench the drawing of flipped images
* `image.scale`: to bench the drawing of scaled images
* `image.rotate`: to bench the drawing of rotated images

The bench `ej.microui.bench.antialiased` uses the properties `ej.microui.bench.antialiased.fade` and `ej.microui.bench.antialiased.thickness` to customize the fading and the thickness of the drawings.

The bench `ej.microui.bench.font` consists in drawing a string with different fonts.
A font is identified by its height and in its number of bits-per-pixel.

* Adjust the font sizes with the list `ej.microui.bench.font.sizes`.
* Adjust the font BPP with the list `ej.microui.bench.font.bpp`.
* Change the text with the property `ej.microui.bench.font.text`.

For each combination, a font file must be provided.
Do not forget to adjust the fonts list file `/com/microej/microui/bench/_AllTests_RunBench.fonts_.list`.

The bench `ej.microui.bench.image` consists in drawing a RAW image (an image generated by the Image Generator).
Each RAW image can be fully opaque and/or can contain a transparency level.
However some combinations format/opacity have no meaning (for example, transparent RGB565 or fully opaque A4).
Additionally, each RAW image can be drawn with or without a global opacity value.

* Select the formats to be tested with the property `ej.microui.bench.image.formats`.
* Enable testing drawing transparent images with the property `ej.microui.bench.image.transparent`.
* Enable testing drawing using a global opacity value with the property `ej.microui.bench.image.globalAlpha`.

For each format/opacity combination, an image is required.
The name pattern is `/com/microej/microui/bench/image[FORMAT]_[opacity].png:[FORMAT]` where

* `[FORMAT]` is a RAW format, for instance `RGB565`. 
* `[opacity]` is `o` or `t` for respectively opaque and transparent.

Do not forget to adjust the images list file `/com/microej/microui/bench/_AllTests_RunBench.images_.list`. 

The benches `flip`, `rotate` and `scale` follow the same test conditions as for `draw image`.

### Configuration: Decoders

The bench is designed to check several runtime image decoders.
The default configuration only checks the internal PNG decoder.
Adjust the property `ej.microui.bench.decoder.input.formats` to add or remove a decoder.

Reduce the number of output formats by updating the list `ej.microui.bench.decoder.output.formats`.

Each decoder requires its own set of images to decode. 
The name pattern is `/com/microej/microui/bench/decoder_[opacity].[format]` where

* `[opacity]` is `o` or `t` for respectively opaque and transparent.
* `[format]` is an encoded format, for instance `png`.

Do not forget to adjust the images list file `/com/microej/microui/bench/_AllTests_RunBench.images_.list`. 

### Run the Bench

The bench is built as a testsuite test.
Therefore, it can be launched as a single test or during the testsuite execution.

* Set at least one property to enable a bench (display, imageheap or decoder): `ej.microui.bench.display=true`.
* Set the properties of the associated score(s): `ej.microui.score.display=355372910`.
* Adjust the other configurations options.

To launch the bench as a single test, the project that contains the bench must be imported into the workspace.
In the launcher of the bench, add the required properties by adding the prefix `microej.java.property.`:

* `-Dmicroej.java.property.ej.microui.bench.display=true`
* `-Dmicroej.java.property.ej.microui.score.display=355372910`

To launch the bench during the testsuite execution, these properties must be listed in the testsuite configuration file `config.properties`.
Each property must have two prefixes: `microej.testsuite.properties.` and `microej.java.property.`:

* `microej.testsuite.properties.microej.java.property.ej.microui.bench.display=false`
* `microej.testsuite.properties.microej.java.property.ej.microui.score.display=355372910`

## Test Suite Source Code Navigation

See VEE Port Test Suites [documentation](../README.md).

## Troubleshooting

See VEE Port Test Suites [documentation](../README.md).
