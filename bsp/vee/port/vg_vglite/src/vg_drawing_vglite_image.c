/*
 * C
 *
 * Copyright 2022-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: image management. This file draws an
 * image and can fill a BufferedVectorImage.
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <string.h>
#include <math.h>
#include <assert.h>

#include <sni.h>
#include <LLUI_DISPLAY_impl.h>
#include <LLVG_MATRIX_impl.h>

#include "vg_configuration.h"
#include "ui_color.h"
#include "vg_helper.h"
#include "vg_trace.h"
#include "vg_drawing_vglite.h"
#include "ui_util.h"
#include "vg_vglite_helper.h"
#include "vg_bvi_vglite.h"

#include "vg_lite.h"
#include "vg_lite_kernel.h"
#include "fsl_debug_console.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Enables loading of vector image files from an external resource system.
 *
 * If this feature is enabled, when an image file is not available in the application classpath, the implementation
 * tries to load it from an external resource system.
 * If this feature is disabled, only the resources compiled with the application are used.
 *
 * This feature will be enabled if the Platform embeds the module "External Resource" and the BSP implements the
 * functions declared in "LLEXT_RES_impl.h".
 */
#ifndef VG_FEATURE_RAW_EXTERNAL
#define VG_FEATURE_RAW_EXTERNAL __has_include("LLEXT_RES_impl.h")
#if VG_FEATURE_RAW_EXTERNAL
#include "LLEXT_RES_impl.h"
#endif
#endif

/*
 * @brief RAW format
 */
#define LLVG_RAW_VERSION (1)

/*
 * @brief Allocator management
 */
static inline size_t ALIGN(size_t size, size_t alignment) {
	return (size + (alignment - 1u)) & ~(alignment - 1u);
}

static inline void * MALLOC(size_t size) {
	return LLUI_DISPLAY_IMPL_imageHeapAllocate(ALIGN(size, 4u));
}

#define FREE(s) (LLUI_DISPLAY_IMPL_imageHeapFree(s))
//#define DEBUG_ALLOCATOR

#define TIMERATIO_RANGE 65535
#define TIMERATIO_OFFSET ((TIMERATIO_RANGE / 2) + 1)

/*
 * @brief Gamma value typically lies between 2.2 and 2.5 for CRT monitors, Android chose 2.2.
 */
#define GAMMA_VALUE 2.2f

#define COLOR_MATRIX_WIDTH (5u)
#define COLOR_MATRIX_HEIGHT (4u)
#define COLOR_MATRIX_SIZE ((COLOR_MATRIX_WIDTH)*(COLOR_MATRIX_HEIGHT))
#define RED_OFFSET (0u)
#define GREEN_OFFSET (1u)
#define BLUE_OFFSET (2u)
#define ALPHA_OFFSET (3u)
#define CONSTANT_OFFSET (4u)
#define RED_LINE (COLOR_MATRIX_WIDTH * RED_OFFSET)
#define GREEN_LINE (COLOR_MATRIX_WIDTH * GREEN_OFFSET)
#define BLUE_LINE (COLOR_MATRIX_WIDTH * BLUE_OFFSET)
#define ALPHA_LINE (COLOR_MATRIX_WIDTH * ALPHA_OFFSET)

/*
 * @brief Copy the gradient content but not the image that renders the gradient
 *
 *  /!\ the VGLite gradient is constitued of the gradient header (colors and positions) and a VGLite image.
 *  A VGLite image is a header + a pixel buffer. This image is made at runtime by VGLite library according
 *  to the gradient data. The RAW format only embbeds the gradient header, not the "image" part to win some
 *  footprint. By consequence the gradient data to copy from ROM to RAM is:
 *  sizeof(gradient) == sizeof(vg_gradient) - sizeof(vg_lite_buffer_t)
 */
#define GRADIENT_COPY_SIZE (sizeof(vg_gradient) - sizeof(vg_lite_buffer_t))
#define GRADIENT_CMP_SIZE (GRADIENT_COPY_SIZE - sizeof(vg_transformation_matrix))

/*
 * @brief Macro to add an IMAGE event and its type.
 */
#define LOG_MICROVG_IMAGE_START(fn) LOG_MICROVG_START(LOG_MICROVG_IMAGE_ID, CONCAT_DEFINES(LOG_MICROVG_IMAGE_, fn))
#define LOG_MICROVG_IMAGE_END(fn) LOG_MICROVG_END(LOG_MICROVG_IMAGE_ID, CONCAT_DEFINES(LOG_MICROVG_IMAGE_, fn))

#define MAP_NATIVE_MATRIX MAP_VGLITE_MATRIX

// -----------------------------------------------------------------------------
// Java library constants (static final)
// -----------------------------------------------------------------------------

#define RAW_OFFSET_F32_WIDTH (0u)
#define RAW_OFFSET_F32_HEIGHT (1u)
#define RAW_OFFSET_U32_DURATION (2u)
#define RAW_OFFSET_U32_FLAGS (3u)

#define RAW_FLAG_OVERLAP_PATH (0x01)
#define RAW_FLAG_FREE_MEMORY_ON_CLOSE (0x02)

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

typedef vg_lite_matrix_t vg_transformation_matrix;
typedef vg_lite_linear_gradient_t vg_gradient;
typedef uint32_t vg_path_param;
typedef VG_DRAWING_VGLITE_draw_image_element_t draw_image_element;

/*
 * @brief RAW and BVI
 */
typedef struct vector_image vector_image_t;
typedef struct vector_buffered_image vector_buffered_image_t;

/*
 * @brief Image blocks
 */
typedef enum vg_block_kind vg_block_kind_t;
typedef struct vg_flags vg_flags_t;
typedef struct vg_block vg_block_t;
typedef struct vg_path_desc vg_path_desc_t;
typedef struct vg_block_path vg_block_path_t;
typedef struct vg_block_group_transform vg_block_group_transform_t;
typedef struct vg_block_group_animation vg_block_group_animation_t;
typedef struct vg_block_group_end vg_block_group_end_t;
typedef struct vg_block_color vg_block_color_t;
typedef struct vg_block_gradient vg_block_gradient_t;
typedef struct vg_block_bvi_color vg_block_bvi_color_t;
typedef struct vg_block_bvi_gradient vg_block_bvi_gradient_t;
typedef struct vg_block_bvi_image vg_block_bvi_image_t;
typedef struct vg_block_animate_color vg_block_animate_color_t;
typedef struct vg_block_animate_gradient vg_block_animate_gradient_t;

/*
 * @brief Image animations
 */
typedef struct vg_path_interpolator vg_path_interpolator_t;
typedef struct vg_animation_header vg_animation_header_t;
typedef struct vg_animation_translate vg_animation_translate_t;
typedef struct vg_animation_translate_xy vg_animation_translate_xy_t;
typedef struct vg_animation_rotate vg_animation_rotate_t;
typedef struct vg_animation_scale vg_animation_scale_t;
typedef struct vg_animation_color vg_animation_color_t;
typedef struct vg_animation_alpha vg_animation_alpha_t;
typedef struct vg_animation_path_data vg_animation_path_data_t;

/*
 * @brief Image drawing data
 */
typedef struct vg_transformation vg_transformation_t;
typedef struct vg_drawing vg_drawing_t;

/*
 * @brief Internal MicroJVM element to retrieve a resource in the microejapp.o.
 */
typedef struct {
	void *data;
	uint32_t size;
} SNIX_resource;

/*
 * @brief Function to update a matrix according to an animation
 *
 * @param[in/out]: inout: the data to update
 * @param[in]: elapsed_time:  the time elapsed within this animation.
 * @param[in]: last: true when the final frame is reached
 *
 * @return: nothing
 */
typedef void (* VG_RAW_apply_animation_t) (const vg_animation_header_t *animation,
                                           void *inout,
                                           jlong elapsed_time,
                                           size_t memory_offset,
                                           bool last);

// -----------------------------------------------------------------------------
// Enum
// -----------------------------------------------------------------------------

/*
 * @brief Types of image's blocks.
 */
enum vg_block_kind {
	// respect the order (hardcoded in VectorImage converter)

	// RAW & BVI
	VG_BLOCK_LAST,

	// RAW only
	VG_BLOCK_PATH_COLOR,
	VG_BLOCK_RAW_GRADIENT,
	VG_BLOCK_GROUP_TRANSFORM,
	VG_BLOCK_GROUP_ANIMATE,
	VG_BLOCK_GROUP_END,
	VG_BLOCK_PATH_COLOR_ANIMATE,
	VG_BLOCK_PATH_GRADIENT_ANIMATE,

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE
	// BVI only
	VG_BLOCK_BVI_COLOR,
	VG_BLOCK_BVI_GRADIENT,
	VG_BLOCK_BVI_IMAGE,
#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE
};

// -----------------------------------------------------------------------------
// Struct
// -----------------------------------------------------------------------------

/*
 * @brief Flags of RAW image
 *
 * (u1) overlapping
 * (u1) rom | ram (resp. 0 | 1)
 * (u1) relative | absolute (resp. 0 | 1)
 * (u5) padding
 * (u24) duration
 */
struct vg_flags {
	uint32_t duration : 24;
	// cppcheck-suppress [unusedStructMember] padding is not used
	uint32_t : 5; // padding
	uint32_t absolute : 1; // 0: addresses are relative (blocks, paths, etc.); 1: absolute
	uint32_t ram : 1; // 0: image in RAM; 1: image in ROM
	uint32_t overlapping : 1;
};

/*
 * @brief Defines an image block. A block is identified by its kind.
 */
struct vg_block {
	// block's kind (see vg_block_xxx)
	vg_block_kind_t kind;

	// warning: aligned on 1 byte is known by "sub" struct
} __attribute__((packed));

/*
 * @brief Defines the description of a path. In a RAW image, the same path can
 * be shared between several operation "draw path".
 */
struct vg_path_desc {
	float bounding_box[4];    // left, top, right, bottom
	uint32_t length;
	vg_lite_format_t format;
	int : 24; // Padding

	// + array of data of path
};

/*
 * @brief Defines a block "path"; useful for all operations that draw a path.
 */
struct vg_block_path {
	vg_block_t block;

	// common block's data for all VG drawings
	uint8_t fill_rule;
	uint8_t blend; // useless for static RAW images
	uint8_t : 8; // Padding

	vg_path_desc_t *desc;
};

/*
 * @brief Defines a block "group with transformation"
 */
struct vg_block_group_transform {
	vg_block_t block;

	uint8_t padding0;
	uint8_t padding1;
	uint8_t padding2;
	vg_transformation_matrix matrix;
};

/*
 * @brief Defines a block "group with animation"
 */
struct vg_block_group_animation {
	vg_block_t block;

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

	// + array of vg_animation_translate_t
	// + array of vg_animation_rotate_t
	// + array of vg_animation_scale_t
};

/*
 * @brief Defines a block "end of current group"
 */
struct vg_block_group_end {
	vg_block_t block;

	uint8_t padding0;
	uint8_t padding1;
	uint8_t padding2;
};

/*
 * @brief Defines a block "path with a color".
 */
struct vg_block_color {
	vg_block_path_t path;
	uint32_t color;
};

/*
 * @brief Defines a block "path with a gradient".
 */
struct vg_block_gradient {
	vg_block_path_t path;

	// /!\ the VGLite gradient is only constitued of the gradient header (colors and positions)
	// see GRADIENT_COPY_SIZE
	vg_gradient *gradient;
};

/*
 * @brief Defines a block "path with a color in a buffered image"
 */
struct vg_block_bvi_color {
	vg_block_color_t header;
	vg_transformation_matrix path_deformation;
	int32_t scissor[4];
	vg_block_t *next;
};

/*
 * @brief Defines a block "path with a gradient in a buffered image"
 */
struct vg_block_bvi_gradient {
	vg_block_gradient_t header;
	vg_transformation_matrix path_deformation;
	int32_t scissor[4];
	vg_block_t *next;
	bool shared_gradient; // if true, means the gradient is shared with another block (must not free it)
};

/*
 * @brief Defines a block "raw image in a buffered image"
 */
struct vg_block_bvi_image {
	vg_block_t block;

	uint8_t flags; // 0 == color_matrix is null
	jchar alpha; // prevent to use the mask "& 0xff"

	const vector_image_t *image;
	jfloat matrix[9];
	int32_t scissor[4];
	jfloat color_matrix[COLOR_MATRIX_SIZE];
	jint elapsed_msb; // prevent unalignment access
	jint elapsed_lsb;

	vg_block_t *next;
};

/*
 * @brief  Defines a block "animate color of a path".
 */
struct vg_block_animate_color {
	vg_block_color_t header;

	uint8_t nb_colors;
	uint8_t nb_alphas;
	uint8_t nb_path_datas;
	uint8_t padding0;

	uint16_t block_size;
	uint16_t padding1;

	// + array of vg_animation_color_t
	// + array of vg_animation_alpha_t
	// + array of vg_animation_path_data_t
};

/*
 * @brief  Defines a block "animate gradient of a path".
 */
struct vg_block_animate_gradient {
	vg_block_gradient_t header;

	uint8_t nb_alphas;
	uint8_t nb_path_datas;
	uint16_t block_size;

	// + array of vg_animation_alpha_t
	// + array of vg_animation_path_data_t
};

/*
 * @brief Defines a "path interpolator"
 */
struct vg_path_interpolator {
	uint32_t size;

	// + array of end_time_ratio (short)
	// + optional padding
	// + array of start_x (float)
	// + array of start_y (float)
};

/*
 * @brief Defines the header of an animation.
 */
struct vg_animation_header {
	int32_t duration;
	int32_t begin;
	int32_t keep_duration;
	vg_path_interpolator_t *offset_path_interpolator;
};

/*
 * @brief Defines an animation "Translate"
 */
struct vg_animation_translate {
	vg_animation_header_t header;
	float start_x;
	float start_y;
	float translation_x;
	float translation_y;
};

/*
 * @brief Defines an animation "Translate XY"
 */
struct vg_animation_translate_xy {
	vg_animation_header_t header;
	vg_path_interpolator_t *offset_path_interpolator;
};

/*
 * @brief Defines an animation "Rotate"
 */
struct vg_animation_rotate {
	vg_animation_header_t header;
	float start_angle;
	float rotation_angle;
	float start_rotation_center_x;
	float start_rotation_center_y;
	float rotation_translation_center_x;
	float rotation_translation_center_y;
};

/*
 * @brief Defines an animation "Scale"
 */
struct vg_animation_scale {
	vg_animation_header_t header;
	float scale_x_from;
	float scale_x_to;
	float scale_y_from;
	float scale_y_to;
	float pivot_x;
	float pivot_y;
};

/*
 * @brief Defines an animation "Color"
 */
struct vg_animation_color {
	vg_animation_header_t header;
	uint32_t start_color;
	uint32_t end_color;
};

/*
 * @brief Defines an animation "Alpha"
 */
struct vg_animation_alpha {
	vg_animation_header_t header;
	uint8_t start_alpha;
	uint8_t end_alpha;
};

/*
 * @brief Defines an animation "Path data"
 */
struct vg_animation_path_data {
	vg_animation_header_t header;
	vg_path_desc_t *from;
	vg_path_desc_t *to;
};

/*
 * @brief Defines a RAW image
 */
struct vector_image {
	uint32_t signature_msb;
	uint32_t signature_lsb;

	float width;
	float height;

	vg_flags_t flags;
	vg_block_t *first_block;
};

/*
 * @brief Defines a buffered image
 */
struct vector_buffered_image {
	/*
	 * @brief RAW image header
	 */
	vector_image_t header;

	/*
	 * @brief Pointer to vg_block_t* of last stored block.
	 */
	vg_block_t **latest_data;

	/*
	 * @brief Last block of image.
	 */
	vg_block_t last_block;

	/*
	 * @brief Pointer to last stored gradient in the bvi; to share same gradient
	 * between several consecutive drawings (very useful when drawing a string).
	 */
	vg_gradient *last_stored_gradient;
};

/*
 * @brief Element of a chained list: a transformation
 */
struct vg_transformation {
	struct vg_transformation *parent;
	vg_transformation_matrix matrix;
};

/*
 * @brief Data used to perform vector image drawings
 */
struct vg_drawing {
	/*
	 * @brief First image block to render.
	 */
	vg_block_t *first_block;

	/*
	 * @brief Offset to apply on each block data address.
	 */
	size_t memory_offset;

	/*
	 * @brief First transformation to apply when rendering an image. This
	 * transformation only holds the image transformation: it does not hold
	 * the final transformation: the image translation.
	 *
	 * This transformation is not stored in the heap to be sure to apply at
	 * least this transformation when drawing an image even if the heap is
	 * full.
	 */
	vg_transformation_t first_transformation;

	/*
	 * @brief At the end of the image drawing, tells if something has been
	 * drawn or not (empty or cleared image).
	 */
	bool drawing_running;
};

// -----------------------------------------------------------------------------
// Extern functions & fields
// -----------------------------------------------------------------------------

/*
 * @brief Internal MicroJVM function to retrieve a resource in the microejapp.o.
 *
 * @param[in] path: the path of the resource to retrieve
 * @param[out] resource: the resource metadata
 */
extern int32_t SNIX_get_resource(char *path, SNIX_resource *resource);

/*
 * @brief Function to retrieve the current scissor.
 */
extern uint32_t vg_lite_get_scissor(int32_t **scissor);

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

/*
 * @brief Gradient used for all drawings with gradient (only one image allocation,
 * does not alterate the original gradient's matrix and colors, etc.).
 */
static vg_gradient render_gradient;

/*
 * @brief Path used for all drawings.
 */
vg_lite_path_t render_path;

/*
 * @brief Working data to draw an image (RAW or BVI)
 */
static vg_drawing_t drawing_data = { 0 };

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

/*
 * @brief BVI destination when rendering an image in a BVI
 */
static void *bvi_target;

#endif

#ifdef DEBUG_ALLOCATOR
static uint32_t cumul = 0;
static uint32_t max = 0;
#endif

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

static inline const void * get_addr(const void *addr, size_t memory_offset) {
	assert(NULL != addr);
	// cppcheck-suppress [misra-c2012-11.5,misra-c2012-18.4] go to the expected address
	return (const void *)(((const uint8_t *)addr) + memory_offset);
}

static inline uintptr_t offset_address(uintptr_t addr, size_t offset) {
	assert(NULL != addr);
	return addr + offset;
}

static inline const void * get_path_data_addr(const vg_path_desc_t *path) {
	return get_addr(path, sizeof(*path));
}

static inline const vg_path_desc_t * get_path_addr(const vg_path_desc_t *path, size_t memory_offset) {
	// cppcheck-suppress [misra-c2012-11.5] cast to vg_path_desc_t* is valid for sure
	return (const vg_path_desc_t *)get_addr(path, memory_offset);
}

static inline const vg_gradient * get_gradient_addr(const vg_gradient *gradient, size_t memory_offset) {
	return (const vg_gradient *)get_addr(gradient, memory_offset);
}

static inline const vg_path_interpolator_t * get_path_interpolator_addr(const vg_path_interpolator_t *interpolator,
                                                                        size_t memory_offset) {
	// cppcheck-suppress [misra-c2012-11.5] cast to vg_path_interpolator_t* is valid for sure
	return (const vg_path_interpolator_t *)get_addr(interpolator, memory_offset);
}

/*
 * @brief Frees the given data from the heap or does nothing
 * when targets RO memory.
 *
 * @param[in] data: pointer on the data to free.
 */
static void _free_data(const void *data) {
	if (NULL != data) {
#ifdef DEBUG_ALLOCATOR
		// cppcheck-suppress [misra-c2012-11.5] data is aligned on 32-bit for sure
		uint32_t *addr = (uint32_t *)data;
		--addr;
		uint32_t size = *addr;
		cumul -= size;
		PRINTF("free %u\t0x%x\t(%u/%u)\n", size, addr, cumul, max);
		FREE((uint8_t *)addr);
#else
		FREE((uint8_t *)data);
#endif
	}
}

/*
 * @brief Allocates the given size in the heap.
 *
 * @param[in] size: the data's size (in bytes).
 *
 * @return the pointer to the stored data.
 */
static inline uint8_t * _alloc_data(uint32_t size) {
#ifdef DEBUG_ALLOCATOR
	size += 4u;
	cumul += size;
	max = (cumul > max) ? cumul : max;
#endif

	uint8_t *ret = MALLOC(size);
	if (NULL == ret) {
		MEJ_LOG_ERROR_MICROVG("OOM\n");
	}

#ifdef DEBUG_ALLOCATOR
	MEJ_LOG_INFO_MICROVG("alloc %u\t0x%x\t(%u/%u)\n", size, ret, cumul, max);
	*((uint32_t *)ret) = size;
	ret += 4;
#endif

	return ret;
}

/*
 * @brief Stores the given data in the heap.
 *
 * @param[in] data: pointer on the data to store.
 * @param[in] size: the data's size.
 *
 * @return the pointer to the stored data or NULL (OOM).
 */
static void * _store_data(const void *data, size_t size) {
	void *ret = _alloc_data(size);
	if (NULL != ret) {
		(void)memcpy((void *)ret, data, size);
	}
	return ret;
}

static inline void _tag_image_in_ram(vector_image_t *image) {
	image->flags.ram = 1u;
}

/*
 * @brief Tells if the image addresses (blocks, paths, etc.) are relative
 */
static inline bool _is_image_address_relative(const vector_image_t *image) {
	return 0u == image->flags.absolute;
}

/*
 * @brief Tells if the image is a buffered vector image.
 * We use the flag "absolute" which is only true for the BVI.
 */
static inline bool _is_image_bvi(const vector_image_t *image) {
	return !_is_image_address_relative(image);
}

/*
 * @brief Stores the given transformation in the heap.
 *
 * @param[in] transformation: pointer on the transformation to store.
 *
 * @return the pointer to the stored transformation or NULL (OOM).
 */
static vg_transformation_t * _store_vg_transformation(const vg_transformation_t *transformation) {
	// cppcheck-suppress [misra-c2012-11.3] the stored data is a vg_transformation_t for sure
	return (vg_transformation_t *)_store_data(transformation, sizeof(vg_transformation_t));
}

static void _free_vg_transformation(const vg_transformation_t *transformation) {
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
static float _filter_color_component(float red, float green, float blue, float alpha, const float color_matrix[],
                                     uint32_t start_index) {
	float redFactor = color_matrix[start_index + (uint32_t)RED_OFFSET] * red;
	float greenFactor = color_matrix[start_index + (uint32_t)GREEN_OFFSET] * green;
	float blueFactor = color_matrix[start_index + (uint32_t)BLUE_OFFSET] * blue;
	float alphaFactor = color_matrix[start_index + (uint32_t)ALPHA_OFFSET] * alpha;
	float hardcodedColor = color_matrix[start_index + (uint32_t)CONSTANT_OFFSET];
	float factors = redFactor + greenFactor + blueFactor + alphaFactor + hardcodedColor;
	return MIN(MAX(0.0f, factors), 255.0f);
}

static uint32_t _filter_color(uint32_t color, const float color_matrix[]) {
	uint32_t red = COLOR_GET_CHANNEL(color, ARGB8888, RED);
	uint32_t green = COLOR_GET_CHANNEL(color, ARGB8888, GREEN);
	uint32_t blue = COLOR_GET_CHANNEL(color, ARGB8888, BLUE);
	uint32_t alpha = COLOR_GET_CHANNEL(color, ARGB8888, ALPHA);

	uint8_t resultAlpha = (uint8_t)_filter_color_component(red, green, blue, alpha, color_matrix, ALPHA_LINE);
	uint8_t resultBlue = (uint8_t)_filter_color_component(red, green, blue, alpha, color_matrix, BLUE_LINE);
	uint8_t resultGreen = (uint8_t)_filter_color_component(red, green, blue, alpha, color_matrix, GREEN_LINE);
	uint8_t resultRed = (uint8_t)_filter_color_component(red, green, blue, alpha, color_matrix, RED_LINE);

	return COLOR_SET_COLOR(resultAlpha, resultRed, resultGreen, resultBlue, ARGB8888);
}

static void _filter_gradient(vg_gradient *gradient, const float color_matrix[]) {
	uint32_t *colors = (uint32_t *)gradient->colors;
	for (int i = 0; i < gradient->count; i++) {
		colors[i] = _filter_color(colors[i], color_matrix);
	}
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
	} else if (alpha != (uint32_t)0xff) {
		ret = VG_HELPER_apply_alpha(color, alpha);
	} else {
		// nothing to change
	}
	return ret;
}

static void _prepare_gradient_colors(vg_gradient *gradient, uint32_t alpha, const float color_matrix[]) {
	if (NULL != color_matrix) {
		assert((uint32_t)0xff == alpha); // filter mode does not use global alpha
		_filter_gradient(gradient, color_matrix);
	} else if (alpha != (uint32_t)0xff) {
		uint32_t *colors = (uint32_t *)gradient->colors;
		for (int i = 0; i < gradient->count; i++) {
			colors[i] = VG_HELPER_apply_alpha(colors[i], alpha);
		}
	} else {
		// nothing to change
	}
}

static float _get_path_interpolator_at_time(const vg_path_interpolator_t *interpolator, float time,
                                            bool apply_x_transformation) {
	// retrieve arrays addresses
	const uint8_t *data_offset = (const uint8_t *)interpolator;
	// cppcheck-suppress [misra-c2012-18.4] points after the interpolator
	data_offset += sizeof(vg_path_interpolator_t);
	// cppcheck-suppress [misra-c2012-11.3] data_offset points on a 16-bit array for sure
	const int16_t *end_time_ratio = (const int16_t *)data_offset;
	uint32_t ratio_array_length = interpolator->size + 1u;
	ratio_array_length &= ~1;
	// cppcheck-suppress [misra-c2012-18.4] points after the ratio array
	data_offset += ratio_array_length * sizeof(int16_t);
	// cppcheck-suppress [misra-c2012-11.3,invalidPointerCast] data_offset points on a float array for sure
	const float *start_x = (const float *)data_offset;
	// cppcheck-suppress [misra-c2012-18.4] points after the start X array
	data_offset += interpolator->size * sizeof(float);
	// cppcheck-suppress [misra-c2012-11.3,invalidPointerCast] data_offset points on a float array for sure
	const float *start_y = (const float *)data_offset;

	// get the values to apply
	const float *values = apply_x_transformation ? start_x : start_y;

	// get a ratio between 0x0000 and 0xffff (TIMERATIO_RANGE)
	int32_t time_ratio = (int32_t)(MIN(MAX(time, 0.f), 1.f) * (float)TIMERATIO_RANGE);

	int32_t origin = 0;
	float ret = 0.f;
	bool done = false;

	// Last cells of arrays store the ending positions of last segment
	for (uint32_t i = 0u; !done && (i < (interpolator->size - 1u)); i++) {
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

static float _get_transformation_ratio_at_time(const vg_animation_header_t *animation,
                                               const vg_path_interpolator_t *interpolator, jlong time) {
	float time_ratio;

	if (0 == animation->duration) {
		time_ratio = 1.f;
	} else {
		time_ratio = (float)time / animation->duration;
	}

	return _get_path_interpolator_at_time(interpolator, time_ratio, true);
}

static void _apply_group_animation_translate(const vg_animation_header_t *animation, void *data, jlong atTime,
                                             size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_translate_t for sure
	const vg_animation_translate_t *animation_translate = (const vg_animation_translate_t *)animation;
	const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
	                                                                        memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float *matrix = (float *)data;
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	LLVG_MATRIX_IMPL_translate(matrix,
	                           animation_translate->start_x + (animation_translate->translation_x * transformRatio),
	                           animation_translate->start_y + (animation_translate->translation_y * transformRatio));
}

static void _apply_group_animation_translate_xy(const vg_animation_header_t *animation, void *data, jlong atTime,
                                                size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_translate_xy_t for sure
	const vg_animation_translate_xy_t *animation_translate = (const vg_animation_translate_xy_t *)animation;
	const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
	                                                                        memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float *matrix = (float *)data;
	const vg_path_interpolator_t *translation =
		get_path_interpolator_addr(animation_translate->offset_path_interpolator, memory_offset);
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	float translateX = _get_path_interpolator_at_time(translation, transformRatio, true);
	float translateY = _get_path_interpolator_at_time(translation, transformRatio, false);
	LLVG_MATRIX_IMPL_translate(matrix, translateX, translateY);
}

static void _apply_group_animation_rotate(const vg_animation_header_t *animation, void *data, jlong atTime,
                                          size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_rotate_t for sure
	const vg_animation_rotate_t *animation_rotate = (const vg_animation_rotate_t *)animation;
	const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
	                                                                        memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float *matrix = (float *)data;
	float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
	LLVG_MATRIX_IMPL_translate(matrix,
	                           animation_rotate->start_rotation_center_x +
	                           (animation_rotate->rotation_translation_center_x * transformRatio),
	                           animation_rotate->start_rotation_center_y +
	                           (animation_rotate->rotation_translation_center_y * transformRatio));
	LLVG_MATRIX_IMPL_rotate(matrix,
	                        animation_rotate->start_angle + (animation_rotate->rotation_angle * transformRatio));
	LLVG_MATRIX_IMPL_translate(matrix,
	                           -animation_rotate->start_rotation_center_x -
	                           (animation_rotate->rotation_translation_center_x * transformRatio),
	                           -animation_rotate->start_rotation_center_y -
	                           (animation_rotate->rotation_translation_center_y * transformRatio));
}

static inline float _get_scale(float from, float to, float transformRatio) {
	float ret;
	if (from < to) {
		ret = (float)(from * powf(to / from, transformRatio));
	} else {
		ret = (float)(to * powf(from / to, 1.f - transformRatio));
	}
	return ret;
}

static void _apply_group_animation_scale(const vg_animation_header_t *animation, void *data, jlong atTime,
                                         size_t memory_offset, bool last) {
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_scale_t for sure
	const vg_animation_scale_t *animation_scale = (const vg_animation_scale_t *)animation;
	const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
	                                                                        memory_offset);
	// cppcheck-suppress [misra-c2012-11.5] data is a float array for sure
	float *matrix = (float *)data;
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
	float startA = ((float)(COLOR_GET_CHANNEL(start_color, ARGB8888, ALPHA))) / COLOR_ARGB8888_CHANNEL_MASK;
	float startR = ((float)(COLOR_GET_CHANNEL(start_color, ARGB8888, RED))) / COLOR_ARGB8888_CHANNEL_MASK;
	float startG = ((float)(COLOR_GET_CHANNEL(start_color, ARGB8888, GREEN))) / COLOR_ARGB8888_CHANNEL_MASK;
	float startB = ((float)(COLOR_GET_CHANNEL(start_color, ARGB8888, BLUE))) / COLOR_ARGB8888_CHANNEL_MASK;

	float endA = ((float)(COLOR_GET_CHANNEL(end_color, ARGB8888, ALPHA))) / COLOR_ARGB8888_CHANNEL_MASK;
	float endR = ((float)(COLOR_GET_CHANNEL(end_color, ARGB8888, RED))) / COLOR_ARGB8888_CHANNEL_MASK;
	float endG = ((float)(COLOR_GET_CHANNEL(end_color, ARGB8888, GREEN))) / COLOR_ARGB8888_CHANNEL_MASK;
	float endB = ((float)(COLOR_GET_CHANNEL(end_color, ARGB8888, BLUE))) / COLOR_ARGB8888_CHANNEL_MASK;

	// convert from sRGB to linear
	startR = (float)powf(startR, GAMMA_VALUE);
	startG = (float)powf(startG, GAMMA_VALUE);
	startB = (float)powf(startB, GAMMA_VALUE);

	endR = (float)powf(endR, GAMMA_VALUE);
	endG = (float)powf(endG, GAMMA_VALUE);
	endB = (float)powf(endB, GAMMA_VALUE);

	// compute the interpolated color in linear space
	float a = startA + (fraction * (endA - startA));
	float r = startR + (fraction * (endR - startR));
	float g = startG + (fraction * (endG - startG));
	float b = startB + (fraction * (endB - startB));

	// convert back to sRGB in the [0..255] range
	a = a * COLOR_ARGB8888_CHANNEL_MASK;
	r = (float)powf(r, 1.0 / GAMMA_VALUE) * COLOR_ARGB8888_CHANNEL_MASK;
	g = (float)powf(g, 1.0 / GAMMA_VALUE) * COLOR_ARGB8888_CHANNEL_MASK;
	b = (float)powf(b, 1.0 / GAMMA_VALUE) * COLOR_ARGB8888_CHANNEL_MASK;

	return COLOR_SET_COLOR((uint32_t)roundf(a), (uint32_t)roundf(r), (uint32_t)roundf(g), (uint32_t)roundf(b),
	                       ARGB8888);
}

static void _apply_path_animation_color(const vg_animation_header_t *animation, void *data, jlong atTime,
                                        size_t memory_offset, bool last) {
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_color_t for sure
	const vg_animation_color_t *animation_color = (const vg_animation_color_t *)animation;

	// cppcheck-suppress [misra-c2012-11.5] data is a 32-bit array for sure
	uint32_t *color = (uint32_t *)data;
	if (!last) {
		const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
		                                                                        memory_offset);
		float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
		*color = _interpolate_color(transformRatio, animation_color->start_color, animation_color->end_color);
	} else {
		*color = animation_color->end_color;
	}
}

static void _apply_path_animation_alpha(const vg_animation_header_t *animation, void *data, jlong atTime,
                                        size_t memory_offset, bool last) {
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_alpha_t for sure
	const vg_animation_alpha_t *animation_alpha = (const vg_animation_alpha_t *)animation;

	// cppcheck-suppress [misra-c2012-11.5] data is a 8-bit array for sure
	uint8_t *alpha = (uint8_t *)data;
	if (!last) {
		const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
		                                                                        memory_offset);
		float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
		float end_alpha = ((uint8_t)0xff & animation_alpha->end_alpha);
		float start_alpha = ((uint8_t)0xff & animation_alpha->start_alpha);
		float result = (end_alpha * transformRatio) + (start_alpha * (1.0f - transformRatio));
		*alpha = (uint8_t)result;
	} else {
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
static void _interpolate_paths(vg_lite_path_t *pathDest, const vg_path_desc_t *pathSrc1, const vg_path_desc_t *pathSrc2,
                               float ratio) {
	float remaining = (1.f - ratio);

	// Compute bounds
	float fSrc1 = pathSrc1->bounding_box[0];
	float fSrc2 = pathSrc2->bounding_box[0];
	float fDest = MIN(fSrc1, fSrc2);
	pathDest->bounding_box[0] = fDest;

	fSrc1 = pathSrc1->bounding_box[1];
	fSrc2 = pathSrc2->bounding_box[1];
	fDest = MIN(fSrc1, fSrc2);
	pathDest->bounding_box[1] = fDest;

	fSrc1 = pathSrc1->bounding_box[2];
	fSrc2 = pathSrc2->bounding_box[2];
	fDest = MAX(fSrc1, fSrc2);
	pathDest->bounding_box[2] = fDest;

	fSrc1 = pathSrc1->bounding_box[3];
	fSrc2 = pathSrc2->bounding_box[3];
	fDest = MAX(fSrc1, fSrc2);
	pathDest->bounding_box[3] = fDest;

	// Compute commands
	vg_path_param *dataDest = (vg_path_param *)pathDest->path;
	// cppcheck-suppress [misra-c2012-11.5] path is a 32-bit array for sure
	const vg_path_param *dataSrc1 = (const vg_path_param *)get_path_data_addr(pathSrc1);
	// cppcheck-suppress [misra-c2012-11.5] path is a 32-bit array for sure
	const vg_path_param *dataSrc2 = (const vg_path_param *)get_path_data_addr(pathSrc2);

	// cppcheck-suppress [misra-c2012-11.4] path is a 32-bit array for sure
	const vg_path_param *dataDestEnd = (const vg_path_param *)(((uint32_t)pathDest->path) + pathSrc1->length);

	while (dataDest < dataDestEnd) {
		uint32_t cmdSrc1 = *dataSrc1;

		uint32_t nb_parameters = VG_PATH_get_command_parameter_number(cmdSrc1);

		*dataDest = cmdSrc1;
		dataSrc1++;
		dataSrc2++;
		dataDest++;

		for (uint32_t j = 0; j < nb_parameters; j++ ) {
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

static void _apply_path_animation_path_data(const vg_animation_header_t *animation, void *data, jlong atTime,
                                            size_t memory_offset, bool last) {
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_path_data_t for sure
	const vg_animation_path_data_t *animation_path_data = (const vg_animation_path_data_t *)animation;
	vg_lite_path_t *dest = (vg_lite_path_t *)data;

	if (!last) {
		// merge from and to in destination
		const vg_path_interpolator_t *interpolator = get_path_interpolator_addr(animation->offset_path_interpolator,
		                                                                        memory_offset);
		float transformRatio = _get_transformation_ratio_at_time(animation, interpolator, atTime);
		_interpolate_paths(
			dest,
			get_path_addr(animation_path_data->from, memory_offset),
			get_path_addr(animation_path_data->to, memory_offset),
			transformRatio
			);
	} else {
		// last path (== path "to")
		const vg_path_desc_t *to = get_path_addr(animation_path_data->to, memory_offset);
		dest->path = (void *)get_path_data_addr(to);
		(void)memcpy((void *)&(dest->bounding_box), (const void *)&(to->bounding_box), 4u * sizeof(float));
	}

	// the command buffer length is now equal to the animation path length
	dest->path_length = get_path_addr(animation_path_data->from, memory_offset)->length;
}

static void _apply_get_path_animation_path_length(const vg_animation_header_t *animation, void *data, jlong atTime,
                                                  size_t memory_offset, bool last) {
	(void)atTime;
	(void)last;
	// cppcheck-suppress [misra-c2012-11.3] animations is a vg_animation_path_data_t for sure
	const vg_animation_path_data_t *animation_path_data = (const vg_animation_path_data_t *)animation;
	// cppcheck-suppress [misra-c2012-11.5] data is a uint32_t* for sure
	uint32_t *path_length = (uint32_t *)data;
	*path_length = MAX(*path_length, get_path_addr(animation_path_data->from, memory_offset)->length);
}

static void _apply_animation(const vg_animation_header_t *animation, void *data, jlong atTime,
                             VG_RAW_apply_animation_t _animation_fct, size_t memory_offset) {
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

static const void * _apply_group_animations_translate(float *matrix, jlong elapsed_time,
                                                      const vg_animation_translate_t *animations, uint32_t size,
                                                      size_t memory_offset) {
	const vg_animation_translate_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)matrix, elapsed_time,
		                 &_apply_group_animation_translate, memory_offset);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static const void * _apply_group_animations_translate_xy(float *matrix, jlong elapsed_time,
                                                         const vg_animation_translate_xy_t *animations, uint32_t size,
                                                         size_t memory_offset) {
	const vg_animation_translate_xy_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)matrix, elapsed_time,
		                 &_apply_group_animation_translate_xy, memory_offset);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static const void * _apply_group_animations_rotate(float *matrix, jlong elapsed_time,
                                                   const vg_animation_rotate_t *animations, uint32_t size,
                                                   size_t memory_offset) {
	const vg_animation_rotate_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)matrix, elapsed_time,
		                 &_apply_group_animation_rotate, memory_offset);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static const void * _apply_group_animations_scale(float *matrix, jlong elapsed_time,
                                                  const vg_animation_scale_t *animations, uint32_t size,
                                                  size_t memory_offset) {
	const vg_animation_scale_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)matrix, elapsed_time,
		                 &_apply_group_animation_scale, memory_offset);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static const void * _apply_path_animations_color(uint32_t *color, jlong elapsed_time,
                                                 const vg_animation_color_t *animations, uint32_t size,
                                                 size_t memory_offset) {
	const vg_animation_color_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)color, elapsed_time,
		                 &_apply_path_animation_color, memory_offset);
		anims++; // go to next animation
	}
	*color &= 0xffffff;
	return (const void *)anims;
}

static void * _derive_path_animations_color(vg_animation_color_t *animations, uint32_t size,
                                            const float color_matrix[]) {
	vg_animation_color_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		anims->start_color = _filter_color(anims->start_color, color_matrix);
		anims->end_color = _filter_color(anims->end_color, color_matrix);
		anims++; // go to next animation
	}
	return (void *)anims;
}

static const void * _apply_path_animations_alpha(uint8_t *alpha, jlong elapsed_time,
                                                 const vg_animation_alpha_t *animations, uint32_t size,
                                                 size_t memory_offset) {
	const vg_animation_alpha_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)alpha, elapsed_time,
		                 &_apply_path_animation_alpha, memory_offset);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static const void * _derive_path_animations_alpha(vg_animation_alpha_t *animations, uint32_t size,
                                                  const float color_matrix[]) {
	vg_animation_alpha_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		anims->start_alpha = _filter_alpha(anims->start_alpha, color_matrix);
		anims->end_alpha = _filter_alpha(anims->end_alpha, color_matrix);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static const void * _apply_path_animations_path_data(vg_lite_path_t *path, jlong elapsed_time,
                                                     const vg_animation_path_data_t *animations, uint32_t size,
                                                     size_t memory_offset) {
	const vg_animation_path_data_t *anims = animations;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)path, elapsed_time,
		                 &_apply_path_animation_path_data, memory_offset);
		anims++; // go to next animation
	}
	return (const void *)anims;
}

static uint32_t _get_path_animations_path_max_length(const vg_lite_path_t *path, jlong elapsed_time,
                                                     const vg_animation_path_data_t *animations, uint32_t size,
                                                     size_t memory_offset) {
	const vg_animation_path_data_t *anims = animations;
	uint32_t length = path->path_length;
	for (uint32_t i = 0; i < size; i++) {
		// cppcheck-suppress [misra-c2012-11.3] animations can be casted as vg_animation_header_t
		_apply_animation((const vg_animation_header_t *)anims, (void *)&length, elapsed_time,
		                 &_apply_get_path_animation_path_length, memory_offset);
		anims++; // go to next animation
	}
	return length;
}

static inline void _store_matrix(vg_transformation_matrix *dest, const vg_transformation_matrix *src) {
	(void)memcpy((void *)dest, (void *)src, sizeof(vg_transformation_matrix));
}

/*
 * @brief path is in ROM (RAW): have to copy the path in a local structure and update the address.
 */
static void _prepare_render_path(const vg_path_desc_t *path) {
	vg_lite_init_path(
		&render_path,
		path->format,
		VG_LITE_HIGH,
		path->length,
		(void *)get_path_data_addr(path),
		path->bounding_box[0],
		path->bounding_box[1],
		path->bounding_box[2],
		path->bounding_box[3]
		);
	render_path.path_type = VG_LITE_DRAW_FILL_PATH;
}

static inline jint _draw_render_path(const vg_path_desc_t *desc, draw_image_element drawer,
                                     const vg_transformation_matrix *matrix, const vg_block_path_t *op,
                                     uint32_t color) {
	_prepare_render_path(desc);
	return (*drawer)(&render_path, VG_VGLITE_HELPER_get_fill_rule(op->fill_rule), matrix, VG_LITE_BLEND_SRC_OVER,
	                 color, NULL, false);
}

static inline jint _draw_render_gradient(const vg_path_desc_t *desc, draw_image_element drawer,
                                         const vg_transformation_matrix *matrix, uint8_t fill_rule,
                                         bool is_new_gradient) {
	_prepare_render_path(desc);
	return (*drawer)(&render_path, VG_VGLITE_HELPER_get_fill_rule(fill_rule), matrix, VG_LITE_BLEND_SRC_OVER, 0,
	                 &render_gradient, is_new_gradient);
}

static inline jint _draw_animated_path(const vg_path_desc_t *desc, draw_image_element drawer,
                                       vg_transformation_matrix *matrix, const vg_block_path_t *op, uint32_t color) {
	return (*drawer)(&render_path, VG_VGLITE_HELPER_get_fill_rule(op->fill_rule), matrix, VG_LITE_BLEND_SRC_OVER,
	                 color, NULL, false);
}

static inline jint _draw_animated_gradient(const vg_path_desc_t *desc, draw_image_element drawer,
                                           vg_transformation_matrix *matrix, uint8_t fill_rule, bool is_new_gradient) {
	return (*drawer)(&render_path, VG_VGLITE_HELPER_get_fill_rule(fill_rule), matrix, VG_LITE_BLEND_SRC_OVER, 0,
	                 &render_gradient, is_new_gradient);
}

static bool _prepare_render_gradient(const vg_drawing_t *drawing_data, const vg_block_gradient_t *op, uint8_t alpha,
                                     const float color_matrix[], const vg_transformation_matrix *transformation) {
	// copy op's gradient in a local gradient to apply the opacity
	vg_gradient local_gradient;
	const vg_gradient *op_gradient = get_gradient_addr(op->gradient, drawing_data->memory_offset);
	(void)memcpy(&local_gradient, op_gradient, GRADIENT_COPY_SIZE);
	_prepare_gradient_colors(&local_gradient, alpha, color_matrix);

	vg_gradient *p_render_gradient = &render_gradient;
	bool is_new_gradient = (0 != memcmp(p_render_gradient, &local_gradient, GRADIENT_CMP_SIZE));

	if (is_new_gradient) {
		// copy the new gradient data in shared gradient
		(void)memcpy(p_render_gradient, &local_gradient, GRADIENT_COPY_SIZE);
	}

	// update the gradient's matrix
	LLVG_MATRIX_IMPL_multiply(
		MAP_NATIVE_MATRIX(&(p_render_gradient->matrix)),
		MAP_NATIVE_MATRIX(transformation),
		MAP_NATIVE_MATRIX(&op_gradient->matrix)
		);

	return is_new_gradient;
}

static uint8_t * _animate_render_path(const vg_path_desc_t *animated_path, jlong elapsed_time,
                                      const vg_animation_path_data_t *first_animation, uint32_t nb_animations,
                                      size_t memory_offset, jint *error) {
	uint8_t *command_buffer;

	*error = LLVG_SUCCESS;

	_prepare_render_path(animated_path);

	if ((uint8_t)0 < nb_animations) {
		// allocate a command buffer where "from" and "to" will be merged
		uint32_t size = _get_path_animations_path_max_length(&render_path, elapsed_time, first_animation, nb_animations,
		                                                     memory_offset);
		command_buffer = _alloc_data(size);

		if (NULL != command_buffer) {
			void *original_commands = render_path.path;
			int32_t original_commands_length = render_path.path_length;

			// update the destination commands buffer address
			render_path.path = (void *)command_buffer;
			render_path.path_length = 0;

			// update path commands, the path length and the bounding box
			(void)_apply_path_animations_path_data(&render_path, elapsed_time, first_animation, nb_animations,
			                                       memory_offset);

			if (0 == render_path.path_length) {
				// no animation has been applied: restore the original path
				render_path.path = original_commands;
				render_path.path_length = original_commands_length;

				// free the useless command buffer
				_free_data(command_buffer);
				command_buffer = NULL;
			}
		} else {
			*error = LLVG_OUT_OF_MEMORY;
		}
		// else not enough memory in heap: use original path
	} else {
		// nothing to do: use path asis
		command_buffer = NULL;
	}

	return command_buffer;
}

static inline void _animation_path_end(const uint8_t *command_buffer) {
	if (NULL != command_buffer) {
		_free_data(command_buffer);
	}
}

static inline vg_block_t * _go_to_next_block(vg_block_t *block, size_t size) {
	// cppcheck-suppress [misra-c2012-11.3,misra-c2012-18.4] cast the block in a u8 address and points to the next block
	return (vg_block_t *)(((uint8_t *)block) + size);
}

static inline const vg_block_t * _go_to_next_block_const(const vg_block_t *block, size_t size) {
	// cppcheck-suppress [misra-c2012-11.3,misra-c2012-18.4] cast the block in a u8 address and points to the next block
	return (const vg_block_t *)(((const uint8_t *)block) + size);
}

static jint _manage_block_gradient(const vg_drawing_t *drawing_data, draw_image_element target, const vg_block_t *block,
                                   const vg_transformation_matrix *matrix, uint32_t alpha, const float color_matrix[],
                                   const vg_transformation_matrix *gradient_deformation) {
	// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_gradient_t for sure
	const vg_block_gradient_t *op = (const vg_block_gradient_t *)block;

	// prepare the path
	const vg_path_desc_t *desc = get_path_addr(op->path.desc, drawing_data->memory_offset);

	// prepare the gradient
	bool is_new_gradient = _prepare_render_gradient(drawing_data, op, alpha, color_matrix, gradient_deformation);

	return _draw_render_gradient(desc, target, matrix, op->path.fill_rule, is_new_gradient);
}

static void _get_image_parameters(const vector_image_t *image, vg_drawing_t *drawing_data) {
	if (!_is_image_address_relative(image)) {
		// the addresses are absolute: no memory offset
		drawing_data->memory_offset = 0;
	} else {
		// the addresses are relative: the memory offset is tehe image address
		// cppcheck-suppress [misra-c2012-11.4] save the image address as the memory offset to apply on each address
		drawing_data->memory_offset = (uintptr_t)image;
	}

	uint8_t *first_block_addr = (uint8_t *)image->first_block;

	// cppcheck-suppress [misra-c2012-18.4] adds the absolute memory offset
	first_block_addr += drawing_data->memory_offset;

	// cppcheck-suppress [misra-c2012-11.3] address points to the first block for sure
	drawing_data->first_block = (vg_block_t *)first_block_addr;

	drawing_data->drawing_running = false;
}

// -----------------------------------------------------------------------------
// Buffered vector images
// -----------------------------------------------------------------------------

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

/*
 * @brief Tells if the image is in ROM.
 */
static inline bool _bvi_is_image_in_rom(const vector_image_t *image) {
	return 0u == image->flags.ram;
}

static inline void _bvi_tag_image_as_absolute(vector_image_t *image) {
	image->flags.absolute = 1u;
}

static vg_path_desc_t * _bvi_store_native_path(const vg_lite_path_t *path) {
	// cppcheck-suppress [misra-c2012-11.3] the allocated data is a vg_path_desc_t for sure
	vg_path_desc_t *data = (vg_path_desc_t *)_alloc_data(sizeof(vg_path_desc_t) + path->path_length);

	if (NULL != data) {
		uint8_t *cmd_addr = (uint8_t *)data;

		// cppcheck-suppress [misra-c2012-18.4] points after the path description
		cmd_addr += sizeof(vg_path_desc_t);

		(void)memcpy((void *)&(data->bounding_box), (void *)&(path->bounding_box), (size_t)4 * sizeof(float));
		(void)memcpy((void *)cmd_addr, path->path, path->path_length);
		data->format = path->format;
		data->length = path->path_length;
	}

	return data;
}

static inline vg_gradient * _bvi_store_native_gradient(const vg_gradient *grad) {
	return (vg_gradient *)_store_data(grad, GRADIENT_COPY_SIZE);
}

static inline vg_block_t * _bvi_free_operation_color(vg_block_bvi_color_t *op) {
	vg_block_t *ret = op->next;
	_free_data(op->header.path.desc);
	_free_data(op);
	return ret;
}

static inline vg_block_t * _bvi_free_operation_gradient(vg_block_bvi_gradient_t *op) {
	vg_block_t *ret = op->next;
	_free_data(op->header.path.desc);
	if (!op->shared_gradient) {
		_free_data(op->header.gradient);
	}
	_free_data(op);
	return ret;
}

static inline vg_block_t * _bvi_free_operation_image(vg_block_bvi_image_t *op) {
	vg_block_t *ret = op->next;
	_free_data(op);
	return ret;
}

static vg_block_t * _bvi_free_block(vg_block_t *block) {
	vg_block_t *ret;
	switch (block->kind) {
	case VG_BLOCK_BVI_COLOR:
		// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_color_t for sure
		ret = _bvi_free_operation_color((vg_block_bvi_color_t *)block);
		break;

	case VG_BLOCK_BVI_GRADIENT:
		// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_gradient_t for sure
		ret = _bvi_free_operation_gradient((vg_block_bvi_gradient_t *)block);
		break;

	case VG_BLOCK_BVI_IMAGE:
		// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_image_t for sure
		ret = _bvi_free_operation_image((vg_block_bvi_image_t *)block);
		break;

	case VG_BLOCK_GROUP_TRANSFORM:
	case VG_BLOCK_GROUP_END:
	case VG_BLOCK_LAST:
	case VG_BLOCK_GROUP_ANIMATE:
	case VG_BLOCK_RAW_GRADIENT:
	case VG_BLOCK_PATH_COLOR_ANIMATE:
	case VG_BLOCK_PATH_GRADIENT_ANIMATE:
	default:
		// a BVI cannot free this kind of block: never here
		ret = block;
		break;
	}
	return ret;
}

static inline jint _bvi_link_operations(vector_buffered_image_t *raw, vg_block_t *op, vg_block_t **op_next_block,
                                        bool oom) {
	jint ret;

	if (!oom) {
		if (NULL == raw->latest_data) {
			// very first block
			raw->header.first_block = op;
		} else {
			*(raw->latest_data) = op;
		}
		raw->latest_data = op_next_block;
		*(op_next_block) = &(raw->last_block);

		ret = LLVG_SUCCESS;
	} else {
		(void)_bvi_free_block(op);
		ret = LLVG_OUT_OF_MEMORY;
	}

	return ret;
}

static void _bvi_save_current_native_scissor(int32_t *dest) {
	int32_t *scissor;
	if (0u != vg_lite_get_scissor(&scissor)) {
		(void)memcpy((void *)dest, (void *)scissor, 4u * sizeof(int32_t));
	} else {
		// no clip: -1 is not a valid X clip value
		dest[0] = -1;
	}
}

static inline vector_buffered_image_t * _bvi_get_bvi_from_target(void *target) {
	// cppcheck-suppress [misra-c2012-11.5] cast is possible (the target stored in drawer is a vector_buffered_image_t*
	// for sure)
	return (vector_buffered_image_t *)target;
}

static jint _bvi_add_draw_raw_image(
	vector_buffered_image_t *dest,
	const vector_image_t *source,
	const jfloat *matrix,
	jint alpha,
	jlong elapsed,
	const float color_matrix[]) {
	// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_image_t for sure
	vg_block_bvi_image_t *op = (vg_block_bvi_image_t *)_alloc_data(sizeof(vg_block_bvi_image_t));
	jint ret = LLVG_OUT_OF_MEMORY;

	if (NULL != op) {
		op->block.kind = VG_BLOCK_BVI_IMAGE;
		op->alpha = (jchar)alpha;
		op->image = source;
		(void)memcpy((void *)&(op->matrix), (void *)matrix, (size_t)LLVG_MATRIX_SIZE * sizeof(jfloat));
		_bvi_save_current_native_scissor(op->scissor);
		if (NULL != color_matrix) {
			// required size is checked by Java library
			(void)memcpy((void *)&(op->color_matrix), color_matrix, COLOR_MATRIX_SIZE * sizeof(jfloat));
			op->flags = 1u;
		} else {
			op->flags = 0u;
		}
		op->elapsed_msb = (jint)elapsed;
		op->elapsed_lsb = (jint)(elapsed >> 32);

		// cppcheck-suppress [misra-c2012-11.3] operation is a vg_block_t
		ret = _bvi_link_operations(dest, (vg_block_t *)op, &(op->next), false);
	}

	return ret;
}

/*
 * @brief Function type is "draw_image_element" (called by _draw_raw_image()).
 */
static jint _bvi_add_image_element(const vg_lite_path_t *path, vg_lite_fill_t fill_rule,
                                   const vg_transformation_matrix *matrix, vg_lite_blend_t blend, vg_lite_color_t color,
                                   vg_gradient *shared_gradient, bool is_new_gradient) {
	(void)is_new_gradient;
	jint ret;
	if (NULL == shared_gradient) {
		ret = VG_BVI_VGLITE_add_draw_path(bvi_target, path, fill_rule, matrix, blend, color);
	} else {
		ret = VG_BVI_VGLITE_add_draw_gradient(bvi_target, path, fill_rule, matrix, shared_gradient, blend);
	}
	return ret;
}

/*
 * @brief Transform a 2D point by a given matrix.
 * XXX use LLVG_MATRIX_IMPL_transformPoint()
 *
 * Copy from vg_lite.c
 */
static void _bvi_transform_point(vg_lite_point_t *result, vg_lite_float_t x, vg_lite_float_t y,
                                 const vg_transformation_matrix *matrix) {
	/* Transform w. */
	vg_lite_float_t pt_w = (x * matrix->m[2][0]) + (y * matrix->m[2][1]) + matrix->m[2][2];
	if (pt_w <= 0.0f) {
		result->x = 0;
		result->y = 0;
	} else {
		/* Transform x and y. */
		vg_lite_float_t pt_x = (x * matrix->m[0][0]) + (y * matrix->m[0][1]) + matrix->m[0][2];
		vg_lite_float_t pt_y = (x * matrix->m[1][0]) + (y * matrix->m[1][1]) + matrix->m[1][2];

		/* Compute projected x and y. */
		result->x = (int)(pt_x / pt_w);
		result->y = (int)(pt_y / pt_w);
	}
}

/*
 * @brief Applies the scissor. The scissor is adjusted with the given matrix (image's matrix)
 * and the current scissor (master scissor).
 *
 * @param[in] scissor: pointer to the scissor to apply or NULL.
 * @param[in] matrix: pointer to the matrix to apply to the scissor.
 * @param[in/out] current_scissor: pointer to the current scissor, set to NULL when the scissor is modified and has to
 * be restored after the drawing.
 *
 * @return false when no drawing should be performed (empty scissor), true otherise.
 */
static bool _bvi_apply_scissor(const int32_t *scissor, const vg_transformation_matrix *matrix,
                               const int32_t **current_scissor) {
	bool draw = true;

	if (scissor[0] >= 0) {
		vg_lite_point_t top_left;
		vg_lite_point_t bottom_right;

		// get the top-left and bottom-right scissor points adjusted with the given matrix.
		_bvi_transform_point(&top_left, (vg_lite_float_t)scissor[0], (vg_lite_float_t)scissor[1], matrix);
		_bvi_transform_point(&bottom_right, (vg_lite_float_t)(scissor[0] + scissor[2] - 1),
		                     (vg_lite_float_t)(scissor[1] + scissor[3] - 1), matrix);

		if ((*current_scissor) != NULL) {
			int32_t cx1 = (*current_scissor)[0];
			int32_t cy1 = (*current_scissor)[1];
			int32_t cx2 = cx1 + (*current_scissor)[2] - 1;
			int32_t cy2 = cy1 + (*current_scissor)[3] - 1;

			if (top_left.x < cx1) {
				top_left.x = cx1;
			}
			if (top_left.y < cy1) {
				top_left.y = cy1;
			}
			if (bottom_right.x >= cx2) {
				bottom_right.x = cx2;
			}
			if (bottom_right.y >= cy2) {
				bottom_right.y = cy2;
			}
		}

		if ((top_left.x <= bottom_right.x) && (top_left.y <= bottom_right.y)) {
			vg_lite_enable_scissor();
			vg_lite_set_scissor(top_left.x, top_left.y, bottom_right.x - top_left.x + 1,
			                    bottom_right.y - top_left.y + 1);
			*current_scissor = NULL; // have to restore the clip
		} else {
			// empty clip: nothing to restore and nothing to draw
			draw = false;
		}
	}
	// else: no local clip: nothing to restore

	return draw;
}

static const int32_t * _bvi_restore_scissor(const int32_t *original_scissor, const int32_t *current_scissor) {
	if (original_scissor != current_scissor) {
		// the scissor has been modified: restore the original one
		if (NULL != original_scissor) {
			vg_lite_enable_scissor();
			vg_lite_set_scissor(original_scissor[0], original_scissor[1], original_scissor[2], original_scissor[3]);
		} else {
			vg_lite_disable_scissor();
		}
	}
	return original_scissor;
}

static vg_block_t * _bvi_clear_blocks(vector_buffered_image_t *image) {
	vg_block_t *block = image->header.first_block;
	while (VG_BLOCK_LAST != block->kind) {
		block = _bvi_free_block(block);
	}
	return block;
}

/*
 * @brief Combines two color matrices into a single one.
 * <p>
 * The application of the returned matrix is equivalent to the successive application of the two given matrices.
 * <p>
 * The algorithm used in this method is based on to the computation of the matrix multiplication <code>B x A</code>
 * where a 5th row of values <code>(0, 0, 0, 0, 1)</code> would be appended to the matrix A.
 * <p>
 * - Matrix a can be null, in such case, matrix b is returned,
 * - Matrix b can be null, in such case, matrix a is returned,
 * - Both matrices can be null, in such case, NULL is returned,
 * - if not null, the result is stored in the global matrix "combined_color_matrices" (old content is overwritten)
 * and returned
 *
 * @param a
 *            the operation matrix.
 * @param b
 *            the transformation to apply.
 * @return the combined matrix.
 */
static float * _combine_color_matrices(float *a, float *b) {
	/*
	 * @brief Color matrix used to combine user matrix and RAW image matrix inside the BVI.
	 *
	 * UI_GLOBAL
	 */
	static const float CONSTANT_ROW[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	static float combined_color_matrices[COLOR_MATRIX_SIZE];

	float *dest;

	if (NULL != a) {
		// operation holds a color matrix (image matrix)
		if (NULL != b) {
			// draw with another matrix

			// -> need to combine user matrix and operation matrix
			// use the shared array
			dest = combined_color_matrices;
			(void)memset(dest, 0, COLOR_MATRIX_SIZE * sizeof(float));

			for (size_t y = 0u; y < COLOR_MATRIX_HEIGHT; y++) {
				size_t y5 = y * COLOR_MATRIX_WIDTH;
				for (size_t x = 0u; x < COLOR_MATRIX_WIDTH; x++) {
					// here (x,y) is the cell to compute
					for (size_t i = 0u; i < COLOR_MATRIX_WIDTH; i++) {
						if (i < COLOR_MATRIX_HEIGHT) {
							size_t i5 = i * COLOR_MATRIX_WIDTH;
							dest[y5 + x] += b[y5 + i] * a[i5 + x];
						} else {
							dest[y5 + x] += b[y5 + i] * CONSTANT_ROW[x];
						}
					}
				}
			}
		} else {
			// use the operation matrix
			dest = a;
		}
	} else if (NULL != b) {
		// use the user matrix
		dest = b;
	} else {
		// no color matrix to apply
		dest = NULL;
	}

	return dest;
}

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

static jint _draw_raw_image(vg_drawing_t *drawing_data, draw_image_element target, const jfloat *matrix, uint32_t alpha,
                            jlong elapsedTime, const float color_matrix[]) {
#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE
	/*
	 * @brief Working data to draw a RAW image stored in a BVI.
	 *
	 * UI_GLOBAL
	 */
	static vg_drawing_t raw_in_bvi_drawing_data;
#endif

	const vg_block_t *block = drawing_data->first_block;
	bool animate = elapsedTime >= 0;
	jlong elapsed_time = animate ? elapsedTime : 0; // default time when there is no animation

	// create initial transformation with application's matrix
	drawing_data->first_transformation.parent = NULL;
	_store_matrix(&(drawing_data->first_transformation.matrix), (vg_transformation_matrix *)matrix);
	vg_transformation_t *p_render_transformation = &(drawing_data->first_transformation);

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

	int32_t original_scissor[4];
	int32_t *p_original_scissor;
	const int32_t *p_current_scissor;
	if (0u != vg_lite_get_scissor(&p_original_scissor)) {
		// p_original_scissor points to vglite scissor

		// save vglite scissor in "original scissor"
		original_scissor[0] = p_original_scissor[0];
		original_scissor[1] = p_original_scissor[1];
		original_scissor[2] = p_original_scissor[2];
		original_scissor[3] = p_original_scissor[3];

		// now p_original_scissor points to the local array
		p_original_scissor = original_scissor;
		p_current_scissor = original_scissor;
	} else {
		// no current scissor
		p_original_scissor = NULL;
		p_current_scissor = NULL;
	}

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

	jint ret = LLVG_SUCCESS;
	bool done = false;

	while (!done) {
		switch (block->kind) {
		case VG_BLOCK_LAST:
		{
			done = true;
		}
		break;

		case VG_BLOCK_GROUP_TRANSFORM:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_group_transform_t for sure
			const vg_block_group_transform_t *op = (const vg_block_group_transform_t *)block;

			// copy current transformation
			vg_transformation_t *parent_transformation = p_render_transformation;
			p_render_transformation = _store_vg_transformation(parent_transformation);

			if (NULL != p_render_transformation) {
				p_render_transformation->parent = parent_transformation;

				// concatenate first transformation with new operation transformation
				LLVG_MATRIX_IMPL_concatenate(MAP_NATIVE_MATRIX(&(p_render_transformation->matrix)),
				                             MAP_NATIVE_MATRIX(&(op->matrix)));
			} else {
				ret = LLVG_OUT_OF_MEMORY;
				// Cannot apply a new transformation: keep the current one.
			}

			block = _go_to_next_block_const(block, sizeof(vg_block_group_transform_t));
		}
		break;

		case VG_BLOCK_GROUP_END:
		{
			if (NULL != p_render_transformation->parent) {
				// back to parent transformation
				vg_transformation_t *transformation_to_remove = p_render_transformation;
				p_render_transformation = transformation_to_remove->parent;
				_free_vg_transformation(transformation_to_remove);
			}

			block = _go_to_next_block_const(block, sizeof(vg_block_group_end_t));
		}
		break;

		case VG_BLOCK_GROUP_ANIMATE:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_group_animation_t for sure
			const vg_block_group_animation_t *op = (const vg_block_group_animation_t *)block;

			// copy current transformation
			vg_transformation_t *parent_transformation = p_render_transformation;
			p_render_transformation = _store_vg_transformation(parent_transformation);

			if (NULL != p_render_transformation) {
				p_render_transformation->parent = parent_transformation;

				float *mapped_matrix = MAP_NATIVE_MATRIX(&(p_render_transformation->matrix));
				// cppcheck-suppress [misra-c2012-18.4] points after the operation
				const void *animations_offset = ((const uint8_t *)op) + sizeof(vg_block_group_animation_t);

				if (animate) {
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_translate_t for
					// sure
					animations_offset = _apply_group_animations_translate(mapped_matrix, elapsed_time,
					                                                      (const vg_animation_translate_t *)
					                                                      animations_offset, op->nb_translates,
					                                                      drawing_data->memory_offset);
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_translate_xy_t
					// for sure
					animations_offset = _apply_group_animations_translate_xy(mapped_matrix, elapsed_time,
					                                                         (const vg_animation_translate_xy_t *)
					                                                         animations_offset, op->nb_translates_xy,
					                                                         drawing_data->memory_offset);
				}

				LLVG_MATRIX_IMPL_translate(mapped_matrix, op->translate_x, op->translate_y);

				if (animate) {
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_rotate_t for sure
					animations_offset = _apply_group_animations_rotate(mapped_matrix, elapsed_time,
					                                                   (const vg_animation_rotate_t *)animations_offset,
					                                                   op->nb_rotates, drawing_data->memory_offset);
				}

				LLVG_MATRIX_IMPL_translate(mapped_matrix, op->pivot_x, op->pivot_y);
				LLVG_MATRIX_IMPL_rotate(mapped_matrix, op->rotation);
				LLVG_MATRIX_IMPL_translate(mapped_matrix, -op->pivot_x, -op->pivot_y);

				if (animate) {
					// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_scale_t for sure
					/*animations_offset =*/ (void)_apply_group_animations_scale(mapped_matrix, elapsed_time,
					                                                            (const vg_animation_scale_t *)
					                                                            animations_offset, op->nb_scales,
					                                                            drawing_data->memory_offset);
				}

				LLVG_MATRIX_IMPL_translate(mapped_matrix, op->pivot_x, op->pivot_y);
				LLVG_MATRIX_IMPL_scale(mapped_matrix, op->scale_x, op->scale_y);
				LLVG_MATRIX_IMPL_translate(mapped_matrix, -op->pivot_x, -op->pivot_y);
			} else {
				ret = LLVG_OUT_OF_MEMORY;
				// Cannot apply a new transformation: keep the current one.
			}

			block = _go_to_next_block_const(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR_ANIMATE:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_animate_color_t for sure
			const vg_block_animate_color_t *op = (const vg_block_animate_color_t *)block;
			// cppcheck-suppress [misra-c2012-18.4] points after the operation
			const void *animations_offset = ((const uint8_t *)op) + sizeof(vg_block_animate_color_t);

			// update color & alpha
			uint32_t color = op->header.color;
			uint8_t color_alpha = COLOR_GET_CHANNEL(color, ARGB8888, ALPHA);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_color_t for sure
			animations_offset = _apply_path_animations_color(&color, elapsed_time,
			                                                 (const vg_animation_color_t *)animations_offset,
			                                                 op->nb_colors, drawing_data->memory_offset);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_alpha_t for sure
			animations_offset = _apply_path_animations_alpha(&color_alpha, elapsed_time,
			                                                 (const vg_animation_alpha_t *)animations_offset,
			                                                 op->nb_alphas, drawing_data->memory_offset);
			color |= (color_alpha << COLOR_ARGB8888_ALPHA_OFFSET);
			color = _prepare_render_color(color, alpha, color_matrix);

			// prepare & animate the path
			jint error;
			const vg_path_desc_t *animated_path = get_path_addr(op->header.path.desc, drawing_data->memory_offset);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_path_data_t for sure
			const uint8_t *command_buffer = _animate_render_path(animated_path, elapsed_time,
			                                                     (const vg_animation_path_data_t *)animations_offset,
			                                                     op->nb_path_datas, drawing_data->memory_offset,
			                                                     &error);
			if (LLVG_SUCCESS != error) {
				ret = error;
			} else {
				error = _draw_animated_path(animated_path, target, &(p_render_transformation->matrix),
				                            &(op->header.path), color);
				if (LLVG_SUCCESS != error) {
					ret = error;
				}
				drawing_data->drawing_running = true;

				_animation_path_end(command_buffer);
			}

			block = _go_to_next_block_const(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_GRADIENT_ANIMATE:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_animate_gradient_t for sure
			const vg_block_animate_gradient_t *op = (const vg_block_animate_gradient_t *)block;
			// cppcheck-suppress [misra-c2012-18.4] points after the operation
			const void *animations_offset = ((const uint8_t *)op) + sizeof(vg_block_animate_gradient_t);

			uint32_t alpha_to_apply = alpha;
			if ((uint8_t)0 < op->nb_alphas) {
				// update alpha
				uint8_t alpha_anim = (uint8_t)0xff;
				// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_alpha_t for sure
				animations_offset = _apply_path_animations_alpha(&alpha_anim, elapsed_time,
				                                                 (const vg_animation_alpha_t *)animations_offset,
				                                                 op->nb_alphas, drawing_data->memory_offset);
				alpha_to_apply *= alpha_anim;
				alpha_to_apply /= 255;
			}

			// prepare & animate the path
			jint error;
			const vg_path_desc_t *animated_path = get_path_addr(op->header.path.desc, drawing_data->memory_offset);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_path_data_t for sure
			const uint8_t *command_buffer = _animate_render_path(animated_path, elapsed_time,
			                                                     (const vg_animation_path_data_t *)animations_offset,
			                                                     op->nb_path_datas, drawing_data->memory_offset,
			                                                     &error);
			if (LLVG_SUCCESS != error) {
				ret = error;
			} else {
				// prepare the gradient
				bool is_new_gradient = _prepare_render_gradient(drawing_data, &(op->header), alpha_to_apply,
				                                                color_matrix, &(p_render_transformation->matrix));

				error = _draw_animated_gradient(animated_path, target, &(p_render_transformation->matrix),
				                                op->header.path.fill_rule, is_new_gradient);
				if (LLVG_SUCCESS != error) {
					ret = error;
				}
				drawing_data->drawing_running = true;

				_animation_path_end(command_buffer);
			}

			block = _go_to_next_block_const(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_color_t for sure
			const vg_block_color_t *op = (const vg_block_color_t *)block;
			const vg_path_desc_t *desc = get_path_addr(op->path.desc, drawing_data->memory_offset);
			uint32_t color = _prepare_render_color(op->color, alpha, color_matrix);
			jint error = _draw_render_path(desc, target, &(p_render_transformation->matrix), &(op->path), color);
			if (LLVG_SUCCESS != error) {
				ret = error;
			}
			drawing_data->drawing_running = true;
			block = _go_to_next_block_const(block, sizeof(vg_block_color_t));
		}
		break;

		case VG_BLOCK_RAW_GRADIENT:
		{
			// apply same deformation on path and gradient
			jint error = _manage_block_gradient(drawing_data, target, block, &(p_render_transformation->matrix), alpha,
			                                    color_matrix, &(p_render_transformation->matrix));
			if (LLVG_SUCCESS != error) {
				ret = error;
			}
			drawing_data->drawing_running = true;
			block = _go_to_next_block_const(block, sizeof(vg_block_gradient_t));
		}
		break;

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

		case VG_BLOCK_BVI_COLOR:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_color_t for sure
			const vg_block_bvi_color_t *op = (const vg_block_bvi_color_t *)block;

			if (_bvi_apply_scissor(op->scissor, &(p_render_transformation->matrix), &p_current_scissor)) {
				// concatenate first transformation with new operation transformation
				vg_transformation_matrix temp;
				LLVG_MATRIX_IMPL_multiply(MAP_NATIVE_MATRIX(&temp),
				                          MAP_NATIVE_MATRIX(&(p_render_transformation->matrix)),
				                          MAP_NATIVE_MATRIX(&(op->path_deformation)));

				const vg_path_desc_t *desc = get_path_addr(op->header.path.desc, drawing_data->memory_offset);
				uint32_t color = _prepare_render_color(op->header.color, alpha, color_matrix);
				jint error = _draw_render_path(desc, target, &temp, &(op->header.path), color);
				if (LLVG_SUCCESS != error) {
					ret = error;
				}
				drawing_data->drawing_running = true;

				// the clip may have been modified: restore the original one
				p_current_scissor = _bvi_restore_scissor(p_original_scissor, p_current_scissor);
			}
			block = op->next;
		}
		break;

		case VG_BLOCK_BVI_GRADIENT:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_gradient_t for sure
			const vg_block_bvi_gradient_t *op = (const vg_block_bvi_gradient_t *)block;

			if (_bvi_apply_scissor(op->scissor, &(p_render_transformation->matrix), &p_current_scissor)) {
				// concatenate first transformation with new operation transformation
				vg_transformation_matrix temp;
				LLVG_MATRIX_IMPL_multiply(MAP_NATIVE_MATRIX(&temp),
				                          MAP_NATIVE_MATRIX(&(p_render_transformation->matrix)),
				                          MAP_NATIVE_MATRIX(&(op->path_deformation)));

				// do not apply same deformation on path and gradient (gradient already "has" the path's deformation)
				jint error = _manage_block_gradient(drawing_data, target, block, &temp, alpha, color_matrix,
				                                    &(p_render_transformation->matrix));
				if (LLVG_SUCCESS != error) {
					ret = error;
				}
				drawing_data->drawing_running = true;

				// the clip may have been modified: restore the original one
				p_current_scissor = _bvi_restore_scissor(p_original_scissor, p_current_scissor);
			}
			block = op->next;
		}
		break;

		case VG_BLOCK_BVI_IMAGE:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_image_t for sure
			const vg_block_bvi_image_t *op = (const vg_block_bvi_image_t *)block;

			if (_bvi_apply_scissor(op->scissor, &(p_render_transformation->matrix), &p_current_scissor)) {
				_get_image_parameters(op->image, &raw_in_bvi_drawing_data);

				// retrieve elapsed time
				jlong elapsedTime_msb = op->elapsed_msb;
				jlong elapsedTime_lsb = op->elapsed_lsb;
				elapsedTime_msb <<= 32;
				elapsedTime_lsb &= 0xfffffffful;
				jlong raw_elapsed_time = (elapsedTime_msb | elapsedTime_lsb);

				// merge operation's color matrix with global color matrix
				float *image_color_matrix = (1u == op->flags) ? (float *)op->color_matrix : NULL;
				// cppcheck-suppress [misra-c2012-11.8] allow cast to float*
				image_color_matrix = _combine_color_matrices(image_color_matrix, (float *)color_matrix);

				// merge operation's alpha with global alpha
				uint32_t image_alpha = op->alpha;
				image_alpha *= alpha;
				image_alpha /= 255;

				// concatenate first transformation with new operation transformation
				vg_transformation_matrix temp;
				LLVG_MATRIX_IMPL_multiply(MAP_NATIVE_MATRIX(&temp),
				                          MAP_NATIVE_MATRIX(&(p_render_transformation->matrix)),
				                          MAP_NATIVE_MATRIX(&(op->matrix)));

				// cppcheck-suppress [misra-c2012-17.2] only one level of recursion
				jint error = _draw_raw_image(&raw_in_bvi_drawing_data, target, MAP_NATIVE_MATRIX(&temp), image_alpha,
				                             raw_elapsed_time, image_color_matrix);
				if (LLVG_SUCCESS != error) {
					ret = error;
				}
				drawing_data->drawing_running |= raw_in_bvi_drawing_data.drawing_running;
			}

			block = op->next;
		}
		break;

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

		default:
			MEJ_LOG_ERROR_MICROVG("unknown operation: %d\n", block->kind);
			ret = LLVG_DATA_INVALID;
			done = true;
			break;
		}
	}

	return ret;
}

static void _derive_raw_image(vg_drawing_t *drawing_data, const float color_matrix[]) {
	bool done = false;
	vg_block_t *block = drawing_data->first_block;

	while (!done) {
		switch (block->kind) {
		case VG_BLOCK_LAST:
		{
			done = true;
		}
		break;

		case VG_BLOCK_GROUP_TRANSFORM:
		{
			// nothing to do
			block = _go_to_next_block(block, sizeof(vg_block_group_transform_t));
		}
		break;

		case VG_BLOCK_GROUP_END:
		{
			// nothing to do
			block = _go_to_next_block(block, sizeof(vg_block_group_end_t));
		}
		break;

		case VG_BLOCK_GROUP_ANIMATE:
		{
			// nothing to do
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_group_animation_t for sure
			const vg_block_group_animation_t *op = (const vg_block_group_animation_t *)block;
			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR_ANIMATE:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_animate_color_t for sure
			vg_block_animate_color_t *op = (vg_block_animate_color_t *)block;
			// cppcheck-suppress [misra-c2012-18.4] points after the operation
			void *animations_offset = ((uint8_t *)op) + sizeof(vg_block_animate_color_t);
			op->header.color = _filter_color(op->header.color, color_matrix);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_color_t for sure
			animations_offset = _derive_path_animations_color((vg_animation_color_t *)animations_offset, op->nb_colors,
			                                                  color_matrix);
			// cppcheck-suppress [misra-c2012-11.5] animations_offset points on a vg_animation_alpha_t for sure
			/*animations_offset =*/ (void)_derive_path_animations_alpha((vg_animation_alpha_t *)animations_offset,
			                                                            op->nb_alphas, color_matrix);
			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_GRADIENT_ANIMATE:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_animate_gradient_t for sure
			const vg_block_animate_gradient_t *op = (const vg_block_animate_gradient_t *)block;
			vg_gradient *gradient = (vg_gradient *)offset_address((uintptr_t)op->header.gradient,
			                                                      drawing_data->memory_offset);
			_filter_gradient(gradient, color_matrix);
			block = _go_to_next_block(block, op->block_size);
		}
		break;

		case VG_BLOCK_PATH_COLOR:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_color_t for sure
			vg_block_color_t *op = (vg_block_color_t *)block;
			op->color = _filter_color(op->color, color_matrix);
			block = _go_to_next_block(block, sizeof(vg_block_color_t));
		}
		break;

		case VG_BLOCK_RAW_GRADIENT:
		{
			// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_gradient_t for sure
			const vg_block_gradient_t *op = (const vg_block_gradient_t *)block;
			vg_gradient *gradient = (vg_gradient *)offset_address((uintptr_t)op->gradient, drawing_data->memory_offset);
			_filter_gradient(gradient, color_matrix);
			block = _go_to_next_block(block, sizeof(vg_block_gradient_t));
		}
		break;

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

		case VG_BLOCK_BVI_COLOR:
		case VG_BLOCK_BVI_GRADIENT:
		case VG_BLOCK_BVI_IMAGE:
			// must not occur: can only derive from a RAW image (see BufferedVectorImage.filterImage())

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

		default:
			MEJ_LOG_ERROR_MICROVG("unknown operation: %d\n", block->kind);
			done = true;
			break;
		}
	}
}

/*
 * @brief Tells if the byte array denotes a RAW image. A RAW image is identified
 * by its signature.
 */
static inline bool _is_raw_image(const void *image) {
	static const uint8_t signature[] = { 'M', 'E', 'J', '_', 'M', 'V', 'G', LLVG_RAW_VERSION };
	return 0 == memcmp((const uint8_t *)image, signature, sizeof(signature));
}

static inline void _store_image_metadata(const vector_image_t *image, jint extra_flags, jint *metadata) {
	metadata[RAW_OFFSET_F32_WIDTH] = JFLOAT_TO_UINT32_t(image->width);
	metadata[RAW_OFFSET_F32_HEIGHT] = JFLOAT_TO_UINT32_t(image->height);
	metadata[RAW_OFFSET_U32_DURATION] = image->flags.duration;
	metadata[RAW_OFFSET_U32_FLAGS] = (image->flags.overlapping ? RAW_FLAG_OVERLAP_PATH : 0) | extra_flags;
}

#if VG_FEATURE_RAW_EXTERNAL

static inline void _store_image_resource(vector_image_t *image, int32_t resource_size, MICROVG_Image *resource) {
	resource->data = (void *)image;
	resource->size = resource_size;
}

/*
 * @brief Loads a RAW vector image from external memory
 *
 * The resource is opened from external memory if it exists. Its address, size and metadata are stored in the output
 * parameters.
 * If the resource exists in a byte-addressable area, the output address will be that of this memory area.
 * Otherwise, a RAM area will be allocated on the MicroUI image heap, the resource will be copied into that area, and
 * the output address will be that of the newly allocated memory area.
 * In the latter case, metadata will contain a RAW_FLAG_FREE_MEMORY_ON_CLOSE. The caller is responsible for properly
 * deallocating the memory.
 *
 * @param[in] image_name: name of the image to be opened
 * @param[in] allocation_allowed: true to allow an allocation in the images heap, false when forbidden
 * @param[out] resource: structure to save the address and size of the image
 * @param[out] metadata: integer array to save the width, height, animation duration and flags of the image
 *
 * @return LLVG_SUCCESS on a successful loading, any other value on a failure.
 */
static int _load_external_image(char const *image_name, bool allocation_allowed, MICROVG_Image *resource,
                                jint *metadata) {
	int result = LLVG_SUCCESS;

	// Ignore the leading '/' in the path.
	char const *image_external_path = image_name;
	image_external_path++;

	// Open the resource matching the provided path.
	RES_ID resource_id = LLEXT_RES_open(image_external_path);
	if (0 > resource_id) {
		result = LLVG_DATA_INVALID_PATH;
	} else {
		// Retrieve the address and size of the resource.
		int32_t resource_size = LLEXT_RES_available(resource_id);
		int32_t base_address = LLEXT_RES_getBaseAddress(resource_id);

		vector_image_t *image = NULL;

		if (-1 != base_address) {
			// The resource is located in a byte-addressable memory area. This will be the output address.
			// cppcheck-suppress [misra-c2012-11.4] base_address is a vector_image_t for sure
			image = (vector_image_t *)base_address;

			// Put the address, size and metadata in the ouptut parameters.
			_store_image_resource(image, resource_size, resource);

			result = LLVG_SUCCESS;
		} else if (!allocation_allowed) {
			// image is available but required a copy in the images heap
			result = LLVG_DATA_INVALID_PATH;
		} else {
			// The resource is located in a memory area that cannot be byte-addressed.

			// Allocate memory on the MicroUI image heap to store the image. Its address will be the output address.
			image = (vector_image_t *)LLUI_DISPLAY_IMPL_imageHeapAllocate(resource_size);
			if (NULL == image) {
				// Out of memory.
				result = LLVG_OUT_OF_MEMORY;
			} else {
				// Copy the image from the external memory into the allocated area.
				if (LLEXT_RES_OK != LLEXT_RES_read(resource_id, (void *)image, &resource_size)) {
					result = LLVG_DATA_INVALID;
				} else if (!_is_raw_image((uint8_t *)image)) {
					// it is not a RAW image -> free it
					LLUI_DISPLAY_IMPL_imageHeapFree((uint8_t *)image);
					result = LLVG_DATA_INVALID;
				} else {
					/* Put the address, size and metadata in the ouptut parameters.
					 * RAW_FLAG_FREE_MEMORY_ON_CLOSE flags the image as requiring to be deallocated.
					 */
					_store_image_resource(image, resource_size, resource);
					_store_image_metadata(image, RAW_FLAG_FREE_MEMORY_ON_CLOSE, metadata);
					_tag_image_in_ram(image);

					result = LLVG_SUCCESS;
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
// BufferedVectorImage (BVI) Management
// -----------------------------------------------------------------------------

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

void VG_BVI_VGLITE_initialize(void) {
	// prepare the gradient used by all draw_gradient
	(void)memset(&(render_gradient), 0, sizeof(vg_gradient));
	(void)vg_lite_init_grad(&(render_gradient)); // always success
	render_gradient.image.format = VG_LITE_RGBA8888; // fix color format
}

void VG_BVI_VGLITE_adjust_new_image_characteristics(uint32_t width, uint32_t height, uint32_t *data_size,
                                                    uint32_t *data_alignment) {
	(void)width;
	(void)height;
	(void)data_alignment;
	*data_size += sizeof(vector_buffered_image_t);
}

void VG_BVI_VGLITE_initialize_new_image(MICROUI_Image *image) {
	vector_buffered_image_t *bvi = MAP_BVI_ON_IMAGE(image);

	bvi->header.width = (float)image->width;
	bvi->header.height = (float)image->height;
	bvi->header.flags.duration = 0;
	bvi->header.flags.overlapping = 0;
	bvi->header.first_block = &(bvi->last_block);

	bvi->last_block.kind = VG_BLOCK_LAST;
	bvi->latest_data = NULL; // very first block
	bvi->last_stored_gradient = NULL;

	// cppcheck-suppress [misra-c2012-11.3] bvi is a vector_image_t for sure
	vector_image_t *raw = (vector_image_t *)bvi;
	_tag_image_in_ram(raw);
	_bvi_tag_image_as_absolute(raw);
}

void VG_BVI_VGLITE_free_resources(MICROUI_Image *image) {
	(void)_bvi_clear_blocks(MAP_BVI_ON_IMAGE(image));
}

jint VG_BVI_VGLITE_add_draw_path(
	void *target,
	const vg_lite_path_t *path,
	vg_lite_fill_t fill_rule,
	const vg_transformation_matrix *matrix,
	vg_lite_blend_t blend,
	vg_lite_color_t color) {
	// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_color_t for sure
	vg_block_bvi_color_t *op = (vg_block_bvi_color_t *)_alloc_data(sizeof(vg_block_bvi_color_t));
	jint ret = LLVG_OUT_OF_MEMORY;

	if (NULL != op) {
		vector_buffered_image_t *image = _bvi_get_bvi_from_target(target);

		op->header.path.block.kind = VG_BLOCK_BVI_COLOR;
		op->header.path.fill_rule = VG_VGLITE_HELPER_get_microvg_fill_rule(fill_rule);
		op->header.path.blend = VG_VGLITE_HELPER_get_microvg_blend(blend);
		op->header.path.desc = _bvi_store_native_path(path);
		op->header.color = (uint32_t)color;
		_bvi_save_current_native_scissor(op->scissor);
		_store_matrix(&(op->path_deformation), matrix);

		// cppcheck-suppress [misra-c2012-11.3] operation is a vg_block_t
		ret = _bvi_link_operations(image, (vg_block_t *)op, &(op->next), (NULL == op->header.path.desc));
	}

	return ret;
}

jint VG_BVI_VGLITE_add_draw_gradient(
	void *target,
	const vg_lite_path_t *path,
	vg_lite_fill_t fill_rule,
	const vg_transformation_matrix *matrix,
	const vg_gradient *grad,
	vg_lite_blend_t blend) {
	// cppcheck-suppress [misra-c2012-11.3] block is a vg_block_bvi_gradient_t for sure
	vg_block_bvi_gradient_t *op = (vg_block_bvi_gradient_t *)_alloc_data(sizeof(vg_block_bvi_gradient_t));
	jint ret = LLVG_OUT_OF_MEMORY;

	if (NULL != op) {
		vector_buffered_image_t *image = _bvi_get_bvi_from_target(target);

		op->header.path.block.kind = VG_BLOCK_BVI_GRADIENT;
		op->header.path.fill_rule = VG_VGLITE_HELPER_get_microvg_fill_rule(fill_rule);
		op->header.path.blend = VG_VGLITE_HELPER_get_microvg_blend(blend);
		op->header.path.desc = _bvi_store_native_path(path);

		if ((NULL != image->last_stored_gradient) && (0 == memcmp(grad, image->last_stored_gradient,
		                                                          GRADIENT_COPY_SIZE))) {
			// the both gradient operations use the same gradient
			// -> second operation can link the gradient of first operation
			op->header.gradient = image->last_stored_gradient;
			op->shared_gradient = true; // do not free it!
		} else {
			op->header.gradient = _bvi_store_native_gradient(grad);
			op->shared_gradient = false; // have to free it
			image->last_stored_gradient = op->header.gradient;
		}

		_bvi_save_current_native_scissor(op->scissor);
		_store_matrix(&(op->path_deformation), matrix);

		// cppcheck-suppress [misra-c2012-11.3] operation is a vg_block_t
		ret = _bvi_link_operations(image, (vg_block_t *)op, &(op->next),
		                           ((NULL == op->header.path.desc) || (NULL == op->header.gradient)));
	}

	return ret;
}

jint VG_BVI_VGLITE_add_draw_image(void *target, const MICROVG_Image *res, const jfloat *drawing_matrix,
                                  jint alpha, jlong elapsed, const float color_matrix[]) {
	// cppcheck-suppress [misra-c2012-11.5] res is a vector image for sure
	const vector_image_t *image = (const vector_image_t *)res->data;
	jint ret;

	if (_bvi_is_image_in_rom(image)) {
		// no need to parse the image: just add a block in the BVI
		vector_buffered_image_t *dest = _bvi_get_bvi_from_target(target);
		ret = _bvi_add_draw_raw_image(dest, image, drawing_matrix, alpha, elapsed, color_matrix);
	} else {
		// have to copy all image elements in the destination (== a bvi)
		// if the source image holds a RAW image block (points to a RAW image), each element of this
		// RAW image is drawn in the destination.

		_get_image_parameters(image, &drawing_data);
		bvi_target = target;
		ret = _draw_raw_image(&drawing_data, &_bvi_add_image_element, drawing_matrix, alpha, elapsed, color_matrix);

		/*
		 * When the target is a BufferedVectorImage, the data of render_gradient has been updated but not the VGLite
		 * image
		 * (it is useless and time consuming). However the image may be required later (if the new drawing's gradient
		 * data
		 * is equal to the stored gradient data). In this case, the VGLite image will be out of date (not synchronized
		 * with
		 * the gradient data) but will not be updated.
		 * A simple solution consists to ensure that next gradient comparison will fail.
		 */
		(void)memset(&render_gradient, 0, GRADIENT_CMP_SIZE);
	}

	return ret;
}

void LLVG_BVI_IMPL_map_context(MICROUI_Image *ui, MICROVG_Image *vg) {
	vg->data = (void *)MAP_BVI_ON_IMAGE(ui);
	vg->size = sizeof(vector_buffered_image_t);
}

void LLVG_BVI_IMPL_clear(MICROUI_GraphicsContext *gc) {
	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_BVI_IMPL_clear)) {
		// map a struct on graphics context's pixel area
		vector_buffered_image_t *image = MAP_BVI_ON_GC(gc);

		image->header.first_block = _bvi_clear_blocks(image); // reset first block
		image->latest_data = NULL; // reset to identify the very first block

		LLUI_DISPLAY_setDrawingStatus(DRAWING_DONE);
	}
}

#endif // #if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE

// -----------------------------------------------------------------------------
// Java library natives
// -----------------------------------------------------------------------------

bool VG_DRAWING_image_is_closed(const MICROVG_Image *image) {
	return (NULL == image) || (NULL == image->data) || (0u >= image->size);
}

void VG_DRAWING_get_image_size(const MICROVG_Image *image, float *width, float *height) {
	const vector_image_t *data = (const vector_image_t *)image->data;
	*width = data->width;
	*height = data->height;
}

jint Java_ej_microvg_VectorGraphicsNatives_getImage(char *path, jint path_length, MICROVG_Image *res, jint *metadata) {
	LOG_MICROVG_IMAGE_START(load);

	(void)path_length;

	jint ret;
	int32_t sni_ret = SNIX_get_resource(path, (SNIX_resource *)res);

#if VG_FEATURE_RAW_EXTERNAL
	// image not found in the application resource list
	if ((sni_ret < 0) && (LLVG_SUCCESS == _load_external_image(path, false, res, metadata))) {
		// image found available in external memory which is byte addressable
		sni_ret = 0;
	}
#endif // VG_FEATURE_RAW_EXTERNAL

	if (sni_ret < 0) {
		ret = LLVG_DATA_INVALID_PATH;
	}
	// cppcheck-suppress [misra-c2012-11.5] cast the resource in a u8 address
	else if (!_is_raw_image(res->data)) {
		ret = LLVG_DATA_INVALID;
	} else {
		// cppcheck-suppress [misra-c2012-11.5] resource is a vector image for sure
		const vector_image_t *image = (const vector_image_t *)res->data;

		// metadata used by the Java library during image opening
		_store_image_metadata(image, 0, metadata);

		ret = LLVG_SUCCESS;
	}

	LOG_MICROVG_IMAGE_END(load);
	return ret;
}

jint Java_ej_microvg_VectorGraphicsNatives_loadImage(char *path, jint path_length, MICROVG_Image *res, jint *metadata) {
	jint result = Java_ej_microvg_VectorGraphicsNatives_getImage(path, path_length, res, metadata);

#if VG_FEATURE_RAW_EXTERNAL
	if (LLVG_DATA_INVALID_PATH == result) {
		// try to load a non-byte addressable image (copy in ui heap)
		result = _load_external_image(path, true, res, metadata);
	}
#endif

	return result;
}

jint VG_DRAWING_VGLITE_draw_image(draw_image_element drawer, const MICROVG_Image *res, const jfloat *drawing_matrix,
                                  jint alpha, jlong elapsed, const float color_matrix[], bool *rendered) {
	// cppcheck-suppress [misra-c2012-11.5] res is a vector image for sure
	const vector_image_t *image = (const vector_image_t *)res->data;
	_get_image_parameters(image, &drawing_data);
	jint error = _draw_raw_image(&drawing_data, drawer, drawing_matrix, alpha, elapsed, color_matrix);
	*rendered = (LLVG_SUCCESS == error) && drawing_data.drawing_running;
	return error;
}

jint Java_ej_microvg_VectorGraphicsNatives_createImage(MICROVG_Image *source, MICROVG_Image *dest,
                                                       const float color_matrix[]) {
	LOG_MICROVG_IMAGE_START(create);

	// cppcheck-suppress [misra-c2012-11.5] source is a vector image for sure
	vector_image_t *image = (vector_image_t *)source->data;

	// the source is a RAW image (in ROM or RAM) but not a BVI (see BufferedVectorImage.filterImage())
	assert(!_is_image_bvi(image));

	dest->data = (void *)LLUI_DISPLAY_IMPL_imageHeapAllocate(source->size);
	jint ret;

	if (NULL != dest->data) {
		(void)memcpy(dest->data, source->data, source->size);
		dest->size = source->size;

		// cppcheck-suppress [misra-c2012-11.5] destination is a vector image for sure
		image = (vector_image_t *)dest->data;
		_tag_image_in_ram(image);

		_get_image_parameters(image, &drawing_data);
		_derive_raw_image(&drawing_data, color_matrix);

		ret = LLVG_SUCCESS;
	} else {
		ret = LLVG_OUT_OF_MEMORY;
	}

	LOG_MICROVG_IMAGE_END(create);
	return ret;
}

void Java_ej_microvg_VectorGraphicsNatives_closeImage(MICROVG_Image *res) {
	if (!VG_DRAWING_image_is_closed(res)) {
		LOG_MICROVG_IMAGE_START(close);

		// cppcheck-suppress [misra-c2012-11.5] cast res->data as uint8_t* is allowed
		if (_is_raw_image((uint8_t *)res->data)) {
			// have to free to vector resource image data
			// cppcheck-suppress [misra-c2012-11.5] cast the resource in a u8 address
			LLUI_DISPLAY_IMPL_imageHeapFree((uint8_t *)res->data);
		}
		// else: it is an image allocated on the Java heap or in a BVI: nothing to free

		LOG_MICROVG_IMAGE_END(close);

		res->data = NULL;
		res->size = 0;
	}
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
