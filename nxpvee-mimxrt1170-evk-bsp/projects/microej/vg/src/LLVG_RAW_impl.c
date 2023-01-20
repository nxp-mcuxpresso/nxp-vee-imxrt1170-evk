/*
 * C
 *
 * Copyright 2022-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: RAW image management.
 * @author MicroEJ Developer Team
 * @version 4.0.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include <sni.h>
#include <LLUI_DISPLAY.h>
#include <LLUI_DISPLAY_impl.h>
#include <LLVG_MATRIX_impl.h>
#include <BESTFIT_ALLOCATOR.h>

#include "bsp_util.h"
#include "color.h"
#include "vg_lite.h"
#include "vg_lite_kernel.h"
#include "display_vglite.h"
#include "vglite_path.h"
#include "microvg_path.h"
#include "microvg_vglite_helper.h"
#include "microvg_helper.h"

#include "fsl_debug_console.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Enables loading of vector image files from an external resource system.
 *
 * If this feature is enabled, when an image file is not available in the application classpath, the implementation tries to load it from an external resource system.
 * If this feature is disabled, only the resources compiled with the application are used.
 *
 * This feature will be enabled if the Platform embeds the module "External Resource" and the BSP implements the functions declared in "LLEXT_RES_impl.h".
 */
#define VG_FEATURE_RAW_EXTERNAL __has_include("LLEXT_RES_impl.h")
#if VG_FEATURE_RAW_EXTERNAL
#include "LLEXT_RES_impl.h"
#endif

/*
 * @brief RAW format
 */
#define LLVG_RAW_VERSION (1)

/*
 * @brief Allocator management
 */
#define HEAP_SIZE (2 * 1024)
#define HEAP_START allocator_heap
#define HEAP_END (HEAP_START + HEAP_SIZE) // end address excluded (best fit allocator spec)
#define MALLOC(s) (BESTFIT_ALLOCATOR_allocate(&allocator_instance, VG_LITE_ALIGN((s), 4)))
#define FREE(s) (BESTFIT_ALLOCATOR_free(&allocator_instance, (s)))

#define TIMERATIO_RANGE 65535
#define TIMERATIO_OFFSET ((TIMERATIO_RANGE / 2) + 1)

/*
 * @brief Gamma value typically lies between 2.2 and 2.5 for CRT monitors, Android chose 2.2.
 */
#define GAMMA_VALUE 2.2f

#define COLOR_MATRIX_WIDTH (5)
#define RED_OFFSET (0)
#define GREEN_OFFSET (1)
#define BLUE_OFFSET (2)
#define ALPHA_OFFSET (3)
#define CONSTANT_OFFSET (4)
#define RED_LINE (COLOR_MATRIX_WIDTH * RED_OFFSET)
#define GREEN_LINE (COLOR_MATRIX_WIDTH * GREEN_OFFSET)
#define BLUE_LINE (COLOR_MATRIX_WIDTH * BLUE_OFFSET)
#define ALPHA_LINE (COLOR_MATRIX_WIDTH * ALPHA_OFFSET)

// -----------------------------------------------------------------------------
// Java library static final
// -----------------------------------------------------------------------------

#define IMAGE_SUCCESS (0)
#define IMAGE_INVALID_PATH (-1)
#define IMAGE_INVALID (-2)
#define IMAGE_OOM (-9)

#define RAW_OFFSET_F32_WIDTH (0)
#define RAW_OFFSET_F32_HEIGHT (1)
#define RAW_OFFSET_U32_DURATION (2)
#define RAW_OFFSET_U32_FLAGS (3)

#define RAW_FLAG_OVERLAP_PATH (0x01)
#define RAW_FLAG_FREE_MEMORY_ON_CLOSE (0x02)

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/*
 * @brief Flags of RAW image
 *
 * (u1) overlapping
 * (u7) padding
 * (u24) duration
 */
typedef struct vg_flags {
	uint32_t duration : 24;
	// cppcheck-suppress [unusedStructMember] padding is not used
	uint32_t : 7; // padding
	uint32_t overlapping : 1;
} vg_flags_t;

/*
 * @brief Defines a RAW image
 */
typedef struct vector_image {

	uint32_t signature_msb;
	uint32_t signature_lsb;

	float width;
	float height;

	vg_flags_t flags;
	uint32_t offset_first_block;

} vector_image_t;

/*
 * @brief Defines a path.
 */
typedef struct vg_path {
	float bounding_box[4];    // left, top, right, bottom
	uint32_t length;
	vg_lite_format_t format;
	uint8_t padding0;
	uint8_t padding1;
	uint8_t padding2;

	// array of data of path

} vg_path_t;

/*
 * @brief Types of image's blocks.
 */
typedef enum vg_block_kind {

	VG_BLOCK_LAST,
	VG_BLOCK_PATH_COLOR,
	VG_BLOCK_PATH_GRADIENT,
	VG_BLOCK_GROUP_TRANSFORM,
	VG_BLOCK_GROUP_ANIMATE,
	VG_BLOCK_GROUP_END,
	VG_BLOCK_PATH_COLOR_ANIMATE,
	VG_BLOCK_PATH_GRADIENT_ANIMATE,

} vg_block_kind_t;

/*
 * @brief Defines a RAW image's block.
 */
typedef struct vg_block {

	// block's kind (see vg_block_xxx)
	vg_block_kind_t kind;

} vg_block_t;

/*
 * @brief Defines a RAW image's path.
 */
typedef struct vg_block_path {

	// [ *** copy of vg_block_t **

	// block's kind and pointer to specific block's data
	vg_block_kind_t kind;

	// ] *** copy of vg_block_t **

	// common block's data for all VG drawings
	vg_lite_fill_t fill_rule;
	uint16_t padding0;

	vg_path_t* path;

} vg_block_path_t;

/*
 * @brief Element group with transformation
 */
typedef struct vg_block_group_transform {

	// [ *** copy of vg_block_t **

	// block's kind and pointer to specific block's data
	vg_block_kind_t kind;

	// ] *** copy of vg_block_t **

	uint8_t padding0;
	uint8_t padding1;
	uint8_t padding2;
	vg_lite_matrix_t matrix;

} vg_block_group_transform_t;

/*
 * @brief Element group with animation
 */
typedef struct vg_block_group_animation {

	// [ *** copy of vg_block_t **

	// block's kind and pointer to specific block's data
	vg_block_kind_t kind;

	// ] *** copy of vg_block_t **

	uint8_t padding;
	uint16_t block_size;

	uint8_t nb_translates;
	uint8_t nb_translates_xy;
	uint8_t nb_rotates;
	uint8_t nb_scales;

	float scale_x;
	float scale_y;
	float translate_x;
	float translate_y;
	float pivot_x;
	float pivot_y;
	float rotation;

	// array of vg_animation_translate_t
	// array of vg_animation_rotate_t
	// array of vg_animation_scale_t

} vg_block_group_animation_t;

/*
 * @brief Element group with animation
 */
typedef struct vg_block_group_end {

	// [ *** copy of vg_block_t **

	// block's kind and pointer to specific block's data
	vg_block_kind_t kind;

	// ] *** copy of vg_block_t **

	uint8_t padding0;
	uint8_t padding1;
	uint8_t padding2;

} vg_block_group_end_t;

/*
 * @brief Element "draw VG path".
 */
typedef struct vg_operation_color {

	vg_block_path_t header;
	uint32_t color;

} vg_operation_color_t;

/*
 * @brief Element "draw VG gradient".
 */
typedef struct vg_operation_gradient {

	vg_block_path_t header;

	// /!\ the VG-Lite gradient is constitued of the gradient header (colors and positions) and a VG-Lite image.
	// A VG-Lite image is a header + a pixel buffer. This image is made at runtime by VG-Lite library according
	// to the gradient data. The RAW format only embbeds the gradient header, not the "image" part to win some
	// footprint. By consequence the gradient data to copy from ROM to RAM is:
	// sizeof(gradient) == sizeof(vg_lite_linear_gradient_t) - sizeof(vg_lite_buffer_t)
	vg_lite_linear_gradient_t* gradient;

} vg_operation_gradient_t;

/*
 * @brief Element "animate color of path".
 */
typedef struct vg_operation_animate_color {

	vg_operation_color_t path;

	uint8_t nb_colors;
	uint8_t nb_alphas;
	uint8_t nb_path_datas;
	uint8_t padding0;

	uint16_t block_size;
	uint16_t padding1;

	// array of vg_animation_color_t
	// array of vg_animation_alpha_t
	// array of vg_animation_path_data_t

} vg_operation_animate_color_t;

/*
 * @brief Element "animate gradient of path".
 */
typedef struct vg_operation_animate_gradient {

	vg_operation_gradient_t path;

	uint8_t nb_alphas;
	uint8_t nb_path_datas;
	uint16_t block_size;

	// array of vg_animation_alpha_t
	// array of vg_animation_path_data_t

} vg_operation_animate_gradient_t;

/*
 * @brief Path interpolator
 */
typedef struct vg_path_interpolator {

	uint32_t size;

	// array of end_time_ratio (short)
	// padding
	// array of start_x (float)
	// array of start_y (float)

} vg_path_interpolator_t;

/*
 * @brief Scale animation
 */
typedef struct vg_animation_header {

	int32_t duration;
	int32_t begin;
	int32_t keep_duration;
	vg_path_interpolator_t* offset_path_interpolator;

} vg_animation_header_t;

/*
 * @brief Translate animation
 */
typedef struct vg_animation_translate {

	vg_animation_header_t header;
	float start_x;
	float start_y;
	float translation_x;
	float translation_y;

} vg_animation_translate_t;

/*
 * @brief Translate XY animation
 */
typedef struct vg_animation_translate_xy {

	vg_animation_header_t header;
	vg_path_interpolator_t* offset_path_interpolator;

} vg_animation_translate_xy_t;

/*
 * @brief Rotate animation
 */
typedef struct vg_animation_rotate {

	vg_animation_header_t header;
	float start_angle;
	float rotation_angle;
	float start_rotation_center_x;
	float start_rotation_center_y;
	float rotation_translation_center_x;
	float rotation_translation_center_y;

} vg_animation_rotate_t;

/*
 * @brief Scale animation
 */
typedef struct vg_animation_scale {

	vg_animation_header_t header;
	float scale_x_from;
	float scale_x_to;
	float scale_y_from;
	float scale_y_to;
	float pivot_x;
	float pivot_y;

} vg_animation_scale_t;

/*
 * @brief Color animation
 */
typedef struct vg_animation_color {

	vg_animation_header_t header;
	uint32_t start_color;
	uint32_t end_color;

} vg_animation_color_t;

/*
 * @brief Alpha animation
 */
typedef struct vg_animation_alpha {

	vg_animation_header_t header;
	uint8_t start_alpha;
	uint8_t end_alpha;

} vg_animation_alpha_t;

/*
 * @brief Path data animation
 */
typedef struct vg_animation_path_data {

	vg_animation_header_t header;
	vg_path_t* from;
	vg_path_t* to;

} vg_animation_path_data_t;

/*
 * @brief Function to update a matrix according an animation
 *
 * @param[in/out]: inout: the data to update
 * @param[in]: elapsed_time:  the time elapsed within this animation.
 * @param[in]: last: true when the final frame is reached
 *
 * @return: nothing
 */
typedef void (* VG_RAW_apply_animation_t) (
		vg_animation_header_t* animation,
		void* inout,
		jlong elapsed_time,
		size_t memory_offset,
		bool last
);

/*
 * @brief Element transformation
 */
typedef struct vg_transformation {

	struct vg_transformation* previous;
	vg_lite_matrix_t matrix;

} vg_transformation_t;

/*
 * @brief Internal MicroJVM element to retrieve a resource in the microejapp.o.
 */
typedef struct {
	void* data;
	uint32_t size;
} SNIX_resource;

/*
 * @brief Internal MicroJVM function to retrieve a resource in the microejapp.o.
 *
 * @param[in] path: the path of the resource to retrieve
 * @param[out] resource: the resource metadata
 */
extern int32_t SNIX_get_resource(char* path, SNIX_resource* resource);

// -----------------------------------------------------------------------------
// Extern functions & fields
// -----------------------------------------------------------------------------

/*
 * @brief Linker symbols that define the ROM block.
 */
extern uint32_t m_text_start;
extern uint32_t m_text_end;

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

const uint8_t signature[] = {(uint8_t)'M', (uint8_t)'E', (uint8_t)'J', (uint8_t)'_', (uint8_t)'M', (uint8_t)'V', (uint8_t)'G', (uint8_t)LLVG_RAW_VERSION};

static BESTFIT_ALLOCATOR allocator_instance;
static uint8_t allocator_heap[HEAP_SIZE];
static bool initialiazed = false;

/*
 * @brief First transformation to apply when rendering an image. This
 * transformation only holds the image transformation: it does not hold
 * the final transformation: the image translation.
 *
 * This transformation is not stored in the heap to be sure to apply at
 * least this transformation when drawing an image even if the heap is
 * full.
 */
static vg_transformation_t first_transformation;

/*
 * @brief Translation (== image translation) to apply on current transformation
 * when drawing a path (see render_matrix).
 */
static vg_lite_matrix_t translate_matrix;

/*
 * @brief Current transformation to apply when rendering a path. It is the
 * current transformation + the final transformation (== the image translation;
 * see translate_matrix).
 */
static vg_lite_matrix_t render_matrix;

/*
 * @brief Copy from RAW image (flash to RAM); require to target the path's data
 * in flash;
 */
static vg_lite_path_t render_path;

/*
 * @brief Gradient used for all drawings with gradient (only one image allocation,
 * does not alterate the original gradient's matrix and colors, etc.).
 */
static vg_lite_linear_gradient_t render_gradient;

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

static inline void* get_addr(void* addr, size_t memory_offset){
	assert(NULL != addr);
	// cppcheck-suppress [misra-c2012-11.5,misra-c2012-18.4] go to the expected address
	return (void*)(((uint8_t*)addr) + memory_offset);
}

static inline void* get_path_data_addr(vg_path_t* path){
	return get_addr(path, sizeof(vg_path_t));
}

static inline vg_path_t* get_path_addr(vg_path_t* path, size_t memory_offset){
	// cppcheck-suppress [misra-c2012-11.5] cast to vg_path_t* is valid for sure
	return (vg_path_t*)get_addr(path, memory_offset);
}

static inline vg_lite_linear_gradient_t* get_gradient_addr(vg_lite_linear_gradient_t* gradient, size_t memory_offset){
	return (vg_lite_linear_gradient_t*)get_addr(gradient, memory_offset);
}

static inline vg_path_interpolator_t* get_path_interpolator_addr(vg_path_interpolator_t* interpolator, size_t memory_offset){
	// cppcheck-suppress [misra-c2012-11.5] cast to vg_path_interpolator_t* is valid for sure
	return (vg_path_interpolator_t*)get_addr(interpolator, memory_offset);
}

/*
 * @brief Frees the given data from the heap or does nothing
 * when targets RO memory.
 *
 * @param[in] data: pointer on the data to free.
 */
static void _free_data(void* data) {
	// cppcheck-suppress [misra-c2012-18.4] HEAP_START and HEAP_END denote heap addresses
	if ((NULL != data) && (data >= (void*) HEAP_START) && (data < (void*) HEAP_END)) {
		FREE(data);
	}
	// else: data in flash (cannot be in RAM and outside the heap) or null
}

/*
 * @brief Allocates the given size in the heap.
 *
 * @param[in] size: the data's size (in bytes).
 *
 * @return the pointer to the stored data.
 */
inline static uint8_t* _alloc_data(uint32_t size) {
	uint8_t* ret = MALLOC(size);
	if (NULL == ret) {
		MEJ_LOG_ERROR_MICROVG("OOM\n");
	}
	return ret;
}

/*
 * @brief Stores the given data in the heap or returns the given pointer
 * when targets RO memory (no need to copy).
 *
 * @param[in] data: pointer on the data to store.
 * @param[in] size: the data's size.
 *
 * @return the pointer to the stored data or the ROM address.
 */
static uint8_t* _store_data(void* data, size_t size) {
	uint8_t* ret;
	if ((data >= &m_text_start) && (data < &m_text_end)) {
		// cppcheck-suppress [misra-c2012-11.5] cast the object in a u8 address
		ret = (uint8_t*)data;
	}
	else {
		ret = _alloc_data(size);
		if (NULL != ret){
			(void)memcpy((void*)ret, data, size);
		}
	}
	return ret;
}

/*
 * @brief Stores the given transformation in the heap.
 *
 * @param[in] transformation: pointer on the transformation to store.
 *
 * @return the pointer to the stored transformation.
 */
static vg_transformation_t* _store_vg_transformation(vg_transformation_t * transformation) {
	// cppcheck-suppress [misra-c2012-11.3] the stored data is a vg_transformation_t for sure
	return (vg_transformation_t*)_store_data(transformation, sizeof(vg_transformation_t));
}

static void _free_vg_transformation(vg_transformation_t * transformation) {
	_free_data(transformation);
}

/**
 * @brief Gets a filter color by applying a color transformation.
 *
 * @param[in] red, green, blue, alpha: the color to transform
 * @param[in] color_matrix: the transformation to apply
 * @param[in] start_index: the transformation index in the matrix
 *
 * @return the filtered color
 */
static uint32_t _filter_color_component(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha, const float color_matrix[], uint32_t start_index)  {
	float redFactor = color_matrix[start_index + (uint32_t)RED_OFFSET] * (float)red;
	float greenFactor = color_matrix[start_index + (uint32_t)GREEN_OFFSET] * (float)green;
	float blueFactor = color_matrix[start_index + (uint32_t)BLUE_OFFSET] * (float)blue;
	float alphaFactor = color_matrix[start_index + (uint32_t)ALPHA_OFFSET] * (float)alpha;
	float hardcodedColor = color_matrix[start_index + (uint32_t)CONSTANT_OFFSET];
	float factors = redFactor + greenFactor + blueFactor + alphaFactor + hardcodedColor;
	int32_t ret = (int32_t) factors;
	return (uint32_t)((ret < 0x0) ? 0x0 : ((ret > 0xff) ? 0xff : ret));
}

static uint32_t _filter_color(uint32_t color, const float color_matrix[]) {

	uint32_t red = COLOR_GET_CHANNEL(color, ARGB8888, RED);
	uint32_t green = COLOR_GET_CHANNEL(color, ARGB8888, GREEN);
	uint32_t blue = COLOR_GET_CHANNEL(color, ARGB8888, BLUE);
	uint32_t alpha = COLOR_GET_CHANNEL(color, ARGB8888, ALPHA);

	uint32_t resultRed = _filter_color_component(red, green, blue, alpha, color_matrix, RED_LINE);
	uint32_t resultGreen = _filter_color_component(red, green, blue, alpha, color_matrix, GREEN_LINE);
	uint32_t resultBlue = _filter_color_component(red, green, blue, alpha, color_matrix, BLUE_LINE);
	uint32_t resultAlpha = _filter_color_component(red, green, blue, alpha, color_matrix, ALPHA_LINE);

	return COLOR_SET_COLOR(resultAlpha, resultRed, resultGreen, resultBlue, ARGB8888);
}

static uint8_t _filter_alpha(uint8_t alpha, const float color_matrix[]) {
	uint32_t color = alpha;
	color <<= 24;
	color = _filter_color(color, color_matrix);
	return (uint8_t)(color >> 24);
}

static uint32_t _prepare_render_color(uint32_t color, uint32_t alpha, const float color_matrix[]) {
	uint32_t ret = color;
	if (NULL != color_matrix) {
		assert((uint32_t)0xff == alpha); // filter mode does not use global alpha (see MicroVG spec)
		ret = _filter_color(color, color_matrix);
	}
	else if (alpha != (uint32_t)0xff) {
		ret = MICROVG_HELPER_apply_alpha(color, alpha);
	}
	else {
		// nothing to change
	}
	return ret;
}

static void _prepare_render_gradient_colors(vg_lite_linear_gradient_t* gradient, uint32_t alpha, const float color_matrix[]) {

	uint32_t* colors = (uint32_t*)gradient->colors;

	if (NULL != color_matrix) {
		assert((uint32_t)0xff == alpha); // filter mode does not use global alpha

		for(int i = 0; i < gradient->count; i++) {
			colors[i] = _filter_color(colors[i], color_matrix);
		}
	}
	else if (alpha != (uint32_t)0xff) {
		for(int i = 0; i < gradient->count; i++) {
			colors[i] = MICROVG_HELPER_apply_alpha(colors[i], alpha);
		}
	}
	else {
		// nothing to change
	}
}

/*
 * @brief Prepares the matrix to apply to render the next paths: current transformation + image translation.
 *
 * FIXME idea: apply only the transformation when the next element to draw is a path. When it is a group, the
 * "rendering transformation" is useless. Beware: when leaving a group, have to check if the "rendering
 * transformation" has to be restored or not.
 */
static inline void _apply_translation(vg_transformation_t* p_render_transformation){
	LLVG_MATRIX_IMPL_multiply(MAP_VGLITE_MATRIX(&render_matrix), MAP_VGLITE_MATRIX(&translate_matrix), MAP_VGLITE_MATRIX(&(p_render_transformation->matrix)));
}

static bool _set_clip(MICROUI_GraphicsContext* gc, jfloat* matrix, jint* x, jint* y, jfloat width, jfloat height){

	bool outside = false;

	// Transform each corners points of the image viewbox
	// top/left, not impacted by scale and rotate
	float x0 = (float) (matrix[2]);
	float y0 = (float) (matrix[5]);

	// top/right
	float x1 = (float) ((width * matrix[0]) + x0);
	float y1 = (float) ((width * matrix[3]) + y0);

	// bottom/Left
	float x2 = (float) ((height * matrix[1]) + x0);
	float y2 = (float) ((height * matrix[4]) + y0);

	// bottom/right
	float x3 = (float) (x2 + x1 - x0);
	float y3 = (float) (y2 + y1 - y0);

	// Compute the clipping area from all points
	int render_area_x = round(MEJ_MIN(x0, MEJ_MIN(x1, MEJ_MIN(x2, x3))));
	int render_area_y = round(MEJ_MIN(y0, MEJ_MIN(y1, MEJ_MIN(y2, y3))));

	int render_area_max_x = ceil(MEJ_MAX(x0, MEJ_MAX(x1, MEJ_MAX(x2, x3))));
	int render_area_max_y = ceil(MEJ_MAX(y0, MEJ_MAX(y1, MEJ_MAX(y2, y3))));

	int render_area_width = render_area_max_x - render_area_x ;
	int render_area_height = render_area_max_y - render_area_y;

	// apply translate
	render_area_x += *x;
	render_area_y += *y;

	// get GC clip
	int master_clip_x = gc->clip_x1;
	int master_clip_y = gc->clip_y1;
	int master_clip_width = gc->clip_x2 - gc->clip_x1 + 1;
	int master_clip_height = gc->clip_y2 - gc->clip_y1 + 1;

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
	}
	else {
		outside = true;
	}

	return outside;
}

static float _get_path_interpolator_at_time(vg_path_interpolator_t* interpolator, float time, bool apply_x_transformation) {

	// retrieve arrays addresses
	uint8_t* data_offset = (uint8_t*)interpolator;
	// cppcheck-suppress [misra-c2012-18.4] points after the interpolator
	data_offset += sizeof(vg_path_interpolator_t);
	// cppcheck-suppress [misra-c2012-11.3] data_offset points on a 16-bit array for sure
	int16_t* end_time_ratio = (int16_t*)data_offset;
	uint32_t ratio_array_length = interpolator->size + (uint32_t)1;
	ratio_array_length &= ~1;
	// cppcheck-suppress [misra-c2012-18.4] points after the ratio array
	data_offset += ratio_array_length * sizeof(int16_t);
	// cppcheck-suppress [misra-c2012-11.3,invalidPointerCast] data_offset points on a float array for sure
	float* start_x = (float*)data_offset;
	// cppcheck-suppress [misra-c2012-18.4] points after the start X array
	data_offset += interpolator->size * sizeof(float);
	// cppcheck-suppress [misra-c2012-11.3,invalidPointerCast] data_offset points on a float array for sure
	float* start_y = (float*)data_offset;

	// get the values to apply
	float* values = apply_x_transformation ? start_x : start_y;

	// get a ratio between 0x0000 and 0xffff (TIMERATIO_RANGE)
	int32_t time_ratio = (int32_t) (MEJ_MIN(MEJ_MAX(time, 0.f), 1.f) * (float)TIMERATIO_RANGE);

	int32_t origin = 0;
	float ret = 0.f;
	bool done = false;

	// Last cells of arrays store the ending positions of last segment
	for (uint32_t i = 0; !done && (i < (interpolator->size - (uint32_t)1)); i++) {

		int32_t end = end_time_ratio[i] + TIMERATIO_OFFSET;

		if (time_ratio <= end) {
			float segPosition = ((float)time_ratio - (float)origin) / ((float)end - (float)origin);
			float segStartValue = values[i];
			float segEndValue = values[i + (uint32_t)1];
			ret = ((segStartValue * (1.f - segPosition)) + (segEndValue * segPosition));
			done = true;
		}
		origin = end;
	}

	return ret;
}

static float _get_transformation_ratio_at_time(vg_animation_header_t* animation, vg_path_interpolator_t* interpolator, jlong time) {
	float time_ratio;

	if (0 == animation->duration) {
		time_ratio = 1.f;
	} else {
		time_ratio = (float) time / animation->duration;
	}

	return _get_path_interpolator_at_time(interpolator, time_ratio, true);
}

static void _apply_group_animation_translate(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_translate_t for sure
	vg_animation_translate_t* animation_translate = (vg_animation_translate_t*)animation;
	vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float* matrix = (float*)data;
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	LLVG_MATRIX_IMPL_translate(matrix, animation_translate->start_x + (animation_translate->translation_x * transformRatio),
			animation_translate->start_y + (animation_translate->translation_y * transformRatio));
}

static void _apply_group_animation_translate_xy(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_translate_xy_t for sure
	vg_animation_translate_xy_t* animation_translate = (vg_animation_translate_xy_t*)animation;
	vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float* matrix = (float*)data;
	vg_path_interpolator_t* translation = get_path_interpolator_addr(animation_translate->offset_path_interpolator, memory_offset);
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	float translateX = _get_path_interpolator_at_time(translation, transformRatio, true);
	float translateY = _get_path_interpolator_at_time(translation, transformRatio, false);
	LLVG_MATRIX_IMPL_translate(matrix, translateX, translateY);
}

static void _apply_group_animation_rotate(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_rotate_t for sure
	vg_animation_rotate_t* animation_rotate = (vg_animation_rotate_t*)animation;
	vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float* matrix = (float*)data;
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	LLVG_MATRIX_IMPL_translate(matrix, animation_rotate->start_rotation_center_x + (animation_rotate->rotation_translation_center_x * transformRatio),
			animation_rotate->start_rotation_center_y + (animation_rotate->rotation_translation_center_y * transformRatio));
	LLVG_MATRIX_IMPL_rotate(matrix, animation_rotate->start_angle + (animation_rotate->rotation_angle * transformRatio));
	LLVG_MATRIX_IMPL_translate(matrix, -animation_rotate->start_rotation_center_x - (animation_rotate->rotation_translation_center_x * transformRatio),
			-animation_rotate->start_rotation_center_y - (animation_rotate->rotation_translation_center_y * transformRatio));
}

static inline float _get_scale(float from, float to, float transformRatio) {
	float ret;
	if (from < to) {
		ret = (float) (from * powf(to / from, transformRatio));
	} else {
		ret = (float) (to * powf(from / to, 1.f - transformRatio));
	}
	return ret;
}

static void _apply_group_animation_scale(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_scale_t for sure
	vg_animation_scale_t* animation_scale = (vg_animation_scale_t*)animation;
	vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float* matrix = (float*)data;
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	float scaleX = _get_scale(animation_scale->scale_x_from, animation_scale->scale_x_to, transformRatio);
	float scaleY = _get_scale(animation_scale->scale_y_from, animation_scale->scale_y_to, transformRatio);
	LLVG_MATRIX_IMPL_translate(matrix, animation_scale->pivot_x, animation_scale->pivot_y);
	LLVG_MATRIX_IMPL_scale(matrix, scaleX, scaleY);
	LLVG_MATRIX_IMPL_translate(matrix, -animation_scale->pivot_x, -animation_scale->pivot_y);
}

/*
 * This function returns the calculated in-between value for a color given integers that represent the start and end
 * values in the four bytes of the 32-bit int. Each channel is separately linearly interpolated and the resulting
 * calculated values are recombined into the return value.
 *
 * @param fraction
 *                       The fraction from the starting to the ending values
 * @param start_color
 *                       A 32-bit int value representing colors in the separate bytes of the parameter
 * @param end_color
 *                       A 32-bit int value representing colors in the separate bytes of the parameter
 * @return A value that is calculated to be the linearly interpolated result, derived by separating the start and
 *         end values into separate color channels and interpolating each one separately, recombining the resulting
 *         values in the same way.
 */
static uint32_t _interpolate_color(float fraction, uint32_t start_color, uint32_t end_color) {

	float startA = ((float) (COLOR_GET_CHANNEL(start_color, ARGB8888, ALPHA))) / COLOR_ARGB8888_CHANNEL_MASK;
	float startR = ((float) (COLOR_GET_CHANNEL(start_color, ARGB8888, RED))) / COLOR_ARGB8888_CHANNEL_MASK;
	float startG = ((float) (COLOR_GET_CHANNEL(start_color, ARGB8888, GREEN))) / COLOR_ARGB8888_CHANNEL_MASK;
	float startB = ((float) (COLOR_GET_CHANNEL(start_color, ARGB8888, BLUE))) / COLOR_ARGB8888_CHANNEL_MASK;

	float endA = ((float) (COLOR_GET_CHANNEL(end_color, ARGB8888, ALPHA))) / COLOR_ARGB8888_CHANNEL_MASK;
	float endR = ((float) (COLOR_GET_CHANNEL(end_color, ARGB8888, RED))) / COLOR_ARGB8888_CHANNEL_MASK;
	float endG = ((float) (COLOR_GET_CHANNEL(end_color, ARGB8888, GREEN))) / COLOR_ARGB8888_CHANNEL_MASK;
	float endB = ((float) (COLOR_GET_CHANNEL(end_color, ARGB8888, BLUE))) / COLOR_ARGB8888_CHANNEL_MASK;

	// convert from sRGB to linear
	startR = (float) powf(startR, GAMMA_VALUE);
	startG = (float) powf(startG, GAMMA_VALUE);
	startB = (float) powf(startB, GAMMA_VALUE);

	endR = (float) powf(endR, GAMMA_VALUE);
	endG = (float) powf(endG, GAMMA_VALUE);
	endB = (float) powf(endB, GAMMA_VALUE);

	// compute the interpolated color in linear space
	float a = startA + (fraction * (endA - startA));
	float r = startR + (fraction * (endR - startR));
	float g = startG + (fraction * (endG - startG));
	float b = startB + (fraction * (endB - startB));

	// convert back to sRGB in the [0..255] range
	a = a * COLOR_ARGB8888_CHANNEL_MASK;
	r = (float) powf(r, 1.0 / GAMMA_VALUE) * COLOR_ARGB8888_CHANNEL_MASK;
	g = (float) powf(g, 1.0 / GAMMA_VALUE) * COLOR_ARGB8888_CHANNEL_MASK;
	b = (float) powf(b, 1.0 / GAMMA_VALUE) * COLOR_ARGB8888_CHANNEL_MASK;

	return COLOR_SET_COLOR((uint32_t)roundf(a), (uint32_t)roundf(r), (uint32_t)roundf(g), (uint32_t)roundf(b), ARGB8888);
}

static void _apply_path_animation_color(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_color_t for sure
	vg_animation_color_t* animation_color = (vg_animation_color_t*)animation;

	// cppcheck-suppress [misra-c2012-11.5] data is a 32-bit array for sure
	uint32_t* color = (uint32_t*)data;
	if (!last){
		vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
		float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
		*color = _interpolate_color(transformRatio, animation_color->start_color, animation_color->end_color);
	}
	else {
		*color = animation_color->end_color;
	}
}

static void _apply_path_animation_alpha(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_alpha_t for sure
	vg_animation_alpha_t* animation_alpha = (vg_animation_alpha_t*)animation;

	// cppcheck-suppress [misra-c2012-11.5] data is a 8-bit array for sure
	uint8_t* alpha = (uint8_t*)data;
	if (!last){
		vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
		float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
		float end_alpha = ((uint8_t)0xff & animation_alpha->end_alpha);
		float start_alpha = ((uint8_t)0xff & animation_alpha->start_alpha);
		float result = (end_alpha * transformRatio) + (start_alpha * (1.0f - transformRatio));
		*alpha = (uint8_t)result;
	}
	else {
		*alpha = animation_alpha->end_alpha;
	}
}

/*
 * @brief Merges 2 paths with same commands.
 *
 * For each source path points a new point is calculated based on the source paths points
 * coordinates and a ratio.
 * If ratio = 0, resulting point will equal the first path point.
 * If ratio = 1, resulting point will equal the second path point.
 *
 * @param[out] pathDest: the path array to update.
 * @param[in]  pathSrc1: the first path array to merge.
 * @param[in]  pathSrc2: the second path array to merge.
 * @param[in]  ratio: the merge ratio between paths points.
 */
static void _interpolate_paths(vg_lite_path_t* pathDest, vg_path_t* pathSrc1, vg_path_t* pathSrc2, float ratio) {

	float remaining = (1.f - ratio);

	// Compute bounds
	float fSrc1 = pathSrc1->bounding_box[0];
	float fSrc2 = pathSrc2->bounding_box[0];
	float fDest = MEJ_MIN(fSrc1, fSrc2);
	pathDest->bounding_box[0] = fDest;

	fSrc1 = pathSrc1->bounding_box[1];
	fSrc2 = pathSrc2->bounding_box[1];
	fDest = MEJ_MIN(fSrc1, fSrc2);
	pathDest->bounding_box[1] = fDest;

	fSrc1 = pathSrc1->bounding_box[2];
	fSrc2 = pathSrc2->bounding_box[2];
	fDest = MEJ_MAX(fSrc1, fSrc2);
	pathDest->bounding_box[2] = fDest;

	fSrc1 = pathSrc1->bounding_box[3];
	fSrc2 = pathSrc2->bounding_box[3];
	fDest = MEJ_MAX(fSrc1, fSrc2);
	pathDest->bounding_box[3] = fDest;

	// Compute commands
	uint32_t* dataDest = (uint32_t*)pathDest->path;
	// cppcheck-suppress [misra-c2012-11.5] path is a 32-bit array for sure
	uint32_t* dataSrc1 = (uint32_t*)get_path_data_addr(pathSrc1);
	// cppcheck-suppress [misra-c2012-11.5] path is a 32-bit array for sure
	uint32_t* dataSrc2 = (uint32_t*)get_path_data_addr(pathSrc2);

	// cppcheck-suppress [misra-c2012-11.4] path is a 32-bit array for sure
	uint32_t* dataDestEnd = (uint32_t*)(((uint32_t)pathDest->path) + pathSrc1->length);

	while (dataDest < dataDestEnd) {

		uint32_t cmdSrc1 = *dataSrc1;

		uint32_t nb_parameters = MICROVG_PATH_get_command_parameter_number(cmdSrc1);

		*dataDest = cmdSrc1;
		dataSrc1++;
		dataSrc2++;
		dataDest++;

		for(uint32_t j=0; j<nb_parameters;j++ ){

			fSrc1 = UINT32_t_TO_JFLOAT(*dataSrc1);
			fSrc2 = UINT32_t_TO_JFLOAT(*dataSrc2);
			fDest = (remaining * fSrc1) + (ratio * fSrc2);

			*dataDest = JFLOAT_TO_UINT32_t(fDest);

			dataSrc1++;
			dataSrc2++;
			dataDest++;
		}
	}
}

static void _apply_path_animation_path_data(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_path_data_t for sure
	vg_animation_path_data_t* animation_path_data = (vg_animation_path_data_t*)animation;
	vg_lite_path_t* dest = (vg_lite_path_t*)data;

	if (!last) {

		// merge from and to in destination
		vg_path_interpolator_t* interpolator = get_path_interpolator_addr(animation->offset_path_interpolator, memory_offset);
		float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
		_interpolate_paths(
				dest,
				get_path_addr(animation_path_data->from, memory_offset),
				get_path_addr(animation_path_data->to, memory_offset),
				transformRatio
		);
	}
	else {
		// last path (== path "to")
		vg_path_t* to = get_path_addr(animation_path_data->to, memory_offset);
		dest->path = get_path_data_addr(to);
		(void)memcpy((void*)&(dest->bounding_box), (void*)&(to->bounding_box), (size_t)4 * sizeof(float));
	}

	// the command buffer length is now equal to the animation path length
	dest->path_length = get_path_addr(animation_path_data->from, memory_offset)->length;
}

static void _apply_get_path_animation_path_length(vg_animation_header_t* animation, void* data, jlong atTime, size_t memory_offset, bool last) {
	(void)atTime;
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_path_data_t for sure
	vg_animation_path_data_t* animation_path_data = (vg_animation_path_data_t*)animation;
	// cppcheck-suppress [misra-c2012-11.5] data is a uint32_t* for sure
	uint32_t* path_length = (uint32_t*)data;
	*path_length = MEJ_MAX(*path_length, get_path_addr(animation_path_data->from, memory_offset)->length);
}

static void _apply_animation(vg_animation_header_t* animation, void* data, jlong atTime, VG_RAW_apply_animation_t _animation_fct, size_t memory_offset) {
	int32_t begin = animation->begin;
	int32_t duration = animation->duration;
	int32_t keep_duration = animation->keep_duration;

	if (atTime >= begin) {
		if (atTime < (begin + duration)) {
			// the current frame isn't the last one
			_animation_fct(animation, data, (atTime - begin), memory_offset, false);
		} else if (atTime < (begin + duration + keep_duration) || (-1 == keep_duration)) {
			// the final frame reached
			_animation_fct(animation, data, duration, memory_offset, true);
		} else {
			// nothing to animate
		}
	}
}

static void* _apply_group_animations_translate(float* matrix, jlong elapsed_time, vg_animation_translate_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_translate_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)matrix, elapsed_time, &_apply_group_animation_translate, memory_offset);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _apply_group_animations_translate_xy(float* matrix, jlong elapsed_time, vg_animation_translate_xy_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_translate_xy_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)matrix, elapsed_time, &_apply_group_animation_translate_xy, memory_offset);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _apply_group_animations_rotate(float* matrix, jlong elapsed_time, vg_animation_rotate_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_rotate_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)matrix, elapsed_time, &_apply_group_animation_rotate, memory_offset);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _apply_group_animations_scale(float* matrix, jlong elapsed_time, vg_animation_scale_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_scale_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)matrix, elapsed_time, &_apply_group_animation_scale, memory_offset);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _apply_path_animations_color(uint32_t* color, jlong elapsed_time, vg_animation_color_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_color_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)color, elapsed_time, &_apply_path_animation_color, memory_offset);
		anims++; // go to next animation
	}
	*color &= 0xffffff;
	return (void*)anims;
}

static void* _derive_path_animations_color(vg_animation_color_t* animations, uint32_t size, const float color_matrix[]) {
	vg_animation_color_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		anims->start_color = _filter_color(anims->start_color, color_matrix);
		anims->end_color = _filter_color(anims->end_color, color_matrix);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _apply_path_animations_alpha(uint8_t* alpha, jlong elapsed_time, vg_animation_alpha_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_alpha_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)alpha, elapsed_time, &_apply_path_animation_alpha, memory_offset);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _derive_path_animations_alpha(vg_animation_alpha_t* animations, uint32_t size, const float color_matrix[]) {
	vg_animation_alpha_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		anims->start_alpha = _filter_alpha(anims->start_alpha, color_matrix);
		anims->end_alpha = _filter_alpha(anims->end_alpha, color_matrix);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void* _apply_path_animations_path_data(vg_lite_path_t* path, jlong elapsed_time, vg_animation_path_data_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_path_data_t* anims = animations;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)path, elapsed_time, &_apply_path_animation_path_data, memory_offset);
		anims++; // go to next animation
	}
	return (void*)anims;
}

static void _draw_path(void* target, vg_lite_path_t* p_render_path, vg_block_path_t* header, uint32_t color) {
	uint32_t c = color;
	VG_DRAWER_update_color(target, &c);
	VG_DRAWER_draw_path(target, p_render_path, header->fill_rule, &render_matrix, VG_LITE_BLEND_SRC_OVER, c);
}

static uint32_t _get_path_animations_path_max_length(vg_lite_path_t* path, jlong elapsed_time, vg_animation_path_data_t* animations, uint32_t size, size_t memory_offset) {
	vg_animation_path_data_t* anims = animations;
	uint32_t length = path->path_length;
	for(uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((vg_animation_header_t*)anims, (void*)&length, elapsed_time, &_apply_get_path_animation_path_length, memory_offset);
		anims++; // go to next animation
	}
	return length;
}

/*
 * @brief path is in ROM (RAW): have to copy the path in a local structure and update the address
 * (replace the path data offset by the absolute address in ROM).
 */
static vg_lite_path_t* _prepare_render_path(vg_path_t* image_path, size_t memory_offset) {
	vg_lite_path_t* p_render_path = &render_path;
	vg_path_t* path = get_path_addr(image_path, memory_offset);
	vg_lite_init_path(
			p_render_path,
			path->format,
			VG_LITE_HIGH,
			path->length,
			get_path_data_addr(path),
			path->bounding_box[0],
			path->bounding_box[1],
			path->bounding_box[2],
			path->bounding_box[3]
	);
	p_render_path->path_type = VG_LITE_DRAW_FILL_PATH;
	return p_render_path;
}

static vg_lite_linear_gradient_t* _prepare_render_gradient(void* target, vg_operation_gradient_t* op, uint8_t alpha, const float color_matrix[], bool draw_gradient_flushed, size_t memory_offset) {

	// copy the gradient content but not the image that renders the gradient
	size_t copy_size = sizeof(vg_lite_linear_gradient_t) - sizeof(vg_lite_buffer_t);

	// copy op's gradient in a local gradient to apply the opacity
	vg_lite_linear_gradient_t local_gradient;
	vg_lite_linear_gradient_t* op_gradient = get_gradient_addr(op->gradient, memory_offset);
	(void)memcpy(&local_gradient, op_gradient, copy_size);
	_prepare_render_gradient_colors(&local_gradient, alpha, color_matrix);

	vg_lite_linear_gradient_t* p_render_gradient = &render_gradient;
	if (0 != memcmp(p_render_gradient, &local_gradient, copy_size)) {
		// not same gradient than previous: have to:

		// 1- flush the previous gradient drawings because we will update the shared gradient's image
		if (!draw_gradient_flushed) {
			DISPLAY_VGLITE_start_operation(false);
		}

		// 2- copy the new gradient data in shared gradient
		(void)memcpy(p_render_gradient, &local_gradient, copy_size);

		// 3- update the shared gradient's image.
		VG_DRAWER_update_gradient(target, p_render_gradient);
	}
	// else: same gradient than previous: no need to update gradient's image again

	// update the gradient's matrix
	LLVG_MATRIX_IMPL_multiply(
			MAP_VGLITE_MATRIX(&(p_render_gradient->matrix)),
			MAP_VGLITE_MATRIX(&render_matrix),
			MAP_VGLITE_MATRIX(&op_gradient->matrix)
	);

	return p_render_gradient;
}

static uint8_t* _animate_path(vg_lite_path_t* p_render_path, jlong elapsed_time, vg_animation_path_data_t* first_animation, uint32_t nb_animations, size_t memory_offset) {

	uint8_t* command_buffer;

	if ((uint8_t)0 < nb_animations) {

		// allocate a command buffer where "from" and "to" will be merged
		uint32_t size = _get_path_animations_path_max_length(&render_path, elapsed_time, first_animation, nb_animations, memory_offset);
		command_buffer = _alloc_data(size);

		if (NULL != command_buffer) {

			void* original_commands = p_render_path->path;
			int32_t original_commands_length = p_render_path->path_length;

			// update the destination commands buffer address
			p_render_path->path = (void*)command_buffer;
			p_render_path->path_length = 0;

			// update path commands, the path length and the bounding box
			(void)_apply_path_animations_path_data(p_render_path, elapsed_time, first_animation, nb_animations, memory_offset);

			if (0 == p_render_path->path_length) {

				// no animation has been applied: restore the original path
				p_render_path->path = original_commands;
				p_render_path->path_length = original_commands_length;

				// free the useless command buffer
				_free_data(command_buffer);
				command_buffer = NULL;
			}
		}
		// else not enough memory in heap: use original path
	}
	else {
		// nothing to do: use path asis
		command_buffer = NULL;
	}

	return command_buffer;
}

static inline void _animation_path_end(uint8_t* command_buffer) {
	if (NULL != command_buffer) {
		_free_data(command_buffer);
	}
}

static inline vg_block_t* _go_to_next_block(vg_block_t* block, size_t size) {
	// cppcheck-suppress [misra-c2012-11.3,misra-c2012-18.4] cast the block in a u8 address and points to the next block
	return (vg_block_t*)(((uint8_t*)block) + size);
}

static bool _draw_raw_image(void* target, vg_block_t* first_block, size_t memory_offset, jint x, jint y, jfloat* matrix, uint32_t alpha, jlong elapsedTime, const float color_matrix[]) {

	vg_block_t* block = first_block;
	bool draw_gradient_flushed = true; // no drawing with gradient has been added yet
	bool animate = elapsedTime >= 0;
	jlong elapsed_time = animate ? elapsedTime : 0; // default time when there is no animation

	// create the image translation matrix
	LLVG_MATRIX_IMPL_setTranslate(MAP_VGLITE_MATRIX(&translate_matrix), x, y);

	// create initial transformation with application's matrix
	first_transformation.previous = NULL;
	(void)memcpy((void*)&(first_transformation.matrix), (void*)matrix, sizeof(vg_lite_matrix_t));
	vg_transformation_t* p_render_transformation = &first_transformation;

	// get the transformation to draw the first path: image transformation + translation
	_apply_translation(p_render_transformation);

	bool done = false;
	while(!done) {

		switch(block->kind) {

		case VG_BLOCK_LAST:{
			done = true;
		}
		break;

		case VG_BLOCK_GROUP_TRANSFORM:{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_group_transform_t for sure
			vg_block_group_transform_t* op = (vg_block_group_transform_t*)block;

			// copy current transformation
			vg_transformation_t* parent_transformation = p_render_transformation;
			p_render_transformation = _store_vg_transformation(parent_transformation);

			if (NULL != p_render_transformation){
				p_render_transformation->previous = parent_transformation;

				// concatenate first transformation with new operation transformation
				LLVG_MATRIX_IMPL_concatenate(MAP_VGLITE_MATRIX(&(p_render_transformation->matrix)), MAP_VGLITE_MATRIX(&(op->matrix)));

				// create matrix "transformation + image translation" (prevent to do it for each path of current group)
				_apply_translation(p_render_transformation);
			}
			// else: cannot apply a new transformation: keep current one

			block = _go_to_next_block(block, sizeof(vg_block_group_transform_t));
		}
		break;

		case VG_BLOCK_GROUP_END:{
			if (NULL != p_render_transformation->previous){

				// back to parent transformation
				vg_transformation_t* transformation_to_remove = p_render_transformation;
				p_render_transformation = transformation_to_remove->previous;
				_free_vg_transformation(transformation_to_remove);

				// restore the final transformation (transformation + image translation)
				_apply_translation(p_render_transformation);
			}

			block = _go_to_next_block(block, sizeof(vg_block_group_end_t));
		}
		break;

		case VG_BLOCK_GROUP_ANIMATE:{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_group_animation_t for sure
			vg_block_group_animation_t* op = (vg_block_group_animation_t*)block;

			// copy current transformation
			vg_transformation_t* parent_transformation = p_render_transformation;
			p_render_transformation = _store_vg_transformation(parent_transformation);

			if (NULL != p_render_transformation){
				p_render_transformation->previous = parent_transformation;

				float* mapped_matrix = MAP_VGLITE_MATRIX(&(p_render_transformation->matrix));
				// cppcheck-suppress [misra-c2012-18.4] points after the operation
				void* animations_offset = ((uint8_t*)op) + sizeof(vg_block_group_animation_t);

				if (animate) {
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_translate_t for sure
					animations_offset = _apply_group_animations_translate(mapped_matrix, elapsed_time, (vg_animation_translate_t*)animations_offset, op->nb_translates, memory_offset);
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_translate_xy_t for sure
					animations_offset = _apply_group_animations_translate_xy(mapped_matrix, elapsed_time, (vg_animation_translate_xy_t*)animations_offset, op->nb_translates_xy, memory_offset);
				}

				LLVG_MATRIX_IMPL_translate(mapped_matrix, op->translate_x, op->translate_y);

				if (animate) {
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_rotate_t for sure
					animations_offset = _apply_group_animations_rotate(mapped_matrix, elapsed_time, (vg_animation_rotate_t*)animations_offset, op->nb_rotates, memory_offset);
				}

				LLVG_MATRIX_IMPL_translate(mapped_matrix, op->pivot_x, op->pivot_y);
				LLVG_MATRIX_IMPL_rotate(mapped_matrix, op->rotation);
				LLVG_MATRIX_IMPL_translate(mapped_matrix, -op->pivot_x, -op->pivot_y);

				if (animate) {
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_scale_t for sure
					animations_offset = _apply_group_animations_scale(mapped_matrix, elapsed_time, (vg_animation_scale_t*)animations_offset, op->nb_scales, memory_offset);
				}

				LLVG_MATRIX_IMPL_translate(mapped_matrix, op->pivot_x, op->pivot_y);
				LLVG_MATRIX_IMPL_scale(mapped_matrix, op->scale_x, op->scale_y);
				LLVG_MATRIX_IMPL_translate(mapped_matrix, -op->pivot_x, -op->pivot_y);

				// create matrix "transformation + image translation" (prevent to do it for each path of current group)
				_apply_translation(p_render_transformation);
			}
			// else: cannot apply a new transformation: keep current one

			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR_ANIMATE: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_animate_color_t for sure
			vg_operation_animate_color_t* op = (vg_operation_animate_color_t*)block;
			// cppcheck-suppress [misra-c2012-18.4] points after the operation
			void* animations_offset = ((uint8_t*)op) + sizeof(vg_operation_animate_color_t);

			// update color & alpha
			uint32_t color = op->path.color;
			uint8_t color_alpha = COLOR_GET_CHANNEL(color, ARGB8888, ALPHA);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_color_t for sure
			animations_offset = _apply_path_animations_color(&color, elapsed_time, (vg_animation_color_t*)animations_offset, op->nb_colors, memory_offset);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_alpha_t for sure
			animations_offset = _apply_path_animations_alpha(&color_alpha, elapsed_time, (vg_animation_alpha_t*)animations_offset, op->nb_alphas, memory_offset);
			color |= (color_alpha << COLOR_ARGB8888_ALPHA_OFFSET);
			color = _prepare_render_color(color, alpha, color_matrix);

			// prepare & animate the path
			vg_lite_path_t* p_render_path = _prepare_render_path(op->path.header.path, memory_offset);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_path_data_t for sure
			uint8_t* command_buffer = _animate_path(p_render_path, elapsed_time, (vg_animation_path_data_t*)animations_offset, op->nb_path_datas, memory_offset);

			_draw_path(target, p_render_path, &(op->path.header), color);

			_animation_path_end(command_buffer);

			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_GRADIENT_ANIMATE: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_animate_gradient_t for sure
			vg_operation_animate_gradient_t* op = (vg_operation_animate_gradient_t*)block;
			// cppcheck-suppress [misra-c2012-18.4] points after the operation
			void* animations_offset = ((uint8_t*)op) + sizeof(vg_operation_animate_gradient_t);

			uint32_t alpha_to_apply = alpha;
			if ((uint8_t)0 < op->nb_alphas) {
				// update alpha
				uint8_t alpha_anim = (uint8_t)0xff;
				// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_alpha_t for sure
				animations_offset = _apply_path_animations_alpha(&alpha_anim, elapsed_time, (vg_animation_alpha_t*)animations_offset, op->nb_alphas, memory_offset);
				alpha_to_apply *= alpha_anim;
				alpha_to_apply /= 255;
			}

			// prepare the gradient
			vg_lite_linear_gradient_t* p_render_gradient = _prepare_render_gradient(target, &(op->path), alpha_to_apply, color_matrix, draw_gradient_flushed, memory_offset);

			// prepare & animate the path
			vg_lite_path_t* p_render_path = _prepare_render_path(op->path.header.path, memory_offset);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_path_data_t for sure
			uint8_t* command_buffer = _animate_path(p_render_path, elapsed_time, (vg_animation_path_data_t*)animations_offset, op->nb_path_datas, memory_offset);

			VG_DRAWER_draw_gradient(target, p_render_path, op->path.header.fill_rule, &render_matrix, p_render_gradient, VG_LITE_BLEND_SRC_OVER);
			draw_gradient_flushed = false;

			_animation_path_end(command_buffer);

			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_color_t for sure
			vg_operation_color_t* op = (vg_operation_color_t*)block;
			vg_lite_path_t* p_render_path = _prepare_render_path(op->header.path, memory_offset);
			uint32_t color = _prepare_render_color(op->color, alpha, color_matrix);
			_draw_path(target, p_render_path, &(op->header), color);
			block = _go_to_next_block(block, sizeof(vg_operation_color_t));
		}
		break;

		case VG_BLOCK_PATH_GRADIENT: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_gradient_t for sure
			vg_operation_gradient_t* op = (vg_operation_gradient_t*)block;

			// prepare the gradient
			vg_lite_path_t* p_render_path = _prepare_render_path(op->header.path, memory_offset);
			vg_lite_linear_gradient_t* p_render_gradient = _prepare_render_gradient(target, op, alpha, color_matrix, draw_gradient_flushed, memory_offset);

			VG_DRAWER_draw_gradient(target, p_render_path, op->header.fill_rule, &render_matrix, p_render_gradient, VG_LITE_BLEND_SRC_OVER);
			draw_gradient_flushed = false;

			block = _go_to_next_block(block, sizeof(vg_operation_gradient_t));
		}
		break;

		default:
			MEJ_LOG_ERROR_MICROVG("unknown operation: %u\n", block->kind);
			done = true;
			break;
		}
	}

	return VG_BLOCK_LAST == block->kind;
}

static void _derive_raw_image(vg_block_t* first_block, size_t memory_offset, const float color_matrix[]) {

	bool done = false;
	vg_block_t* block = first_block;

	while(!done) {

		switch(block->kind) {

		case VG_BLOCK_LAST:{
			done = true;
		}
		break;

		case VG_BLOCK_GROUP_TRANSFORM: {
			// nothing to do
			block = _go_to_next_block(block, sizeof(vg_block_group_transform_t));
		}
		break;

		case VG_BLOCK_GROUP_END: {
			// nothing to do
			block = _go_to_next_block(block, sizeof(vg_block_group_end_t));
		}
		break;

		case VG_BLOCK_GROUP_ANIMATE: {
			// nothing to do
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_group_animation_t for sure
			vg_block_group_animation_t* op = (vg_block_group_animation_t*)block;
			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR_ANIMATE: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_animate_color_t for sure
			vg_operation_animate_color_t* op = (vg_operation_animate_color_t*)block;
			// cppcheck-suppress [misra-c2012-18.4] points after the operation
			void* animations_offset = ((uint8_t*)op) + sizeof(vg_operation_animate_color_t);
			op->path.color = _filter_color(op->path.color, color_matrix);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_color_t for sure
			animations_offset = _derive_path_animations_color((vg_animation_color_t*)animations_offset, op->nb_colors, color_matrix);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_alpha_t for sure
			animations_offset = _derive_path_animations_alpha((vg_animation_alpha_t*)animations_offset, op->nb_alphas, color_matrix);
			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_GRADIENT_ANIMATE: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_animate_gradient_t for sure
			vg_operation_animate_gradient_t* op = (vg_operation_animate_gradient_t*)block;
			vg_lite_linear_gradient_t* gradient = get_gradient_addr(op->path.gradient, memory_offset);
			uint32_t* colors = (uint32_t*)gradient->colors;
			for(int i = 0; i < gradient->count; i++) {
				colors[i] = _filter_color(colors[i], color_matrix);
			}
			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_color_t for sure
			vg_operation_color_t* op = (vg_operation_color_t*)block;
			op->color = _filter_color(op->color, color_matrix);
			block = _go_to_next_block(block, sizeof(vg_operation_color_t));
		}
		break;

		case VG_BLOCK_PATH_GRADIENT: {
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_operation_gradient_t for sure
			vg_operation_gradient_t* op = (vg_operation_gradient_t*)block;
			vg_lite_linear_gradient_t* gradient = get_gradient_addr(op->gradient, memory_offset);
			uint32_t* colors = (uint32_t*)gradient->colors;
			for(int i = 0; i < gradient->count; i++) {
				colors[i] = _filter_color(colors[i], color_matrix);
			}
			block = _go_to_next_block(block, sizeof(vg_operation_gradient_t));
		}
		break;

		default:
			MEJ_LOG_ERROR_MICROVG("unknown operation: %u\n", block->kind);
			done = true;
			break;
		}
	}
}

static void _initialize(void) {
	if (!initialiazed) {

		// prepare the allocator
		BESTFIT_ALLOCATOR_new(&allocator_instance);
		// cppcheck-suppress [misra-c2012-11.4,misra-c2012-18.4] HEAP_START and HEAP_END denote heap addresses
		BESTFIT_ALLOCATOR_initialize(&allocator_instance, (uint32_t) HEAP_START, (uint32_t) HEAP_END);

		// prepare the gradient used by all draw_gradient
		(void)memset(&render_gradient, 0, sizeof(vg_lite_linear_gradient_t));
		(void)vg_lite_init_grad(&render_gradient); // always success
		render_gradient.image.format = VG_LITE_RGBA8888; // fix color format

		initialiazed = true;
	}
}

static inline bool _is_raw_image(const uint8_t image[]) {
	return 0 == memcmp(image, signature, sizeof(signature));
}

static inline void _get_image_parameters(vector_image_t* image, vg_block_t** first_block, size_t* memory_offset) {
	// cppcheck-suppress [misra-c2012-11.4] save the image address as the memory offset to apply on each address
	*memory_offset = (size_t)image;
	// cppcheck-suppress [misra-c2012-11.4] offset points to the first block for sure
	*first_block = (vg_block_t*)(image->offset_first_block + *memory_offset);
}

static inline void __store_image_resource(vector_image_t* image, int32_t resource_size, SNIX_resource* resource) {
	resource->data = (void*) image;
	resource->size = resource_size;
}

static inline void __store_image_metadata(const vector_image_t* image, jint extra_flags, jint* metadata) {
	metadata[RAW_OFFSET_F32_WIDTH] = JFLOAT_TO_UINT32_t(image->width);
	metadata[RAW_OFFSET_F32_HEIGHT] = JFLOAT_TO_UINT32_t(image->height);
	metadata[RAW_OFFSET_U32_DURATION] = image->flags.duration;
	metadata[RAW_OFFSET_U32_FLAGS] = (image->flags.overlapping ? RAW_FLAG_OVERLAP_PATH : 0) | extra_flags;
}

static inline void __store_image_resource_and_metadata(vector_image_t* image, int32_t resource_size, jint extra_flags, SNIX_resource* resource, jint* metadata) {
	__store_image_resource(image, resource_size, resource);
	__store_image_metadata(image, extra_flags, metadata);
}

#if VG_FEATURE_RAW_EXTERNAL

/*
 * @brief Loads a RAW vector image from external memory
 *
 * The resource is opened from external memory if it exists. Its address, size and metadata are stored in the output parameters.
 * If the resource exists in a byte-addressable area, the output address will be that of this memory area.
 * Otherwise, a RAM area will be allocated on the MicroUI image heap, the resource will be copied into that area, and the output address will be that of the newly allocated memory area.
 * In the latter case, metadata will contain a RAW_FLAG_FREE_MEMORY_ON_CLOSE. The caller is responsible for properly deallocating the memory.
 *
 * @param[in] image_name: name of the image to be opened
 * @param[in] allocation_allowed: true to allow an allocation in the images heap, false when forbidden
 * @param[out] resource: structure to save the address and size of the image
 * @param[out] metadata: integer array to save the width, height, animation duration and flags of the image
 *
 * @return IMAGE_SUCCESS on a successful loading, any other value on a failure.
 */
static int __load_external_image(char const* image_name, bool allocation_allowed, SNIX_resource* resource, jint* metadata) {
	int result = IMAGE_SUCCESS;

	// Ignore the leading '/' in the path.
	char const* image_external_path = image_name;
	image_external_path ++;

	// Open the resource matching the provided path.
	RES_ID resource_id = LLEXT_RES_open(image_external_path);
	if (0 > resource_id) {
		result = IMAGE_INVALID_PATH;
	}
	else {
		// Retrieve the address and size of the resource.
		int32_t resource_size = LLEXT_RES_available(resource_id);
		int32_t base_address = LLEXT_RES_getBaseAddress(resource_id);

		vector_image_t* image = NULL;

		// Initialize the library (if needed)
		_initialize();

		if (-1 != base_address) {
			// The resource is located in a byte-addressable memory area. This will be the output address.
			image = (vector_image_t*) base_address;

			// Put the address, size and metadata in the ouptut parameters.
			__store_image_resource_and_metadata(image, resource_size, 0, resource, metadata);

			result = IMAGE_SUCCESS;
		}
		else if (!allocation_allowed) {
			// image is available but required a copy in the images heap
			result = IMAGE_INVALID_PATH;
		}
		else {
			// The resource is located in a memory area that cannot be byte-addressed.

			// Allocate memory on the MicroUI image heap to store the image. Its address will be the output address.
			image = (vector_image_t*) LLUI_DISPLAY_IMPL_image_heap_allocate(resource_size);
			if (NULL == image) {
				// Out of memory.
				result = IMAGE_OOM;
			}
			else {
				// Copy the image from the external memory into the allocated area.
				if (LLEXT_RES_OK != LLEXT_RES_read(resource_id, (void*) image, &resource_size)) {
					result = IMAGE_INVALID;
				}
				else {
					/* Put the address, size and metadata in the ouptut parameters.
					 * RAW_FLAG_FREE_MEMORY_ON_CLOSE flags the image as requiring to be deallocated.
					 */
					__store_image_resource_and_metadata(image, resource_size, RAW_FLAG_FREE_MEMORY_ON_CLOSE, resource, metadata);

					result = IMAGE_SUCCESS;
				}
			}
		}

		// Close the resource.
		LLEXT_RES_close(resource_id);
	}

	return result;
}

#endif // VG_FEATURE_RAW_EXTERNAL

// -----------------------------------------------------------------------------
// RAW Management
// -----------------------------------------------------------------------------

jint Java_ej_microvg_VectorGraphicsNatives_getImage(char* path, jint path_length, SNIX_resource* res, jint* metadata) {

	(void)path_length;

	jint ret;
	int32_t sni_ret = SNIX_get_resource(path, res);

#if VG_FEATURE_RAW_EXTERNAL
	// image not found in the application resource list
	if ((sni_ret < 0) && (IMAGE_SUCCESS == __load_external_image(path, false, res, metadata))) {
		// image found available in external memory which is byte addressable
		sni_ret = 0;
	}
#endif // VG_FEATURE_RAW_EXTERNAL

	if (sni_ret < 0) {
		ret = IMAGE_INVALID_PATH;
	}
	// cppcheck-suppress [misra-c2012-11.5] cast the resource in a u8 address
	else if (!_is_raw_image((uint8_t*)res->data)) {
		ret = IMAGE_INVALID;
	}
	else {

		_initialize();

		// cppcheck-suppress [misra-c2012-11.5] resource is a vector image for sure
		vector_image_t* image = (vector_image_t*)res->data;

		// metadata used by the Java library during image opening
		__store_image_metadata(image, 0, metadata);

		ret = IMAGE_SUCCESS;
	}
	return ret;
}

jint Java_ej_microvg_VectorGraphicsNatives_loadImage(char* path, jint path_length, SNIX_resource* res, jint* metadata) {
	jint result = Java_ej_microvg_VectorGraphicsNatives_getImage(path, path_length, res, metadata);

#if VG_FEATURE_RAW_EXTERNAL
	if (IMAGE_SUCCESS != result) {
		result = __load_external_image(path, true, res, metadata);
	}
#endif

	return result;
}

jint Java_ej_microvg_PainterNatives_drawImage(MICROUI_GraphicsContext* gc, SNIX_resource* sni_context, jint x, jint y, jfloat *matrix, jint alpha, jlong elapsed, const float color_matrix[]){

	jint ret = IMAGE_SUCCESS;

	// cppcheck-suppress [misra-c2012-11.5] sni_context is a vector image for sure
	vector_image_t* image = (vector_image_t*)sni_context->data;

	if ((alpha > (uint32_t)0)
			&& !_set_clip(gc, matrix, &x, &y, image->width, image->height)
			&& LLUI_DISPLAY_requestDrawing(gc, (SNI_callback)&Java_ej_microvg_PainterNatives_drawImage)) {

		size_t memory_offset;
		vg_block_t* first_block;
		_get_image_parameters(image, &first_block, &memory_offset);
		void* target = VG_DRAWER_configure_target(gc);
		if (_draw_raw_image(target, first_block, memory_offset, x, y, matrix, alpha, elapsed, color_matrix)) {
			// flush all operations
			LLUI_DISPLAY_setDrawingStatus(VG_DRAWER_post_operation(target, VG_LITE_SUCCESS));
		}
		else {
			LLUI_DISPLAY_setDrawingStatus(DRAWING_DONE);
			ret = IMAGE_INVALID;
		}
	}

	return ret;
}

jint Java_ej_microvg_VectorGraphicsNatives_createImage(SNIX_resource* source, SNIX_resource* dest, const float color_matrix[]) {

	dest->data = (void*)LLUI_DISPLAY_IMPL_image_heap_allocate(source->size);
	jint ret;

	if (NULL != dest->data){
		(void)memcpy(dest->data, source->data, source->size);
		dest->size = source->size;

		// cppcheck-suppress [misra-c2012-11.5] destination is a vector image for sure
		vector_image_t* image = (vector_image_t*)dest->data;

		size_t memory_offset;
		vg_block_t* first_block;
		_get_image_parameters(image, &first_block, &memory_offset);

		_derive_raw_image(first_block, memory_offset, color_matrix);

		ret = IMAGE_SUCCESS;
	}
	else {
		ret = IMAGE_OOM;
	}

	return ret;
}

void Java_ej_microvg_VectorGraphicsNatives_closeImage(SNIX_resource* res) {
	if ((NULL != res) && (NULL != res->data) && ((uint32_t)0 < res->size)) {
		// cppcheck-suppress [misra-c2012-11.5] cast the resource in a u8 address
		LLUI_DISPLAY_IMPL_image_heap_free((uint8_t* )res->data);
		res->data = NULL;
		res->size = 0;
	}
	// else: the image has been already closed
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

