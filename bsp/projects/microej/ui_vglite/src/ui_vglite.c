/*
 * C
 *
 * Copyright 2020-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief MicroEJ MicroUI library low level API: implementation of ui_vglite.h.
 * @author MicroEJ Developer Team
 * @version 9.0.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLUI_DISPLAY_impl.h>

#include "ui_vglite.h"
#include "ui_color.h"
#include "mej_math.h"
#include "bsp_util.h"
#include "interrupts.h"

#include "vg_lite_hal.h"
#include "vglite_support.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Unknown display format
 */
#define DISPLAY_UNKNOWN_FORMAT      -1

/*
 * @brief To check if the image format is known
 */
#define VG_LITE_UNKNOWN_FORMAT      ((vg_lite_buffer_format_t)(DISPLAY_UNKNOWN_FORMAT))

/*
 * @brief Macro to align data
 */
#define ALIGN(value, align) (((value) + (align) - 1u) & ~((align) - 1u))

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

/*
 * @brief Action to perform during the GPU interrupt.
 */
typedef enum {
	/*
	 * @brief Nothing to do, the interrupt occurs during a drawing
	 */
	IRQ_BYPASS,

	/*
	 * @brief Wakes-up the drawer to continue the drawing
	 */
	IRQ_WAKEUP_TASK,

	/*
	 * @brief Wakes-up the MicroUI Graphics Engine
	 */
	IRQ_WAKEUP_GRAPHICS_ENGINE,
} vglite_irq_post_operation_t;

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

/*
 * @brief VGLite destination buffer (there is only one destination at any time)
 */
static vg_lite_buffer_t destination_buffer;

#ifdef VGLITE_OPTION_TOGGLE_GPU
/*
 * @brief flag to enable/disable hardware rendering
 */
static bool hardware_rendering;
#endif

/*
 * @brief IRQ operation
 */
static volatile vglite_irq_post_operation_t vg_lite_irq_operation;

/*
 * @brief vglite operation semaphore
 */
static void *vg_lite_operation_semaphore;

// -----------------------------------------------------------------------------
// Static Constants
// -----------------------------------------------------------------------------

/*
 * @brief LUT to convert MicroUI image format to VGLite image format
 */
const vg_lite_buffer_format_t __microui_to_vg_lite_format[] = {
	VG_LITE_RGB565,             // MICROUI_IMAGE_FORMAT_LCD = 0,
	VG_LITE_UNKNOWN_FORMAT,     // UNKNOWN = 1,
	VG_LITE_RGBA8888,           // MICROUI_IMAGE_FORMAT_ARGB8888 = 2,
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_RGB888 = 3, unsupported
	VG_LITE_RGB565,             // MICROUI_IMAGE_FORMAT_RGB565 = 4,
	VG_LITE_RGBA5551,           // MICROUI_IMAGE_FORMAT_ARGB1555 = 5
	VG_LITE_RGBA4444,           // MICROUI_IMAGE_FORMAT_ARGB4444 = 6,
	VG_LITE_A4,                 // MICROUI_IMAGE_FORMAT_A4 = 7,
	VG_LITE_A8,                 // MICROUI_IMAGE_FORMAT_A8 = 8,
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_LRGB888
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_LARGB8888
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_A2
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_A1
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_C4
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_C2
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_C1
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_AC44
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_AC22
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_AC11
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F8
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F4
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F2
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F1
	VG_LITE_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_A8_RLE
	VG_LITE_RGBA8888,           // MICROUI_IMAGE_FORMAT_ARGB8888_PRE
	VG_LITE_RGBA5551,           // MICROUI_IMAGE_FORMAT_ARGB1555_PRE
	VG_LITE_RGBA4444,           // MICROUI_IMAGE_FORMAT_ARGB4444_PRE
	// outside of the table ... MICROUI_IMAGE_FORMAT_CUSTOM = 255,
};

/*
 * @brief Look Up Table to translate MicroUI image to BPP
 */
static const int __microui_to_bpp[] = {
	(5 + 6 + 5),                // MICROUI_IMAGE_FORMAT_LCD = 0 = RGB565,
	DISPLAY_UNKNOWN_FORMAT,     // UNKNOWN = 1,
	(4 * 8),                    // MICROUI_IMAGE_FORMAT_ARGB8888 = 2,
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_RGB888 = 3, unsupported
	(5 + 6 + 5),                // MICROUI_IMAGE_FORMAT_RGB565 = 4,
	(1 + (3 * 5)),              // MICROUI_IMAGE_FORMAT_ARGB1555 = 5,
	(4 * 4),                    // MICROUI_IMAGE_FORMAT_ARGB4444 = 6,
	4,                          // MICROUI_IMAGE_FORMAT_A4 = 7,
	8,                          // MICROUI_IMAGE_FORMAT_A8 = 8,
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_LRGB888
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_LARGB8888
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_A2
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_A1
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_C4
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_C2
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_C1
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_AC44
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_AC22
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_AC11
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F8
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F4
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F2
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_F1
	DISPLAY_UNKNOWN_FORMAT,     // MICROUI_IMAGE_FORMAT_A8_RLE
	(4 * 8),                    // MICROUI_IMAGE_FORMAT_ARGB8888_PRE
	(1 + (3 * 5)),              // MICROUI_IMAGE_FORMAT_ARGB1555_PRE
	(4 * 4),                    // MICROUI_IMAGE_FORMAT_ARGB4444_PRE
	// outside of the table ... MICROUI_IMAGE_FORMAT_CUSTOM = 255,
};

/*
 * @brief Look Up Table to enable the premultiplication according to MicroUI image format
 */
static const bool __microui_to_premul[] = {
	false,      // MICROUI_IMAGE_FORMAT_LCD = 0 = RGB565,
	false,      // UNKNOWN = 1,
	true,       // MICROUI_IMAGE_FORMAT_ARGB8888 = 2,
	false,      // MICROUI_IMAGE_FORMAT_RGB888 = 3, unsupported
	false,      // MICROUI_IMAGE_FORMAT_RGB565 = 4,
	true,       // MICROUI_IMAGE_FORMAT_ARGB1555 = 5,
	true,       // MICROUI_IMAGE_FORMAT_ARGB4444 = 6,
	// outside of the table ... false,
};

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

/*
 * @brief Configures a vg_lite buffer for a RGB565 image
 *
 * @param[in] b: buffer to configure
 */
static void __buffer_default_configuration(vg_lite_buffer_t *b) {
	/* Reset planar. */
	b->yuv.uv_planar = 0;
	b->yuv.v_planar = 0;
	b->yuv.alpha_planar = 0;

	b->tiled = VG_LITE_LINEAR;
	b->format = VG_LITE_RGB565;
	b->handle = NULL;
	b->memory = NULL;
	b->address = 0;
	b->yuv.swizzle = VG_LITE_SWIZZLE_UV;
	b->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
	b->transparency_mode = VG_LITE_IMAGE_OPAQUE;
}

/*
 * @brief Configures and return current vg_lite buffer for the given image (address and size)
 *
 * @param[in] image: image from which buffer should be configured
 * @param[in] b: buffer to configure
 */
static void __buffer_set_address_and_size(MICROUI_Image *image, vg_lite_buffer_t *b) {
	b->width = image->width;
	b->height = image->height;
	b->stride = LLUI_DISPLAY_getStrideInBytes(image);
	b->memory = (void *)LLUI_DISPLAY_getBufferAddress(image);
	b->address = (uint32_t)LLUI_DISPLAY_getBufferAddress(image);
}

/*
 * @brief Convert MICROUI_ImageFormat to vg_lite_buffer_format_t
 *
 * @param[in] microui_format: MicroUI image format
 */
static vg_lite_buffer_format_t __convert_format(MICROUI_ImageFormat microui_format) {
	vg_lite_buffer_format_t vg_lite_format = VG_LITE_UNKNOWN_FORMAT;
	if (microui_format < (sizeof(__microui_to_vg_lite_format) / sizeof(__microui_to_vg_lite_format[0]))) {
		vg_lite_format = __microui_to_vg_lite_format[microui_format];
	}

	return vg_lite_format;
}

static vg_lite_buffer_format_t __convert_input_format(MICROUI_Image *image) {
	MICROUI_ImageFormat microui_format = (MICROUI_ImageFormat)image->format;
	vg_lite_buffer_format_t vg_lite_format = __convert_format(microui_format);

	if (VG_LITE_UNKNOWN_FORMAT != vg_lite_format) {
		// check if the image pixels must be pre-multiplied with the opacity
		bool premul_required;
		if (LLUI_DISPLAY_isTransparent(image) &&
		    (microui_format < (sizeof(__microui_to_premul) / sizeof(__microui_to_premul[0])))) {
			premul_required = __microui_to_premul[microui_format];
		} else {
			premul_required = false;
		}

		if (premul_required) {
			if (VG_LITE_SUCCESS != vg_lite_enable_premultiply()) {
				// pre-multiplication cannot be enabled or unsupported
#ifndef VGLITE_USE_GPU_FOR_TRANSPARENT_IMAGES
				// cannot draw a transparent image without applying a pre-multiplication
				vg_lite_format = VG_LITE_UNKNOWN_FORMAT;
#endif // VGLITE_USE_GPU_FOR_TRANSPARENT_IMAGES
			}
		} else if (!UI_VGLITE_need_to_premultiply() && (VG_LITE_SUCCESS != vg_lite_disable_premultiply())) {
			// have to disable the premultiplication but cannot
			UI_VGLITE_IMPL_error(false, "vg_lite engine premultiply error: cannot disable the pre multiplication");
		} else {
			// premultiplication useless and nothing to disable
			// -> nothing to do
		}
	}

	return vg_lite_format;
}

// -----------------------------------------------------------------------------
// Low Level API [optional]: weak functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT void UI_VGLITE_IMPL_notify_gpu_start(void) {
	// does nothing by default
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT void UI_VGLITE_IMPL_notify_gpu_stop(void) {
	// does nothing by default
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT void UI_VGLITE_IMPL_error(bool critical, const char *format, ...) {
	(void)format;
	while (critical) {
	}
}

// -----------------------------------------------------------------------------
// ui_vglite.h functions
// -----------------------------------------------------------------------------

void UI_VGLITE_initialize(void) {
	static bool initialized = false;

	if (!initialized) {
		if (kStatus_Success != BOARD_PrepareVGLiteController()) {
			UI_VGLITE_IMPL_error(true, "Prepare VGlite controlor error");
		}

		// Initialize the VGLite library and it's tesselation buffer.
		vg_lite_error_t ret = vg_lite_init(VGLITE_TESSELATION_WIDTH, VGLITE_TESSELATION_HEIGHT);
		if (VG_LITE_SUCCESS != ret) {
			UI_VGLITE_IMPL_error(true, "vg_lite engine init failed: vg_lite_init() returned error %d", ret);
		}

		initialized = true;
	}
}

// See the header file for the function documentation
void UI_VGLITE_start(void *binary_semaphore) {
	// Configures the vg_lite destination buffer for a RGB565 image
	__buffer_default_configuration(&destination_buffer);

	// Enable by default hardware rendering
	UI_VGLITE_enable_hardware_rendering();

	vg_lite_operation_semaphore = binary_semaphore;
}

void UI_VGLITE_enable_hardware_rendering(void) {
#ifdef VGLITE_OPTION_TOGGLE_GPU
	hardware_rendering = true;
#endif
}

// See the header file for the function documentation
void UI_VGLITE_disable_hardware_rendering(void) {
#ifdef VGLITE_OPTION_TOGGLE_GPU
	hardware_rendering = false;
#endif
}

// See the header file for the function documentation
void UI_VGLITE_toggle_hardware_rendering(void) {
#ifdef VGLITE_OPTION_TOGGLE_GPU
	hardware_rendering = !hardware_rendering;
#endif
}

// See the header file for the function documentation
bool UI_VGLITE_is_hardware_rendering_enabled(void) {
#ifdef VGLITE_OPTION_TOGGLE_GPU
	return hardware_rendering;
#else
	return true;
#endif
}

// See the header file for the function documentation
vg_lite_buffer_t * UI_VGLITE_configure_destination(MICROUI_GraphicsContext *gc) {
	MICROUI_Image *image = &gc->image;

	if (!LLUI_DISPLAY_isLCD(image) || ((uint32_t)LLUI_DISPLAY_getBufferAddress(image) != destination_buffer.address)) {
		// we target an image or the previous destination was not the display
		// -> have to use another context to force vg_lite to reload the new context
		__buffer_set_address_and_size(image, &destination_buffer);
		destination_buffer.format = __convert_format((MICROUI_ImageFormat)image->format);
	}
	// else: we target the display which is the same destination as for the previous drawing: nothing to do

	vg_lite_irq_operation = IRQ_BYPASS; // do nothing in the IRQ routine
	UI_VGLITE_IMPL_notify_gpu_start();

	return &destination_buffer;
}

// See the header file for the function documentation
bool UI_VGLITE_configure_source(vg_lite_buffer_t *buffer, MICROUI_Image *image) {
	bool ret = false;

	uint32_t stride = LLUI_DISPLAY_getStrideInBytes(image);

	if (LLUI_DISPLAY_IMPL_getNewImageStrideInBytes(image->format, image->width, image->height, stride) == stride) {
		vg_lite_buffer_format_t format = __convert_input_format(image);

		if (VG_LITE_UNKNOWN_FORMAT != format) {
			__buffer_default_configuration(buffer);
			__buffer_set_address_and_size(image, buffer);

			buffer->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE; // image only
			buffer->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
			buffer->format = format;
			buffer->stride = stride;
			ret = true;
		}
	}
	// else: frame buffer does not respect VGLite alignment

	return ret;
}

// See the header file for the function documentation
void UI_VGLITE_start_operation(bool wakeup_graphics_engine) {
	vg_lite_irq_operation = wakeup_graphics_engine ? IRQ_WAKEUP_GRAPHICS_ENGINE : IRQ_WAKEUP_TASK;

	// VG drawing has been added to the GPU commands list: ask to submit VG operation
	vg_lite_flush();

	if (!wakeup_graphics_engine) {
		// active waiting until the GPU interrupt is thrown
		LLUI_DISPLAY_IMPL_binarySemaphoreTake(vg_lite_operation_semaphore);
	}
}

// See the header file for the function documentation
DRAWING_Status UI_VGLITE_post_operation(MICROUI_GraphicsContext *gc, vg_lite_error_t vg_lite_error) {
	DRAWING_Status ret;
	if (VG_LITE_SUCCESS != vg_lite_error) {
		// GPU is useless now, can be disabled. No GPU access should be done after this line
		UI_VGLITE_IMPL_notify_gpu_stop();
		ret = UI_VGLITE_report_vglite_error(gc, vg_lite_error);
	} else {
		// start GPU operation and do not wait for it to end
		UI_VGLITE_start_operation(true);
		ret = DRAWING_RUNNING;
	}

	if (!UI_VGLITE_need_to_premultiply() && (VG_LITE_SUCCESS != vg_lite_enable_premultiply())) {
		// cannot restore premultiplication
		UI_VGLITE_IMPL_error(false, "vg_lite engine premultiply error: cannot restore the pre multiplication");
	}

	return ret;
}

// See the header file for the function documentation
uint32_t UI_VGLITE_premultiply_alpha(uint32_t color, uint8_t alpha) {
	uint32_t ret;

	if (alpha == (uint8_t)0) {
		ret = 0;
	} else if (alpha == (uint8_t)0xff) {
		ret = color;
	} else {
		uint8_t red = (uint8_t)(
			(uint32_t)(alpha * COLOR_GET_CHANNEL(color, ARGB8888, RED))
			/ (uint32_t)0xff);
		uint8_t green = (uint8_t)(
			(uint32_t)(alpha * COLOR_GET_CHANNEL(color, ARGB8888, GREEN))
			/ (uint32_t)0xff);
		uint8_t blue = (uint8_t)(
			(uint32_t)(alpha * COLOR_GET_CHANNEL(color, ARGB8888, BLUE))
			/ (uint32_t)0xff);
		ret = COLOR_SET_COLOR(alpha, red, green, blue, ARGB8888);
	}
	return ret;
}

// See the header file for the function documentation
uint32_t UI_VGLITE_premultiply(uint32_t color) {
	uint8_t alpha = COLOR_GET_CHANNEL(color, ARGB8888, ALPHA);
	return UI_VGLITE_premultiply_alpha(color, alpha);
}

// See the header file for the function documentation
bool UI_VGLITE_need_to_premultiply(void) {
	return !vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY);
}

// See the header file for the function documentation
vg_lite_color_t UI_VGLITE_get_vglite_color(const MICROUI_GraphicsContext *gc, const MICROUI_Image *img, jint alpha) {
	jint fc;

	switch (img->format) {
	case MICROUI_IMAGE_FORMAT_A4:
	case MICROUI_IMAGE_FORMAT_A8:
		if (UI_VGLITE_need_to_premultiply()) {
			// hardware does not manage the pre-multiplication: the src color must be pre-multiplied
			fc = UI_VGLITE_premultiply_alpha(gc->foreground_color, alpha);
		} else {
			// hardware manages the pre-multiplication (even if it has been disabled because not applicable
			// for the "picto" images): the src color must stay as is
			fc = (alpha << 24) | (gc->foreground_color & 0xFFFFFF);
		}
		break;

	default:
		if (UI_VGLITE_need_to_premultiply()) {
			// hardware does not manage the pre-multiplication: the color is used as "pre-multiplied" factor
			// for all pixel components (A-R-G-B)
			fc = alpha * 0x01010101;
		} else {
			// hardware manages the pre-multiplication
			// - pre-multiplication enabled: keep the image pixels values and use the opacity as
			// "pre-multiplied" factor
			// - pre-multiplication disabled: the image pixels are already pre-multiplied: keep the image
			// pixels values and use the opacity as "pre-multiplied" factor
			fc = (alpha << 24) | 0xFFFFFF;
		}
		break;
	}

	return (vg_lite_color_t)fc;
}

// See the header file for the function documentation
void UI_VGLITE_IRQHandler(void) {
	// retrieve the post operation to perform and reset it
	vglite_irq_post_operation_t operation = vg_lite_irq_operation;
	vg_lite_irq_operation = IRQ_BYPASS;

	switch (operation) {
	default: // should not occur
	case IRQ_BYPASS:
	{
		// Nothing to do: an automatic flush() has been made by the vg_lite engine to free the command buffer.
		// After this interrupt, the vg_lite engine is waked-up (see vg_lite_os_wait_interrupt()) and fills the
		// command buffer with the new pending command (command added by a VEE drawing).
		break;
	}

	case IRQ_WAKEUP_TASK:
	{
		// The intermediate drawing of a "big" drawing is performed: wakes-up the drawer (the caller of
		// UI_VGLITE_start_operation()) to draw the next steps of this "big" drawing (text, raw image,
		// bvi, etc.).
		LLUI_DISPLAY_IMPL_binarySemaphoreGive(vg_lite_operation_semaphore, true);
		break;
	}

	case IRQ_WAKEUP_GRAPHICS_ENGINE:
	{
		uint8_t it = interrupt_enter();

		// The "simple" drawing or the last step of the "big" drawing is made: wakes-up the Graphics Engine to
		// notify the end of the drawing and unlock the next application drawing.
		LLUI_DISPLAY_notifyAsynchronousDrawingEnd(true);

		// GPU is useless now, can be disabled. No GPU access should be done after this line
		UI_VGLITE_IMPL_notify_gpu_stop();

		interrupt_leave(it);

		break;
	}
	}
}

// See the header file for the function documentation
bool UI_VGLITE_enable_vg_lite_scissor(MICROUI_GraphicsContext *gc) {
	bool ret;
	int32_t width = UI_RECT_get_width(&gc->clip);
	int32_t height = UI_RECT_get_height(&gc->clip);

	if (LLUI_DISPLAY_isClipEnabled(gc)) {
		if ((width > 0) && (height > 0)) {
			// not "empty" clip

			// enable scissor for next vglite drawing
			vg_lite_enable_scissor();
			vg_lite_set_scissor(gc->clip.x1, gc->clip.y1, width, height);

			// perform drawing
			ret = true;
		} else {
			// empty clip: nothing to draw
			ret = false;
		}
	} else {
		// clip is disabled :

		// disable scissor (vglite lib already crops to the buffer bounds)
		vg_lite_disable_scissor();

		// perform drawing
		ret = true;
	}

	return ret;
}

// See the header file for the function documentation
bool UI_VGLITE_enable_vg_lite_scissor_region(MICROUI_GraphicsContext *gc, int x1, int y1, int x2, int y2) {
	bool ret;

	if (LLUI_DISPLAY_isClipEnabled(gc)) {
		if (LLUI_DISPLAY_clipRectangle(gc, (jint *)&x1, (jint *)&y1, (jint *)&x2, (jint *)&y2)) {
			// drawing fully or partially fits the clip:

			// enable scissor for next vglite drawing
			vg_lite_enable_scissor();
			vg_lite_set_scissor(gc->clip.x1, gc->clip.y1, UI_RECT_get_width(&gc->clip), UI_RECT_get_height(&gc->clip));

			// perform drawing
			ret = true;
		} else {
			// drawing is fully outside the clip, nothing to draw
			ret = false;
		}
	} else {
		// clip is disabled :

		// disable scissor (vglite lib already crops to the buffer bounds)
		vg_lite_disable_scissor();

		// perform drawing
		ret = true;
	}

	return ret;
}

// See the header file for the function documentation
bool UI_VGLITE_enable_vg_lite_scissor_area(MICROUI_GraphicsContext *gc, jfloat width, jfloat height,
                                           const jfloat *matrix) {
	bool ret;

	if (LLUI_DISPLAY_isClipEnabled(gc)) {
		// Transform each corners points of the image viewbox
		// top/left, not impacted by scale and rotate
		int x0 = (int)matrix[2];
		int y0 = (int)matrix[5];

		// top/right
		int x1 = (int)((width * matrix[0]) + x0);
		int y1 = (int)((width * matrix[3]) + y0);

		// bottom/Left
		int x2 = (int)((height * matrix[1]) + x0);
		int y2 = (int)((height * matrix[4]) + y0);

		// bottom/right
		int x3 = (int)(x2 + x1 - x0);
		int y3 = (int)(y2 + y1 - y0);

		// Compute the clipping area from all points
		int render_area_x = MEJ_MIN(x0, MEJ_MIN(x1, MEJ_MIN(x2, x3)));
		int render_area_y = MEJ_MIN(y0, MEJ_MIN(y1, MEJ_MIN(y2, y3)));
		int render_area_width = MEJ_MAX(x0, MEJ_MAX(x1, MEJ_MAX(x2, x3))) - render_area_x;
		int render_area_height = MEJ_MAX(y0, MEJ_MAX(y1, MEJ_MAX(y2, y3))) - render_area_y;

		// get GC clip
		int master_clip_x = gc->clip.x1;
		int master_clip_y = gc->clip.y1;
		int master_clip_width = UI_RECT_get_width(&gc->clip);
		int master_clip_height = UI_RECT_get_height(&gc->clip);

		// crop x
		if (render_area_x < master_clip_x) {
			//-> decrease width and set x to the minimum coordinate
			render_area_width -= master_clip_x - render_area_x;
			render_area_x = master_clip_x;
		}

		// crop y
		if (render_area_y < master_clip_y) {
			//-> decrease height and set y to the minimum coordinate
			render_area_height -= master_clip_y - render_area_y;
			render_area_y = master_clip_y;
		}

		int master_clip_outside_x = master_clip_x + master_clip_width;
		int master_clip_outside_y = master_clip_y + master_clip_height;

		if (render_area_x >= master_clip_outside_x) {
			// expected X is out of clip on the right: set an empty clip
			render_area_width = 0;
		} else {
			// crop x+width, width is set to 0 if negative (empty clip)
			render_area_width = MEJ_MAX(0, MEJ_MIN(render_area_width, master_clip_outside_x - render_area_x));
		}

		if (render_area_y >= master_clip_outside_y) {
			// expected Y is out of clip on the bottom: set an empty clip
			render_area_height = 0;
		} else {
			// crop y+height, height is set to 0 if negative (empty clip)
			render_area_height = MEJ_MAX(0, MEJ_MIN(render_area_height, master_clip_outside_y - render_area_y));
		}

		if ((0 < render_area_width) && (0 < render_area_height)) {
			vg_lite_enable_scissor();
			vg_lite_set_scissor(render_area_x, render_area_y, render_area_width, render_area_height);

			// perform drawing
			ret = true;
		} else {
			// drawing is fully outside the clip, nothing to draw
			ret = false;
		}
	} else {
		// clip is disabled :

		// disable scissor (vglite lib already crops to the buffer bounds)
		vg_lite_disable_scissor();

		// perform drawing
		ret = true;
	}

	return ret;
}

// See the header file for the function documentation
int32_t UI_VGLITE_get_bpp(MICROUI_ImageFormat image_format) {
	int bpp = DISPLAY_UNKNOWN_FORMAT;

	if (image_format < (sizeof(__microui_to_bpp) / sizeof(__microui_to_bpp[0]))) {
		bpp = __microui_to_bpp[image_format];
	}

	return (DISPLAY_UNKNOWN_FORMAT == bpp) ? -1 : bpp;
}

// See the header file for the function documentation
DRAWING_Status UI_VGLITE_report_vglite_error(MICROUI_GraphicsContext *gc, vg_lite_error_t vg_lite_error) {
	UI_VGLITE_IMPL_error(false, "vg_lite operation failed with error: %d\n", vg_lite_error);
	LLUI_DISPLAY_reportError(gc,
	                         DRAWING_LOG_LIBRARY_INCIDENT | ((VG_LITE_OUT_OF_MEMORY ==
	                                                          vg_lite_error) ? DRAWING_LOG_OUT_OF_MEMORY : 0));
	return DRAWING_DONE;
}

// -----------------------------------------------------------------------------
// ui_drawing.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
uint32_t UI_DRAWING_getNewImageStrideInBytes(jbyte image_format, uint32_t image_width, uint32_t image_height,
                                             uint32_t default_stride) {
	(void)image_height;

	int32_t bpp = UI_VGLITE_get_bpp((MICROUI_ImageFormat)image_format);
	uint32_t ret;

	if (-1 != bpp) {
		// VGLite library requires a stride on 16 pixels
		uint32_t stride = ALIGN(image_width, 16u);
		ret = (stride * (uint32_t)bpp) / 8u;
	} else {
		// unsupported format
		ret = default_stride;
	}

	return ret;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
